/* -------------------------------------------------------------------- */
/*              BACnet Protocol Interpreter Version 15                  */
/* -------------------------------------------------------------------- */
/*                                                                      */
/* INITPI.C will be set up to branch to this routine when 802.3 packets */
/* are found with a DSAP of 0x82, 0x03(PTP), 0x04(MSTP); or ARCNET      */
/* packets are found with a Sytem Code of 0xCD.                         */
/*                   Steven T. Bushby, NIST                             */
/*                   H. Michael Newman, Cornell                         */
/*                   Joel Bender, Cornell                               */
/* -------------------------------------------------------------------- */

/*------------------------------------------------------------------------------
module:     BACSN015.cpp
last edit:  03-Sep-03 [002] LCX add new lines in the Detail View to display less tagging
                        information,but more actual data within each context or 
                        application tagged section.
            16-Jul-03 [001] LCX add more AL information to the summary line.
             5-Mar-10 JLH: rework and cleanup

TODO:
- Use a fixed width font in the detail tree to line up the indentations?
- Use color in the detail tree to highlight errors?
- In CListSummaryView::OnCustomdrawList, VTSPacketPtr::bErrorDecode true changes to error color
  AND: does (gag) string searches for "readproperty" etc. to set other colors.
  At least the search is case insensitive (the hard way)
  There seems to be no other use of SUMMARY_PACKET_COLOR[]
  SILLY COLOR USE: would seem to make more sense to distinguish between
  - lines output by the script
  - failures detected by the scripts
  - PDU types - Conf, Unconf, SimpleAck etc.
  - PDUs with encoding errors
  Would also let the colors be shared by the detail tree if desired.
-----------------------------------------------------------------------------*/ 

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vts.h>

#include "pi.h"

#include "StringTables.h"

//madanner 9/04, added hack to build RP and RPM obj/prop/values for EPICS panel
extern void EPICS_AddRPValue(long obj_id, int prop_id, int prop_index, char * pbuffer, int nLen );

namespace NetworkSniffer {

#include "bacproto.h"

#define max_confirmed_services   30  // through GetEventInformation, 135-2012 (rev 14)
#define max_unconfirmed_services 11  // through WriteGroup, 135-2012 (rev 14)

#define FW "-27"
#define ARC 0

static char outstr[880]; /* buffer for output string manipulation */
static unsigned int lenexp; /* expected length of data following context tag */
static unsigned int lenact; /* actual length of data following context tag */

extern struct pi_data *pi_data_bacnet_IP;       /* Pointer to PI data structure */
extern struct pi_data *pi_data_bacnet_ETHERNET; /* Pointer to PI data structure */
extern struct pi_data *pi_data_bacnet_ARCNET;   /* Pointer to PI data structure */
extern struct pi_data *pi_data_bacnet_MSTP;     /* Pointer to PI data structure */
extern struct pi_data *pi_data_bacnet_PTP;      /* Pointer to PI data structure */
extern struct pi_data *pi_data_bacnet_BVLL;     /* Pointer to PI data structure */
extern struct pi_data *pi_data_bacnet_NL;       /* Pointer to PI data structure */
extern struct pi_data *pi_data_bacnet_AL;       /* Pointer to PI data structure */
extern struct pi_data *pi_data_Message;         /* Pointer to PI data structure */

struct pi_data *pi_data_current;     /* Current ptr for BAC_SHOW functions */

#include "bacfuncs.h"

/**************************************************************************/
int interp_bacnet_IP( char *header, int length)  /* IP interpreter */
/**************************************************************************/
{
   /* Summary line? */
   if (pi_data_bacnet_IP->do_sum)
      interp_bacnet_BVLL( header + 6, length - 6 );

   /* Detail line? */
   if (pi_data_bacnet_IP->do_int) {
      pif_init(pi_data_bacnet_IP, header, length);
      pif_header(length, "IP Frame Detail");
      bac_show_bipaddr( "Source/Destination" );
      pif_show_space();

      interp_bacnet_BVLL( header + 6, length - 6 );
   }

   return length;
}

/**************************************************************************/
int interp_bacnet_ETHERNET( char *header, int length)  /* Ethernet interpreter */
/**************************************************************************/
{
   pif_init( pi_data_bacnet_ETHERNET, header, length );
   int len = (pif_get_byte(12) << 8) + pif_get_byte(13) - 3;

   /* Summary line? */
   if (pi_data_bacnet_ETHERNET->do_sum) {
     if (pif_get_byte(17) == 0x01)
        interp_bacnet_NL( header + 17, len );
     else {
        // This is bogus - BVLL on Ethernet without IP?
        // interp_bacnet_BVLL( header + 17, len );
        interp_bacnet_BVLL( header + 17, len );
     }
   } 

   /* Detail line? */
   if (pi_data_bacnet_ETHERNET->do_int) {
      pif_header( length, "Ethernet Frame Detail" );
      bac_show_enetaddr( "Destination" );
      bac_show_enetaddr( "Source" );
      
      bac_show_word_hl( "Length", "%u" );
      
      bac_show_byte( "DSAP", "X'%02X'" );
      bac_show_byte( "SSAP", "X'%02X'" );
      bac_show_byte( "LLC Control", "X'%02X'" );

      pif_show_space();

      if (pif_get_byte(0) == 0x01)
         interp_bacnet_NL( header + 17, len );
      else {
         // This is bogus - BVLL on Ethernet without IP?
         // interp_bacnet_BVLL( header + 17, len );
         pif_show_byte("Ethernet with BACnet LLC is not network version 1.  Has 0x%02X");
      }
   }

   return length;
}

/**************************************************************************/
int interp_bacnet_ARCNET( char *header, int length)  /* ARCNET interpreter */
/**************************************************************************/
{
   /* Summary line? */
   if (pi_data_bacnet_ARCNET->do_sum)
      sprintf( get_sum_line( pi_data_bacnet_ARCNET ), "ARCNET Frame" );

   /* Detail line? */
   if (pi_data_bacnet_ARCNET->do_int) {
      pif_init( pi_data_bacnet_ARCNET, header, length );
      pif_header( length, "ARCNET Frame Detail" );
      bac_show_byte( "Source", "%u" );
      bac_show_byte( "Destination", "%u" );

      bac_show_byte( "BACnet SC", "X'%02X'" );
      bac_show_byte( "DSAP", "X'%02X'" );
      bac_show_byte( "SSAP", "X'%02X'" );

      bac_show_byte( "LLC Control", "X'%02X'" );

      pif_show_space();

      if (pif_get_byte(0) == 0x01)
         interp_bacnet_NL( header + 6, length - 6 );
      else {
         // This is bogus - BVLL on ARCNET without IP?
         // interp_bacnet_BVLL( header + 6, length - 6 );
         pif_show_byte("ARCNET with BACnet LLC is not network version 1.  Has 0x%02X");
      }
   }

   return length;
}

/**************************************************************************/
int interp_bacnet_MSTP( char *header, int length)  /* MS/TP interpreter */
/**************************************************************************/
{
   unsigned int wbuff;        /* 2 octet buffer */
   unsigned char *ptr;        /* general purpose pointer */
   int data_length;           /* length of data field not including CRC */
   unsigned char frame_type;  /* PTP frame type */
   boolean crc_test;          /* TRUE if CRC is correct */
   int i;
   unsigned char dataValue;   /* used in data CRC check */
   unsigned char headerCRC;   /* used in header CRC check */
   unsigned int crc;          /* used in header CRC check */
//   unsigned int crcValue;     /* used in data CRC check */
   unsigned short crcValue;     /* used in data CRC check */
//   unsigned int crcLow;       /* used in data CRC check */
   unsigned short crcLow;       /* used in data CRC check */
   char *mstp_header;         /* points to beginning of MS/TP preamble */

   /* Summary line? */
   if (pi_data_bacnet_MSTP->do_sum) {
      interp_bacnet_NL( header + 8, length-10 );
//      sprintf (get_sum_line (pi_data_bacnet_MSTP), "BACnet MS/TP Frame");
   }   /* End of Summary Line */

   /* Detail line? */
   if (pi_data_bacnet_MSTP->do_int) {
      pif_init (pi_data_bacnet_MSTP, header, length);
      pif_header (length, "BACnet MS/TP Frame Detail");
      mstp_header = header;

      /* ----- interpret MS/TP frame ----- */
      wbuff = pif_get_word_hl(0);
      if(wbuff == 0x55FF)
         pif_show_word_hl("Preamble                      = X'%04X'");
      else
         pif_show_word_hl("Error: Preamble (X'55FF') Expected. Found  X'%04X'");

      /* frame type field */
      frame_type = pif_get_byte(0);
      switch(frame_type){
         case 0x00: pif_show_byte("Token Frame                   = X'%02X'");
                    break;
         case 0x01: pif_show_byte("Poll For Master Frame         = X'%02X'");
                    break;
         case 0x02: pif_show_byte("Reply To Poll For Master Frame= X'%02X'");
                    break;
         case 0x03: pif_show_byte("Test Request Frame            = X'%02X'");
                    break;
         case 0x04: pif_show_byte("Test Response Frame           = X'%02X'");
                    break;
         case 0x05: pif_show_byte("Data Expecting Reply Frame    = X'%02X'");
                    break;
         case 0x06: pif_show_byte("Data Not Expecting Reply Frame= X'%02X'");
                    break;
         case 0x07: pif_show_byte("Reply Postponed Frame         = X'%02X'");
                    break;
         default:   pif_show_byte("Error: Unknown Frame Type     = X'%02X'");
         };

      /* address fields */
      pif_show_byte("Destination Address           = X'%02X'");
      pif_show_byte("Source Address                = X'%02X'");


      /* length field */
      data_length = pif_get_word_hl(0);

      switch(frame_type){
         /* frames with no data */
         case 0x00: case 0x01: case 0x02: case 0x07:
                    if(data_length == 0)
                       bac_show_word_hl("Data Length","%u");
                     else
                       bac_show_word_hl("Error: Invalid Data Length","%u");
                    break;
         /* frames with data */
         case 0x03: case 0x04: case 0x05: case 0x06:
                    if( (((data_length + 10) == length) || ((data_length + 11) == length))
                       && (data_length <= 510) )
                       /* allow for optional pad octet but restrict to 510 octets of data */
                       bac_show_word_hl("Data Length","%u");
                    else
                       bac_show_word_hl("Error: Invalid Data Length","%u");
                    break;
         default:   /* unknown frame case */
                    if(data_length == 0){
                       bac_show_word_hl("Data Length","%u");
                       break;
                       };
                    if( (((data_length + 10) == length) || ((data_length + 11) == length))
                       && (data_length <= 510) )
                       /* allow for optional pad octet but restrict to 510 octets of data */
                       bac_show_word_hl("Data Length","%u");
                    else
                       bac_show_word_hl("Error: Invalid Data Length","%u");
                    break;

      }  /* end of frame_type switch */

      /* header CRC field */
      /* -- check the CRC -- */
      headerCRC = 0xff;
      ptr = (unsigned char *)(mstp_header + 2);  /* point to beginning frame type field */
      for(i=0; i<6; i++){  /* process frame type, address, length, and CRC */
         dataValue = *ptr++;
         crc = headerCRC ^ dataValue;
         crc = crc ^ (crc<<1) ^ (crc<<2) ^ (crc<<3)
                   ^ (crc <<4) ^ (crc<<5) ^ (crc<<6) ^ (crc <<7);
         headerCRC = (crc & 0xfe) ^ ((crc>>8) & 1);
      }; /* end of for loop */
      crc_test = (headerCRC == 0x55);
      if(crc_test)
         bac_show_byte("Header CRC Verified","%02X");
      else
         bac_show_byte("Error: Header CRC Failure","%02X");


      /* data field */
      if(data_length > 0){
         switch(frame_type){
            /* frames with no data */
            case 0x00: case 0x01: case 0x02: case 0x07:
                       bac_show_nbytes(data_length,"Error: No data expected for this frame type.");
                       break;
            case 0x03: case 0x04: /* Test Request or Test Response Frame */
                       bac_show_nbytes(data_length,"Test Data");
                       break;
            case 0x05: case 0x06: /* call network layer interpreter */
                       pif_show_space();
                       interp_bacnet_NL( header+=8, length - 10 );
                       break;
            };  /* end of frame_type switch */

         /* data CRC field */
         /* -- check the CRC -- */
         crcValue = 0xffff;
         ptr = (unsigned char *)(mstp_header + 8);  /* point to beginning data field */
         for(i=0; i< (data_length + 2); i++){  /* process all data incl CRC */
            dataValue = *ptr++;

            crcLow = (crcValue & 0xff) ^ dataValue;   /* XOR C7..C0 with D7..D0 */
            /* Exclusive OR the terms in the table (top down) */
            crcValue = (crcValue >> 8) ^ (crcLow << 8)   ^ (crcLow << 3)
                                   ^ (crcLow << 12)  ^ (crcLow >> 4)
                                   ^ (crcLow & 0x0f) ^((crcLow & 0x0f) << 7);
            }; /* end of for loop */
         crc_test = (crcValue == 0xf0b8);
         if(crc_test)
            bac_show_word_hl("Data CRC Verified","%04X");
         else
            bac_show_word_hl("Error: Data CRC Failure","%04X");
         }; /* end of if(data_length>0) */
      
      if(pif_offset < pif_end_offset) 
         bac_show_byte("Padding Octet","%02X");
   };   /* End of Detail Lines */
   return length;
}

/**************************************************************************/
int interp_bacnet_PTP( char *header, int length)  /* PTP interpreter */
/**************************************************************************/
{
   unsigned char buff;        /* 1 octet buffer for manipulation */
   unsigned int wbuff;        /* 2 octet buffer */
   unsigned char *ptr;        /* general purpose pointer */
   unsigned char *ptr1;       /* general purpose pointer */
   char trigger[8];           /* BACnet trigger string */
   char result_str[8];        /* data string */
   int data_length;           /* length of data field not including CRC */
   int len;                   /* data length field size */
   int len2;                  /* stuffed data field (or data CRC) size */
   unsigned char frame_type;  /* PTP frame type */
   boolean crc_test;          /* TRUE if CRC is correct */
   boolean byte1_stuffed;     /* TRUE if X'10' code found */
   boolean byte2_stuffed;     /* TRUE if X'10' code found */
   int i,j;                   /* loop index */

   unsigned char dataValue;   /* used in header and data CRC check */
   unsigned char headerCRC;   /* used in header CRC check */

   //unsigned int crc;          /* used in header CRC check */
   unsigned short crc;          // used in header CRC check. 2 bytes. Xiao Shiyuan 2002-5-17

   //unsigned int crcValue;     /* used in data CRC check */
   unsigned short crcValue;     // used in data CRC check. 2 bytes. Xiao Shiyuan 2002-5-17  
   //unsigned int crcLow;       /* used in data CRC check */
   unsigned short crcLow;       // used in data CRC check. 2 bytes. Xiao Shiyuan 2002-5-17
   char *ptp_header;          /* points to beginning of PTP preamble */

   strcpy(trigger, "BACnet");  /* initialize trigger string */
   /* Summary line? */
   if (pi_data_bacnet_PTP->do_sum) {
      sprintf (get_sum_line (pi_data_bacnet_PTP),
         "BACnet PTP Frame");
   }   /* End of Summary Line */

   /* Detail line? */
   BOOL flag = pi_data_bacnet_PTP->do_int; //xsy for test 2002-5-16

   if (pi_data_bacnet_PTP->do_int) {
      pif_init (pi_data_bacnet_PTP, header, length);
      pif_header (length, "BACnet PTP Frame Detail");
      ptp_header = header;

      /* ----- check for BACnet trigger sequence ----- */
     //0x0d == '\r'
     int x1 = *msg_origin; //xsy for test 2002-5-16
     int x2 = pif_offset; //xsy for test 2002-5-16
     
      if((length == 7) && (pif_get_byte(6) == 0x0d)){
         pif_get_ascii(0, 6, result_str, 6); //results are stored in "result_str"
         if(strcmp(result_str, trigger) == 0)  /* trigger found */
            bac_show_nbytes(7,"`BACnet<CR>' Trigger Sequence Found");
         }
      else{ /* not a trigger sequence */
         /* ----- interpret PTP frame ----- */
         wbuff = pif_get_word_hl(0);
       x2 = pif_offset; //xsy for test 2002-5-16
         if(wbuff == 0x55FF)
            pif_show_word_hl("Preamble                        = X'%04X'"); //pif_offset adds 2
         else
       {
          pif_show_word_hl("Error: PTP Preamble (X'55FF') Expected.  X'%04X'");
         return length; //when error occurs, return. xsy 2002-5-16
       }

         /* frame type field */
         frame_type = pif_get_byte(0);
       x2 = pif_offset; //xsy for test 2002-5-16
         switch(frame_type){
            case 0x00: pif_show_byte("Heartbeat (XOFF) Frame          = X'%02X'");
                       break;
            case 0x01: pif_show_byte("Heartbeat (XON) Frame           = X'%02X'");
                       break;
            case 0x02: pif_show_byte("Data Request 0 Frame            = X'%02X'");
                       break;
            case 0x03: pif_show_byte("Data Request 1 Frame            = X'%02X'");
                       break;
            case 0x04: pif_show_byte("Data Response 0 (XOFF) Frame    = X'%02X'");
                       break;
            case 0x05: pif_show_byte("Data Response 1 (XOFF) Frame    = X'%02X'");
                       break;
            case 0x06: pif_show_byte("Data Response 0 (XON) Frame     = X'%02X'");
                       break;
            case 0x07: pif_show_byte("Data Response 1 (XON) Frame     = X'%02X'");
                       break;
            case 0x08: pif_show_byte("Data Reject 0 Frame             = X'%02X'");
                       break;
            case 0x09: pif_show_byte("Data Reject 1 Frame             = X'%02X'");
                       break;
            case 0x0A: pif_show_byte("Connect Request Frame           = X'%02X'");
                       break;
            case 0x0B: pif_show_byte("Connect Response Frame          = X'%02X'");
                       break;
            case 0x0C: pif_show_byte("Disconnect Request Frame        = X'%02X'");
                       break;
            case 0x0D: pif_show_byte("Disconnect Response Frame       = X'%02X'");
                       break;
            case 0x0E: pif_show_byte("Test Request Frame              = X'%02X'");
                       break;
            case 0x0F: pif_show_byte("Test Response Frame             = X'%02X'");
                       break;
            default:   {
                  pif_show_byte("Error: Invalid Frame Type       = X'%02X'");
                  return length; //when error occurs, return. xsy 2002-5-16
                       }
            };

         // length field -- check for stuffed bytes, X'10' 
       //len means the length of length field
         if(pif_get_byte(0) == 0x10){
            byte1_stuffed = TRUE;
            if(pif_get_byte(2) == 0x10)
               byte2_stuffed = TRUE;
            else
               byte2_stuffed = FALSE;
            }
         else{
            byte1_stuffed = FALSE;
            if(pif_get_byte(1) == 0x10)
               byte2_stuffed = TRUE;
            else
               byte2_stuffed = FALSE;
            };

         if(byte1_stuffed && byte2_stuffed) {
            data_length = ((pif_get_byte(1) & 0x7f) *16) +
                          (pif_get_byte(3) & 0x7f);
            len = 4;
            };

         if(byte1_stuffed && (!byte2_stuffed)){
            data_length = ((pif_get_byte(1) & 0x7f) * 16) +
                          pif_get_byte(2);
            len = 3;
            };

         if((!byte1_stuffed) && byte2_stuffed){
            data_length = pif_get_byte(0) * 16 +
                          (pif_get_byte(2) & 0x7f);
            len = 3;
            };
         if((!byte1_stuffed) && (!byte2_stuffed)){
            len = 2;
            data_length = pif_get_word_hl(0);
            };

         switch(frame_type){
            /* frames with no data */
            case 0x00: case 0x01: case 0x04: case 0x05: case 0x06: case 0x07:
            case 0x08: case 0x09: case 0x0a: case 0x0d:
                       if(data_length == 0){
                          sprintf(outstr, "Data Length                 = %u", data_length);
                          bac_show_nbytes(len, outstr);
                          }
                        else{
                          sprintf(outstr, "Error: Invalid Data Length  = %u", data_length);
                          bac_show_nbytes(len, outstr);
                    return length; //when error occurs, return. xsy 2002-5-16
                          };
                       break;
            /* frames with data */
            case 0x02: case 0x03: case 0x0B: 
            case 0x0C: case 0x0E: case 0x0F: 
                       if((data_length + 8) <= length){ /* must allow byte stuffing in data */
                          sprintf(outstr, "Data Length                 = %u", data_length);
                          bac_show_nbytes(len, outstr);
                          }
                        else{
                          sprintf(outstr, "Error: Invalid Data Length  = %u", data_length);
                          bac_show_nbytes(len, outstr);
                    return length; //when error occurs, return. xsy 2002-5-16
                          };
                       break;
            };  /* end of frame_type switch */

         /* header CRC field */
         /* -- check the CRC -- */
         headerCRC = 0xff;
         ptr = (unsigned char *)(ptp_header + 2);  /* point to beginning frame type field */
         for(i=0; i<4; i++){  /* process frame type, length, and CRC */
            dataValue = *ptr++;
            if(dataValue == 0x10){  /* decode next octct */
               dataValue = *ptr++;
               dataValue = dataValue & 0x7f;
               };
            crc = headerCRC ^ dataValue;
            crc = crc ^ (crc<<1) ^ (crc<<2) ^ (crc<<3)
                      ^ (crc <<4) ^ (crc<<5) ^ (crc<<6) ^ (crc <<7);
            headerCRC = (crc & 0xfe) ^ ((crc>>8) & 1);
         }; /* end of for loop */
         crc_test = (headerCRC == 0x55);
         if(crc_test){
            if(pif_get_byte(0) == 0x10)
               bac_show_word_hl("Header CRC Verified", "%04X");
            else
               bac_show_byte("Header CRC Verified","%02X");          
            }
         else{
            if(pif_get_byte(0) == 0x10)
               bac_show_word_hl("Header CRC Failure", "%04X");
            else
               bac_show_byte("Error: Header CRC Failure","%02X");
            return length; //when error occurs, return. xsy 2002-5-16
            };


            /*  check the data CRC before decoding higher layers */
            crcValue = 0xffff;
            ptr = (unsigned char *)(ptp_header + len + 4);  /* point to beginning data field */
            for(i=0; i< (data_length + 2); i++){  /* process all data incl CRC */
               dataValue = *ptr++;
               if(dataValue == 0x10){  /* decode next octct */
                  dataValue = *ptr++;
                  dataValue = dataValue & 0x7f;
                  };

               crcLow = (crcValue & 0xff) ^ dataValue;   /* XOR C7..C0 with D7..D0 */
               /* Exclusive OR the terms in the table (top down) */
               crcValue = (crcValue >> 8) ^ (crcLow << 8)   ^ (crcLow << 3)
                                      ^ (crcLow << 12)  ^ (crcLow >> 4)
                                      ^ (crcLow & 0x0f) ^((crcLow & 0x0f) << 7);
               }; /* end of for loop */
            crc_test = (crcValue == 0xf0b8);

         /* data field */
         if(data_length > 0){
            switch(frame_type){
               case 0x00: case 0x01: case 0x04: case 0x05: case 0x06: case 0x07:
               case 0x08: case 0x09: case 0x0a: case 0x0d:
                          bac_show_nbytes(data_length,"Error: No data expected for this frame type.");
                          break;
               case 0x02: case 0x03: // call network layer interpreter 
                          // decode AL message before passing it on 
                          ptr = (unsigned char *)(ptp_header + len + 4);  // point to beginning data field 
                          ptr1 = ptr;
                          len2 = 0;
                          for(i=0; i<data_length; i++){
                            if(*ptr1 == 0x10){
                              len2++;
                              ptr1++;
                              *ptr = *ptr1 & 0x7f;
                              ptr++;
                              ptr1++;
                              }
                            else
                              *ptr++ = *ptr1++;
                            };
                          pif_show_space();
                          interp_bacnet_NL (ptp_header+len+4, data_length);
                          if(len2 > 0){
                            sprintf(outstr, "%d Encoded data octet(s) were found", len2);
                            bac_show_nbytes(len2,outstr);
                            };
                          break;
               case 0x0B: /* Connect Response Frame */
                          len2 = data_length;
                          j = 0;
                          for(i=0; i<data_length; i++){
                            /* account for stuffed data bytes */
                            if(pif_get_byte(j) == 0x10){
                               j++; /* increment to pass a stuffed X'10' */
                               len2++;
                               };
                            j++;
                            };
                          bac_show_nbytes(len2,"Password");
                          break;
               case 0x0C: /* Disconnect Request Frame  */
                          buff = pif_get_byte(0);
                          pif_show_space();
                          pif_show_ascii(0,"Reason for Disconnect");
                          switch(buff){
                           case 0x00: bac_show_byte("No more data to be transmitted","%u");
                                      break;
                           case 0x01: bac_show_byte("The peer process is being preempted","%u");
                                      break;
                           case 0x02: bac_show_byte("The received password is invalid","%u");
                                      break;
                           case 0x03: bac_show_byte("Other","%u");
                                      break;
                           default:   bac_show_byte("Error: Invalid Disconnect Reason.", "%u");
                           };
                          break;
               case 0x0E: case 0x0F: /* Test Request or Test Response Frame */
                          len2 = data_length;
                          j = 0;
                          for(i=0; i<data_length; i++){
                            /* account for stuffed data bytes */
                            if(pif_get_byte(j) == 0x10){
                               j++; /* increment to pass a stuffed X'10' */
                               len2++;
                               };
                            j++;
                            };
                          bac_show_nbytes(len2,"Test Data");
                          break;
               };  /* end of frame_type switch */

            /* data CRC field (CRC was tested before decoding data) */
            if(crc_test){ /* CRC is good */
               if((pif_get_byte(0) == 0x10) && (pif_get_byte(2) == 0x10)){
                  crcValue = ((pif_get_byte(1) & 0x7f)*16) +
                           (pif_get_byte(3) & 0x7f);
                  sprintf(outstr,  "Data CRC Verified           = %04X", crcValue);
                  len2 = 4;
                  };
               if((pif_get_byte(0) == 0x10) && (pif_get_byte(2) != 0x10)){
                  crcValue = ((pif_get_byte(1) & 0x7f)*16) +
                           pif_get_byte(2);
                  sprintf(outstr,  "Data CRC Verified           = %04X",crcValue);
                  len2 = 3;
                  };
               if((pif_get_byte(0) != 0x10) && (pif_get_byte(1) == 0x10)){
                  crcValue = (pif_get_byte(0) * 16) +
                           (pif_get_byte(2) & 0x7f);
                  sprintf(outstr,  "Data CRC Verified           = %04X",crcValue);
                  len2 = 3;
                  };
               if((pif_get_byte(0) != 0x10) && (pif_get_byte(1) != 0x10)){
                  sprintf(outstr,  "Data CRC Verified           = %04X",
                          pif_get_word_hl(0));
                  len2 = 2;
                  };
               bac_show_nbytes(len2, outstr);
               }
            else{ /* CRC is bad */
               if((pif_get_byte(0) == 0x10) && (pif_get_byte(2) == 0x10)){
                  crcValue = ((pif_get_byte(1) & 0x7f)*16) +
                           (pif_get_byte(3) & 0x7f);
                  sprintf(outstr,  "Error: Data CRC Failure     = %04X",crcValue);
                  len2 = 4;
                  };
               if((pif_get_byte(0) == 0x10) && (pif_get_byte(2) != 0x10)){
                  crcValue = ((pif_get_byte(1) & 0x7f)*16) +
                           pif_get_byte(2);
                  sprintf(outstr,  "Error: Data CRC Failure     = %04X",crcValue);
                  len2 = 3;
                  };
               if((pif_get_byte(0) != 0x10) && (pif_get_byte(2) == 0x10)){
                  crcValue = (pif_get_byte(0) * 16) +
                           (pif_get_byte(2) & 0x7f);
                  sprintf(outstr,  "Error: Data CRC Failure     = %04X",crcValue);
                  len2 = 3;
                  };
               if((pif_get_byte(0) != 0x10) && (pif_get_byte(2) != 0x10)){
                  sprintf(outstr,  "Error: Data CRC Failure     = %04X",
                          pif_get_word_hl(0));
                  len2 = 2;
                  };
               bac_show_nbytes(len2, outstr);
               };
            }; /* end of if(data_length>0) */
         };  /* end of else not a trigger sequence */
      };   /* End of Detail Lines */
   return length;
}

/**************************************************************************/
int interp_bacnet_BVLL( char *header, int length)  /* BVLL interpreter */
/**************************************************************************/
{
   pi_data_current = pi_data_bacnet_BVLL;

   if (pi_data_bacnet_BVLL->do_sum) {
      pif_init( pi_data_bacnet_BVLL, header, length );

      int version = pif_get_byte(0);
      switch (version) {
         case 0x81:
            switch (pif_get_byte(1)) {
               case 0x04:     // Forwarded-NPDU
                  interp_bacnet_NL( header + 10, length - 10 );
                  break;
               case 0x0A:     // Original-Unicast-NPDU
               case 0x0B:     // Original-Broadcast-NPDU
                  interp_bacnet_NL( header + 4, length - 4 );
                  break;
               default:
                  {
                     char *pStr = get_sum_line(pi_data_bacnet_BVLL);
                     int fn = pif_get_byte(1);
                     if (fn < BAC_STRTAB_BVLL_Function.m_nStrings)
                     {
                        strcpy( pStr, BAC_STRTAB_BVLL_Function.m_pStrings[fn] );
                     }
                     else
                     {
                        sprintf( pStr, "Unknown BVLC function %u", fn );
                     }
                  }
                  break;
            }
            break;
         case 0x01:
            // This formerly called interp_bacnet_NL( header, length );
            // But IP without BVLCI is NOT street-legal BACnet, so fall through to show error.
         default:
            {
               char *pStr = get_sum_line(pi_data_bacnet_BVLL);
               sprintf( pStr, "Expected BVLC Type 0x81.  Got 0x%02X", version );
            }
            break;
      }
   }

   if (pi_data_bacnet_BVLL->do_int) {
      pif_init( pi_data_bacnet_BVLL, header, length );
      pif_header( length, "BACnet Virtual Link Layer Detail" );

      switch (pif_get_byte(0)) {
         case 0x81:
            show_str_eq_str("BVLC Type","BACnet/IP",1);
            pif_offset += 1;
            switch (pif_get_byte(0)) {
               case 0x00:
                  pif_show_byte("BVLC Function               = %u  BVLC-Result");
                  bac_show_word_hl("BVLC Length","%u");
                  bac_show_word_hl("Result Code","%u");
                  pif_show_space();
                  break;
               case 0x01:
                  pif_show_byte("BVLC Function               = %u  Write-Broadcast-Distribution-Table");
                  bac_show_word_hl("BVLC Length","%u");
                  while (pif_offset < pif_end_offset) {
                     pif_show_space();
                     bac_show_bipaddr( "BDT Entry" );
                     bac_show_long_hl( "Distribution Mask", "X'%08X'" );
                  }
                  pif_show_space();
                  break;
               case 0x02:
                  pif_show_byte("BVLC Function               = %u  Read-Broadcast-Distribution-Table");
                  bac_show_word_hl("BVLC Length","%u");
                  pif_show_space();
                  break;
               case 0x03:
                  pif_show_byte("BVLC Function               = %u  Read-Broadcast-Distribution-Table-Ack");
                  bac_show_word_hl("BVLC Length","%u");
                  while (pif_offset < pif_end_offset) {
                     pif_show_space();
                     bac_show_bipaddr( "BDT Entry" );
                     bac_show_long_hl( "Distribution Mask", "X'%08X'" );
                  }
                  pif_show_space();
                  break;
               case 0x04:
                  pif_show_byte("BVLC Function               = %u  Forwarded-NPDU");
                  bac_show_word_hl("BVLC Length","%u");
                  pif_show_space();
                  bac_show_bipaddr("Originating Device");
                  pif_show_space();
                  interp_bacnet_NL( header + pif_offset, length - pif_offset );
                  break;
               case 0x05:
                  pif_show_byte("BVLC Function               = %u  Register-Foreign-Device");
                  bac_show_word_hl("BVLC Length","%u");
                  pif_show_space();
                  bac_show_word_hl("Time-To-Live","%u");
                  pif_show_space();
                  break;
               case 0x06:
                  pif_show_byte("BVLC Function               = %u  Read-Foreign-Device-Table");
                  bac_show_word_hl("BVLC Length","%u");
                  pif_show_space();
                  break;
               case 0x07:
                  pif_show_byte("BVLC Function               = %u  Read-Foreign-Device-Table-Ack");
                  bac_show_word_hl("BVLC Length","%u");
                  while (pif_offset < pif_end_offset) {
                     pif_show_space();
                     bac_show_bipaddr( "FDT Entry" );
                     bac_show_word_hl( "Time-To-Live", "%u" );
                     bac_show_word_hl( "Time-Remaining", "%u" );
                  }
                  pif_show_space();
                  break;
               case 0x08:
                  pif_show_byte("BVLC Function               = %u  Delete-Foreign-Device-Table-Entry");
                  bac_show_word_hl("BVLC Length","%u");
                  pif_show_space();
                  bac_show_bipaddr( "FDT Entry" );
                  pif_show_space();
                  break;
               case 0x09:
                  pif_show_byte("BVLC Function               = %u  Distribute-Broadcast-To-Network");
                  bac_show_word_hl("BVLC Length","%u");
                  pif_show_space();
                  interp_bacnet_NL( header + pif_offset, length - pif_offset );
                  break;
               case 0x0A:
                  pif_show_byte("BVLC Function               = %u  Original-Unicast-NPDU");
                  bac_show_word_hl("BVLC Length","%u");
                  pif_show_space();
                  interp_bacnet_NL( header + pif_offset, length - pif_offset );
                  break;
               case 0x0B:
                  pif_show_byte("BVLC Function               = %u  Original-Broadcast-NPDU");
                  bac_show_word_hl("BVLC Length","%u");
                  pif_show_space();
                  interp_bacnet_NL( header + pif_offset, length - pif_offset );
                  break;
         default:
            pif_show_byte("Unknown BVLC Function               = %u");
            break;
         }
         break;
      case 0x01:
         // This formerly called interp_bacnet_NL( header, length );
         // But IP without BVLCI is NOT street-legal BACnet, so fall through to show error.
         // pif_show_space();
         // return interp_bacnet_NL( header, length );
      default:
         pif_show_byte("Unknown BVLC Type               = 0x%02X");
         break;
      }
   }

   return length;
}

/**************************************************************************/
int interp_bacnet_NL( char *header, int length)  /* Network Layer interpreter */
/**************************************************************************/
{
   unsigned char buff;      /* 1 octet buffer for manipulation */
   unsigned short wbuff;    /* 2 octet buffer */
   unsigned char APDUmask;  /* APDU vs Network Layer flag mask */
   unsigned char RESV1mask; /* Reserved bit mask */
   unsigned char DESTmask;  /* DNET, DLEN, DADR, and Hop Count flag mask */
   unsigned char RESV2mask; /* Reserved bit mask */
   unsigned char SOURCEmask;/* SNET, SLEN, AND SADR flag mask */
   unsigned char DERmask;   /* Data Expecting Reply mask */
   int mac_length;          /* MAC address length in octets */
   int npdu_length;         /* NPDU length in octets */
   int apdu_length;         //APDU length in octets. Xiao Shiyuan 2002-5-19
   int port_id;             /* port ID field */
   int port_info_length;    /* port information length in octets */
   const char *name;        /* translated name */
   char namebuff[MAX_TEXT_STRING]; /* buffer to print translated name */

   pi_data_current = pi_data_bacnet_NL;
   npdu_length = 2;
   /* Summary line? */
   if (pi_data_bacnet_NL->do_sum) {
      /* Figure out length of NPCI */
      pif_init(pi_data_bacnet_NL, header, length);

      if(length < 2)
      {
         strcpy( get_sum_line(pi_data_bacnet_NL), "Invalid NPDU (too short)" );
         return length;
      }

      buff = pif_get_byte(1);
      pif_skip(2);                    /* Point after control octet */
      DESTmask = (buff & 0x20 )>>5;   /* mask for DNET, DLEN, DADR, & Hop Count present switch */
      SOURCEmask = (buff & 0x08)>>3;  /* mask for SNET, SLEN, and SADR present switch */
      if (DESTmask == 1) {
         npdu_length += 4; // DNET, DLEN, & Hop Count ARE present   
         pif_skip(2);      /* Bypass DNET, point to DLEN */      
         npdu_length += pif_get_byte(0); /* Add length of DADR and skip over */
         pif_skip(1 + pif_get_byte(0));
      }
      if (SOURCEmask == 1) {
         npdu_length += 3; // SNET, SLEN ARE present 
         pif_skip(2);      // Bypass SNET, point to SLEN 
         npdu_length += pif_get_byte(0); //Add length of SADR and skip over
         pif_skip(1 + pif_get_byte(0));
      }

      if (buff & 0x80) { 
         // Network Message
         if (DESTmask == 1) /* skip over hop count */
            pif_skip(1);

         int nlMsgID = pif_get_byte(0) & 0xFF; // MessageType
         if (nlMsgID <= 0x80)
         {
            // Show type from table, or "Unknown..." for values not in the table
            strcpy( get_sum_line(pi_data_bacnet_NL), BAC_STRTAB_NL_msgs.EnumString( nlMsgID, "Unknown network message type:" ) );
         }
         else
         {  
            // Proprietary network message must have vendor ID
            UINT vendorID = pif_get_word_hl(1);
            pif_skip(3);      // Point past message type and vendorID to data
            npdu_length += 2;

            sprintf( namebuff, "Proprietary network message %d (vendor %u)", nlMsgID, vendorID );
            strcpy( get_sum_line(pi_data_bacnet_NL), namebuff );        
         }
      }
      else {
         // BACnet APDU
         //Xiao Shiyuan 2002-5-19. BEGIN
         apdu_length = length - npdu_length;
         if(apdu_length <= 0 || npdu_length > length || npdu_length > 1497) //npdu length error
         {
            strcpy( get_sum_line(pi_data_bacnet_NL), "Error: NPDU length" );
            return length;
         }
         //Xiao Shiyuan 2002-5-19. END
         return npdu_length + interp_bacnet_AL( header + npdu_length, length - npdu_length );
      }
   }   /* End of Summary Line */

   /* Detail line? */
   if (pi_data_bacnet_NL->do_int) {
      pif_init(pi_data_bacnet_NL, header, length);
      pif_header(length, "BACnet Network Layer Detail");

      if (length < 2)
      {
         pif_show_ascii(0, "Error NPDU" );
         return length;       
      }

      pif_show_byte("Protocol Version Number = %d");

      /* --- display the network control octet detail --- */
      buff = pif_get_byte(0);
      APDUmask = (buff & 0x80)>>7;       //mask  APDU/Network control bit (7).Xiao Shiyuan 2002-5-17
      RESV1mask = (buff & 0x40)>>6;   /* reserved mask */
      DESTmask = (buff & 0x20 )>>5;   /* mask for DNET, DLEN, DADR, & Hop Count present switch */
      RESV2mask = (buff & 0x10)>>4;   /* reserved mask */
      SOURCEmask = (buff & 0x08)>>3;  /* mask for SNET, SLEN, and SADR present switch */
      DERmask = (buff & 0x04)>>2;     /* data expecting reply mask */

      /*  ----- show control octet  ------  */
      sprintf(outstr,"%"FW"s = X'%%02X'","Network Control Octet");
      bac_show_flag(outstr,0xFF);
      pif_show_flagbit(0x80,"BACnet Network Control Message", "BACnet APDU");
      pif_show_flagbit(0x40,"Reserved","Reserved");
      pif_show_flagbit(0x20,"DNET,DLEN,DADR & Hop Count Present","DNET,DLEN,DADR & Hop Count Omitted");
      pif_show_flagbit(0x10,"Reserved","Reserved");
      pif_show_flagbit(0x08,"SNET,SLEN, & SADR Present","SNET,SLEN, & SADR Omitted");
      pif_show_flagbit(0x04,"Data Expecting Reply","No Reply Expected");
      pif_show_flagmask(0x03, 0x03, "Life Safety Message");
      pif_show_flagmask(0x03, 0x02, "Critical Equipment Message");
      pif_show_flagmask(0x03, 0x01, "Urgent Message");
      pif_show_flagmask(0x03, 0x00, "Normal Message");

      /*  ----- show DNET, DLEN and DADR if present ----- */
      if (DESTmask == 1) {
         wbuff = pif_get_word_hl(0);
         if(wbuff == 65535) {
            if ((name = LookupName( 65535, 0, 0 )) != 0) {
               sprintf( namebuff, "Global Broadcast DNET          = X'%%04X', %s", name );
               pif_show_word_hl(namebuff);
            } else
               pif_show_word_hl("Global Broadcast DNET          = X'%04X'");
         } else {
            pif_show_word_hl("Destination Network Number     = %u");
         }

         mac_length = pif_get_byte(0);
         pif_show_byte("Destination MAC address length = %u");

         if (wbuff != 65535)
            name = LookupName( wbuff, (const unsigned char *)msg_origin + pif_offset, mac_length );
         else
            name = 0;

         if(mac_length > 0) {
            pif_show_nbytes_hex("Destination MAC address        = X'%s'",mac_length);
         }
         else {  /* Broadcast MAC address (not shown in PDU) */
            pif_show_ascii(0,"Broadcast MAC address implied");
         }

         if (name) pif_append_ascii( ", %s", name );
      }

      /*  ----- show SNET, SLEN and SADR if present ----- */
      if (SOURCEmask == 1) {
         wbuff = pif_get_word_hl(0);
         pif_show_word_hl("Source Network Number          = %u");

         mac_length = pif_get_byte(0);
         pif_show_byte("Source MAC address length      = %u");

         name = LookupName( wbuff, (const unsigned char *)msg_origin + pif_offset, mac_length );

         pif_show_nbytes_hex("Source MAC address             = X'%s'", mac_length);
         if(mac_length <= 0)
            pif_show_ascii(0,"Invalid MAC address length!");

         if (name) pif_append_ascii( ", %s", name );
      }

      /*  ----- show Hop Count if DNET present ----- */
      if (DESTmask == 1) pif_show_byte("Hop Count                   = %d");

      /*  -----  APDU or network layer message ? ----  */
      if(APDUmask == 0) {  /* BACnet APDU in message, call AL interpreter */
         pif_show_space();
         npdu_length = pif_offset;

         //Xiao Shiyuan 2002-5-19. BEGIN
         apdu_length = length - npdu_length;
         if(apdu_length <= 0 || npdu_length > length || npdu_length > 1497) //npdu length error
         {
            pif_show_ascii(0, "Error NPDU" );
            return length;
         }
         //Xiao Shiyuan 2002-5-19. END

         return npdu_length + interp_bacnet_AL( header + npdu_length, length - npdu_length );
      }  /* end of APDU present case */

      else {   /* BACnet network control message */

         /*  ----- show message type ----- */
         buff = pif_get_byte(0);
         if(buff >= 0x80){
            pif_show_byte(   "Proprietary Message Type    = %u");
            pif_show_word_hl("Vendor ID                   = %u");
            // TODO: instead, get the two-byte int and show the vendorID string?
            // pif_show_ascii( BAC_STRTAB_BACnetVendorID.EnumString( pif_get_byte(0), "vendor:" )
         }
         else{
            switch (buff) {   /* show appropriate NL interpretation */
            case 0x00:  /* Who-Is-Router-To-Network */
                        pif_show_byte("Message Type                = %u  Who-Is-Router-To-Network");
                        if(pif_offset < pif_end_offset) {   /* network address present */
                           sprintf(outstr,"%"FW"s = %%u","Network Number");
                           pif_show_word_hl(outstr);
                        }
                        break;
            case 0x01:  /*  I-Am-Router-To-Network  */
                        pif_show_byte("Message Type                = %u  I-Am-Router-To-Network");
                        while(pif_offset < pif_end_offset) { /* display network numbers */
                           sprintf(outstr,"%"FW"s = %%u","Network Number");
                           pif_show_word_hl(outstr);
                        }
                        break;
            case 0x02:  /*  I-Could-Be-Router-To-Network  */
                        pif_show_byte("Message Type                = %u  I-Could-Be-Router-To-Network");
                        sprintf(outstr,"%"FW"s = %%u","Network Number");
                        pif_show_word_hl(outstr);
                        pif_show_byte("Performance Index           = %u");
                        break;
            case 0x03:  /* Reject-Message-To-Network */
                        pif_show_byte("Message Type                = %u  Reject-Message-To-Network");
                        sprintf(outstr,"%"FW"s = X'%%02X'","Reject Reason");
                        bac_show_flag(outstr,0xFF);
                        pif_show_flagmask(0x07, 0x00, "Other");
                        pif_show_flagmask(0x07, 0x01, "Unknown DNET");
                        pif_show_flagmask(0x07, 0x02, "Router Busy");
                        pif_show_flagmask(0x07, 0x03, "Unknown Message Type");
                        pif_show_flagmask(0x07, 0x04, "Msg too long for this DNET");
                        pif_show_flagmask(0x07, 0x05, "Security error");
                        pif_show_flagmask(0x07, 0x06, "Address error in DADR or SADR");
                        pif_show_word_hl("Network Number              = %u");
                        break;
            case 0x04:  /* Router-Busy-To-Network */
                        pif_show_byte("Message Type                = %u  Router-Busy-To-Network");
                        if (length > 3) {   /* DNET included */
                           while(pif_offset < pif_end_offset) { /* display net numbers */
                              sprintf(outstr,"%"FW"s = %%u","Network Number");
                              pif_show_word_hl(outstr);
                           }
                        }
                        else {
                           sprintf(pif_line(0),"Busy to all networks");
                        }
                        break;
            case 0x05:  /* Router-Available-To-Network */
                        pif_show_byte("Message Type                = %u  Router-Available-To-Network");
                        if (length > 3) {   /* DNET included */
                           while(pif_offset < pif_end_offset) { /* display net numbers */
                              sprintf(outstr,"%"FW"s = %%u","Network Number");
                              pif_show_word_hl(outstr);
                           }
                        }
                        else {
                           sprintf(pif_line(0), "Available to all networks");
                        }
                        break;
            case 0x06:  /* Initialize-Routing-Table */
                        pif_show_byte("Message Type                = %u  Initialize-Routing-Table");
                        pif_show_byte("Number of Ports             = %u");
                        if (pif_offset == pif_end_offset) {
                           pif_show_space();
                           pif_show_ascii(0, "Initialize-Routing-Table Query");
                        }
                        else {
                           while(pif_offset < pif_end_offset) { /* process data */
                              sprintf(outstr,"%"FW"s = %%u","Connected DNET");
                              pif_show_word_hl(outstr);
                              port_id = pif_get_byte(0);
                              if(port_id == 0)
                                 pif_show_byte("Port ID Number              = %u (remove entry)");
                              else
                                 pif_show_byte("Port ID Number              = %u");
                              port_info_length = pif_get_byte(0);
                              pif_show_byte("Port Info Length            = %u");
                              bac_show_nbytes(port_info_length,"Port Information");
                           }
                        }
                        break;
            case 0x07:  /* Initialize-Routing-Table-Ack */
                        pif_show_byte("Message Type                = %u  Initialize-Routing-Table-Ack");
                        if (pif_offset == pif_end_offset) {
                           pif_show_space();
                           pif_show_ascii(0, "Simple Initialize-Routing-Table-Ack without data");
                        }
                        else {
                           pif_show_byte("Number of Ports             = %u");
                           while(pif_offset < pif_end_offset) { /* process data */
                              sprintf(outstr,"%"FW"s = %%u","Connected DNET");
                              pif_show_word_hl(outstr);
                              pif_show_byte("Port ID Number              = %u");
                              port_info_length = pif_get_byte(0);
                              pif_show_byte("Port Info Length            = %u");
                              bac_show_nbytes(port_info_length,"Port Information");
                           }
                        }
                        break;
            case 0x08:  /* Establish-Connection-To-Network */
                        pif_show_byte("Message Type                = %u  Establish-Connection-To-Network");
                        sprintf(outstr,"%"FW"s = %%u","Network Number");
                        pif_show_word_hl(outstr);
                        if(pif_get_byte(0) == 0)
                           pif_show_byte("Permanent Connection");
                        else
                           pif_show_byte("Termination Time         = %u seconds");
                        break;
            case 0x09:  /* Disconnect-Connection-To-Network */
                        pif_show_byte("Message Type                = %u  Disconnect-Connection-To-Network");
                        sprintf(outstr,"%"FW"s = %%u","Network Number");
                        pif_show_word_hl(outstr);
                        break;

            case 0x0A:  pif_show_byte("Message Type                = %u  Challenge-Request");
                        // TODO: Decode the body
                        break;
            case 0x0B:  pif_show_byte("Message Type                = %u  Security-Payload");
                        // TODO: Decode the body
                        break;
            case 0x0C:  pif_show_byte("Message Type                = %u  Security-Response");
                        // TODO: Decode the body
                        break;
            case 0x0D:  pif_show_byte("Message Type                = %u  Request-Key-Update");
                        // TODO: Decode the body
                        break;
            case 0x0E:  pif_show_byte("Message Type                = %u  Update-Key-Set");
                        // TODO: Decode the body
                        break;
            case 0x0F:  pif_show_byte("Message Type                = %u  Update-Distribution-Key");
                        // TODO: Decode the body
                        break;
            case 0x10:  pif_show_byte("Message Type                = %u  Request-Master-Key");
                        // TODO: Decode the body
                        break;
            case 0x11:  pif_show_byte("Message Type                = %u  Set-Master-Key");
                        // TODO: Decode the body
                        break;

            case 0x12:  /* What is network number */
                        pif_show_byte("Message Type                = %u  What-Is-Network-Number");
                        break;
            case 0x13:  /* Network-Number-Is*/
                        pif_show_byte(   "Message Type                = %u  Network-Number-Is");
                        sprintf(outstr,"%"FW"s = %%u","Network Number");
                        pif_show_word_hl(outstr);
                        if(pif_get_byte(0) == 0)
                           pif_show_byte("Learned:                      %u");
                        else
                           pif_show_byte("Configured:                   %u");
                        break;
            default:    // Show type from table, or "Unknown..." for values not in the table
                        sprintf(outstr,"%"FW"s = %%u", 
                                BAC_STRTAB_NL_msgs.EnumString( buff, "Unknown network message type:" ));
                        pif_show_byte(outstr);
                        break;
            }  /* End of switch */
         } /* End of if proprietary message type else branch */
          
         pif_show_space();
      }  /* End of else network control message*/
   }   /* End of Detail Lines */
   return length;
}

/**************************************************************************/
int interp_bacnet_AL( char *header, int length )  /* Application Layer interpreter */
/**************************************************************************/
/* This function is the Application Layer interpreter for BACnet  */
{
   unsigned char buff;   /* 1 octet buffer for manipulation */
   unsigned char x,ID,SEG,MOR,SEQ,NAK,SRV;
   unsigned char pdu_type;
   unsigned char service_choice;
   unsigned int len, tagbuff;

   char moreDetail[500];

   /* Summary line? */
   if (pi_data_bacnet_AL->do_sum) {
      pif_init (pi_data_bacnet_AL, header, length);
      pdu_type = pif_get_byte(0)>>4; /*Get APDU type */
      x = (pif_get_byte(0)&0x08)>>3; /* x = 1 implies header is 2 bytes longer */
      /* Set up to do cool stuff in summary based on PDU type and service choice */
      // TODO: could I buy somebody a function-per-case here?  Please?
      // TODO: do the 3+x*2 schtick ONCE and let everyone start at the tagged PDU.
      // Update each service to use "tagged", then ditch "x"
      unsigned int tagged; // offset to first tagged byte
      switch (pdu_type) {                                                     //  ***001 begin
      case 0: /* Confirmed service request */
         service_choice = pif_get_byte(3+x*2);
         tagged         = 4 + 2*x;
         ID = pif_get_byte(2);
         SEG = x;
         if (SEG) {
            MOR = (pif_get_byte(0)&0x04)>>2;
            SEQ = pif_get_byte(3);
            sprintf(moreDetail,"Segment %u %s", SEQ, (MOR) ? "More follows" : "Final segment" );
         }
         else {
            switch (service_choice) {
            case 0:   /* Acknowledge Alarm Request */
               {
                  tagbuff = pif_get_byte( 3+x*2 +1 );
                  len = tagbuff & 0x07;
                  unsigned long nValue = get_bac_unsigned( 3+x*2+2, len );

                  long  obj_id;
                  int      obj_type;
                  long  obj_instance;
                  
                  for (int i = 0; i < 4; i++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( i+3+x*2+3+len );
                  
                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);
                  sprintf(moreDetail, "%lu, %s_%lu", nValue, ObjectTypeString(obj_type), obj_instance);
               }
               break;
            case 1: /* Confirmed COV Notification Request */
               {
                  tagbuff = pif_get_byte( 3+x*2 +1 );
                  len = tagbuff & 0x07;
                  unsigned long nValue = get_bac_unsigned( 3+x*2+2, len );
                  
                  long  device_id, obj_id;
                  int   device_type, obj_type;
                  long  device_instance, obj_instance;
                  
                  for (int i = 0; i < 4; i++)
                     device_id = (device_id << 8) | (unsigned char)pif_get_byte( i+4+2*x+len+2 );
                  
                  device_type = (device_id >> 22) & 0x000003FF;
                  device_instance = (device_id & 0x003FFFFF);
                  
                  for (int j = 0; j < 4; j++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( j+4+2*x+len+2+5 );
                  
                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);
                  sprintf(moreDetail,"%lu, %s_%lu, %s_%lu", nValue, 
                          ObjectTypeString(device_type), device_instance, 
                          ObjectTypeString(obj_type), obj_instance);
               }
               break;
            case 2: /* Confirmed Event Notification Request */
               {
                  tagbuff = pif_get_byte( 3+x*2 +1 );
                  len = tagbuff & 0x07;
                  unsigned long nValue = get_bac_unsigned( 3+x*2+2, len );
                  
                  long  device_id, obj_id;
                  int   device_type, obj_type;
                  long  device_instance, obj_instance;
                  
                  for (int i = 0; i < 4; i++)
                     device_id = (device_id << 8) | (unsigned char)pif_get_byte( i+4+2*x+len+2 );

                  device_type = (device_id >> 22) & 0x000003FF;
                  device_instance = (device_id & 0x003FFFFF);
                  
                  for (int j = 0; j < 4; j++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( j+4+2*x+len+2+5 );
                  
                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);
                  
                  sprintf(moreDetail,"%lu, %s_%lu, %s_%lu", nValue, 
                          ObjectTypeString(device_type), device_instance, 
                          ObjectTypeString(obj_type), obj_instance);
               }
               break;
            case 5: /* Subscribe COV Request */
               {
                  tagbuff = pif_get_byte( 3+x*2 +1 );
                  len = tagbuff & 0x07;
                  unsigned long nValue = get_bac_unsigned( 3+x*2+2, len );

                  long  obj_id;
                  int      obj_type;
                  long  obj_instance;
                  
                  for (int i = 0; i < 4; i++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( i+3+x*2+3+len );

                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);

                  int nBool = pif_get_byte( 3+x*2+3+len+5 );
                  char strBool[20];
                  if(nBool == 1){
                     sprintf(strBool, "TRUE");
                  }
                  else if(nBool == 0){
                     sprintf(strBool, "FALSE");
                  }
                  else{
                     sprintf(strBool, "Error");
                  }

                  sprintf(moreDetail, "%lu, %s_%lu, %s", nValue, 
                          ObjectTypeString(obj_type), obj_instance, strBool);
               }
               break;
            case 6: /* Atomic Readfile Request */
               {
                  long  obj_id;
                  int      obj_type;
                  long  obj_instance;
                  
                  for (int i = 0; i < 4; i++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( i+3+x*2+2 );

                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);
                  sprintf(moreDetail, "%s_%lu", ObjectTypeString(obj_type), obj_instance);
               }
               break;
            case 7: /* Atomic Writeflle Request */
               {
                  long  obj_id;
                  int      obj_type;
                  long  obj_instance;

                  for (int i = 0; i < 4; i++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( i+3+x*2+2 );
                  
                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);
                  sprintf(moreDetail, "%s_%lu", ObjectTypeString(obj_type), obj_instance);
               }
               break;
            case 8: /* Add List Element Request */
               {
                  // "tagged" is offset of first tag byte
                  unsigned obj_id = get_bac_unsigned( tagged+1, 4 );
                  unsigned obj_type = (obj_id >> 22) & 0x000003FF;
                  unsigned obj_instance = (obj_id & 0x003FFFFF);
                  tagged += 5;

                  len = pif_get_byte( tagged ) & 0x07;
                  unsigned int propID = get_bac_unsigned( tagged+1, len );
                  tagged += 1+len;

                  // TODO: show optional index?

                  sprintf(moreDetail, "%s_%lu, %s", ObjectTypeString(obj_type), obj_instance, PropertyIdentifierString(propID));
               }
               break;
            case 9: /* Remove List Element Request */
               {
                  // "tagged" is offset of first tag byte
                  unsigned obj_id = get_bac_unsigned( tagged+1, 4 );
                  unsigned obj_type = (obj_id >> 22) & 0x000003FF;
                  unsigned obj_instance = (obj_id & 0x003FFFFF);
                  tagged += 5;
                  
                  len = pif_get_byte( tagged ) & 0x07;
                  unsigned int propID = get_bac_unsigned( tagged+1, len );
                  tagged += 1+len;

                  // TODO: show optional index?

                  sprintf(moreDetail, "%s_%lu, %s", ObjectTypeString(obj_type), obj_instance, PropertyIdentifierString(propID));
               }
               break;
            case 10: /* Create Object Request */
               {
                  int tagval;
                  tagbuff = pif_get_byte( 3+x*2 +2 );
                  len = tagbuff & 0x07;
                  tagval = (tagbuff&0xF0)>>4;

                  switch(tagval){
                  case 0:
                     {
                        unsigned long nType = get_bac_unsigned( 3+x*2+3, len );
                        sprintf(moreDetail, "%s", ObjectTypeString(nType));
                     }
                     break;
                  case 1:
                     {
                        long  obj_id;
                        int      obj_type;
                        long  obj_instance;
                        
                        for (int i = 0; i < 4; i++)
                           obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( i+3+x*2+3 );
                        
                        obj_type = (obj_id >> 22) & 0x000003FF;
                        obj_instance = (obj_id & 0x003FFFFF);

                        sprintf(moreDetail, "%s_%lu", ObjectTypeString(obj_type), obj_instance);
                     }
                     break;
                  default:
                     {
                        sprintf(moreDetail, "Error");
                     }
                     break;
                  }
               }
               break;
            case 11: /* Delete Object Request */
               {
                  // "tagged" is offset of first tag byte
                  unsigned obj_id = get_bac_unsigned( tagged+1, 4 );
                  unsigned obj_type = (obj_id >> 22) & 0x000003FF;
                  unsigned obj_instance = (obj_id & 0x003FFFFF);

                  sprintf(moreDetail, "%s_%lu", ObjectTypeString(obj_type), obj_instance);
               }
               break;
            case 12: /* Read Property Request */
               {
                  // "tagged" is offset of first tag byte
                  unsigned obj_id = get_bac_unsigned( tagged+1, 4 );
                  unsigned obj_type = (obj_id >> 22) & 0x000003FF;
                  unsigned obj_instance = (obj_id & 0x003FFFFF);
                  tagged += 5;
                  
                  len = pif_get_byte( tagged ) & 0x07;
                  unsigned int pID = get_bac_unsigned( tagged+1, len );
                  tagged += 1+len;

                  tagbuff = pif_get_byte(tagged);
                  if((tagbuff & 0xF8) == 0x28) {
                     len = tagbuff & 0x07;
                     unsigned int index = get_bac_unsigned(tagged+1, len);
                     tagged += 1+len;
                     sprintf(moreDetail, "%s_%u, %s, Index = %u", ObjectTypeString(obj_type), obj_instance, PropertyIdentifierString(pID),index);
                  }
                  else {
                     sprintf(moreDetail, "%s_%u, %s", ObjectTypeString(obj_type), obj_instance, PropertyIdentifierString(pID));
                  }
               }
               break;
            case 15: /* Write Property Request */
               //Modified by Zhu Zhenhua, 2004-12-21, for task #544511
               {
                  // "tagged" is offset of first tag byte
                  unsigned obj_id = get_bac_unsigned( tagged+1, 4 );
                  unsigned obj_type = (obj_id >> 22) & 0x000003FF;
                  unsigned obj_instance = (obj_id & 0x003FFFFF);
                  tagged += 5;

                  len = pif_get_byte( tagged ) & 0x07;
                  unsigned int pID = get_bac_unsigned( tagged+1, len );
                  tagged += 1+len;

                  int lentemp;
                  unsigned int pid_index;
                  tagbuff = pif_get_byte(tagged);
                  if((tagbuff & 0xF8) == 0x28) {
                     len = tagbuff & 0x07;
                     pid_index = (int)get_bac_unsigned(tagged+1, len);
                     tagged += 1+len;
                     lentemp = sprintf(moreDetail, "%s_%u, %s, Index %d", ObjectTypeString(obj_type), obj_instance, PropertyIdentifierString(pID),pid_index);
                  }
                  else {
                     lentemp = sprintf(moreDetail, "%s_%u, %s", ObjectTypeString(obj_type), obj_instance, PropertyIdentifierString(pID));
                     pid_index = -1;
                  }

                  // Skip over opening [3]
                  // DO NOT parse it: we want this first line of decoding to be the VALUE
                  pif_offset = tagged + 1;
                     
                  // TODO: this dumps a bunch of lines into the detailLine, and then
                  // assumes that the first value line will be of the form
                  // "some text: value" and show the part to the right of the colon.
                  // Seems fragile: any change to string format will break the code.
                  BACnetSequence seq;
                  show_bac_ANY(seq, obj_type, pID, pid_index);
                  if (gCurrentInfo->detailCount > 0)
                  {
                     const char *pValue = strchr( gCurrentInfo->detailLine[0]->piLine, ':' );
                     if (pValue != NULL)
                     {   
                        SafeCopy( moreDetail + lentemp, ",", sizeof(moreDetail) - lentemp );
                        lentemp += 1;
                        SafeCopy( moreDetail + lentemp, pValue+1, sizeof(moreDetail) - lentemp );
                     }
                  }

                  // Eat the closing tag, else ~BACnetSequence complains 
                  // about unparsed APDU
                  pif_offset += 1;
               }
               break;
            case 18: /* Confirmed Private Transfer Request */
               {
                  int len1,len2,tagbuff;
                  tagbuff = pif_get_byte( 3+x*2 +1 );
                  len1 = tagbuff & 0x07;
                  unsigned long nValue1 = get_bac_unsigned( 3+x*2+2, len1 );

                  tagbuff = pif_get_byte( 3+x*2 +2+len1 );
                  len2 = tagbuff & 0x07;
                  unsigned long nValue2 = get_bac_unsigned( 3+x*2 +3+len1, len2 );

                  sprintf(moreDetail, "%lu, %lu", nValue1, nValue2);
               }
               break;
            case 19: /* Confirmed Text Message Request */
               {
                  long  obj_id;
                  int      obj_type;
                  long  obj_instance;
                  
                  for (int i = 0; i < 4; i++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( i+3+x*2+2 );
                  
                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);
                  
                  sprintf(moreDetail, "%s_%lu", ObjectTypeString(obj_type), obj_instance);
               }
               break;
            case 23: /* VT Data Request */
               {
                  int len,tagbuff;
                  tagbuff = pif_get_byte( 3+x*2 +1 );
                  len = tagbuff & 0x07;
                  unsigned long nValue = get_bac_unsigned( 3+x*2+2, len );

                  sprintf(moreDetail, "%lu", nValue);
               }
               break;
            default:
               moreDetail[0] = 0;
               break;
            }     // End switch on confirmed service choice
         }
         sprintf(outstr,"%s, ID=%u %s",
                 BAC_STRTAB_BACnetConfirmedServiceChoice.EnumString(service_choice, "ConfRQ" ),
                 ID, moreDetail);
         break;
      case 1: /* Unconfirmed service request */
            service_choice = pif_get_byte(1);
            tagged         = 2;
            switch (service_choice) {
            case 0: /* I-Am */
               {
                  unsigned obj_id;
                  unsigned obj_type;
                  unsigned obj_instance;

                  for (int i = 0; i < 4; i++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( i+3 );

                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);
                  sprintf(moreDetail, "%s_%lu", ObjectTypeString(obj_type), obj_instance);
               }
               sprintf(outstr,"%s, %s", 
                       BAC_STRTAB_BACnetUnconfirmedServiceChoice.EnumString( service_choice, "UnConf" ), 
                       moreDetail);

//             sprintf(outstr,"%s, %lu",BACnetUnconfirmedServiceChoice[service_choice], 
//                     pif_get_long_hl(3)&0x003FFFFF);
               break;
            case 1: /* I-Have */
               {
                  unsigned device_id, obj_id;
                  unsigned device_type, obj_type;
                  unsigned device_instance, obj_instance;

                  for (int i = 0; i < 4; i++)
                     device_id = (device_id << 8) | (unsigned char)pif_get_byte( i+3 );

                  device_type = (device_id >> 22) & 0x000003FF;
                  device_instance = (device_id & 0x003FFFFF);

                  for (int j = 0; j < 4; j++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( j+8 );

                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);

                  sprintf(outstr,"%s, %s_%lu, %s_%lu", 
                          BAC_STRTAB_BACnetUnconfirmedServiceChoice.EnumString( service_choice, "UnConf" ),
                          ObjectTypeString(device_type), device_instance, 
                          ObjectTypeString(obj_type), obj_instance);
               }
               break;
            case 2: /* Unconfirmed COV Notification  Request */
               {
                  int len,tagbuff;
                  tagbuff = pif_get_byte( 2 );
                  len = tagbuff & 0x07;
                  unsigned long nValue = get_bac_unsigned( 3, len );

                  unsigned device_id, obj_id;
                  unsigned device_type, obj_type;
                  unsigned device_instance, obj_instance;

                  for (int i = 0; i < 4; i++)
                     device_id = (device_id << 8) | (unsigned char)pif_get_byte( i+4+len );

                  device_type = (device_id >> 22) & 0x000003FF;
                  device_instance = (device_id & 0x003FFFFF);

                  for (int j = 0; j < 4; j++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( j+4+len+5 );

                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);

                  sprintf(outstr, "%s, %lu, %s_%lu, %s_%lu", 
                          BAC_STRTAB_BACnetUnconfirmedServiceChoice.EnumString( service_choice, "UnConf" ), 
                          nValue, 
                          ObjectTypeString(device_type), device_instance, 
                          ObjectTypeString(obj_type), obj_instance);
                }
                break;
            case 3: /* Unconfirmed Event Notification Request */
               {
                  int len,tagbuff;
                  tagbuff = pif_get_byte( 2 );
                  len = tagbuff & 0x07;
                  unsigned long nValue = get_bac_unsigned( 3, len );

                  unsigned device_id, obj_id;
                  unsigned device_type, obj_type;
                  unsigned device_instance, obj_instance;

                  for (int i = 0; i < 4; i++)
                     device_id = (device_id << 8) | (unsigned char)pif_get_byte( i+4+len );

                  device_type = (device_id >> 22) & 0x000003FF;
                  device_instance = (device_id & 0x003FFFFF);

                  for (int j = 0; j < 4; j++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( j+4+len+5 );

                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);

                  sprintf(outstr, "%s, %lu, %s_%lu, %s_%lu", 
                          BAC_STRTAB_BACnetUnconfirmedServiceChoice.EnumString( service_choice, "UnConf" ), 
                          nValue, 
                          ObjectTypeString(device_type), device_instance, 
                          ObjectTypeString(obj_type), obj_instance);
               }
               break;
            case 4: /* Unconfirmed Private Transfer Request */
               {
                  int len1,len2,tagbuff;
                  tagbuff = pif_get_byte( 2 );
                  len1 = tagbuff & 0x07;
                  unsigned long nValue1 = get_bac_unsigned( 3, len1 );

                  tagbuff = pif_get_byte( 3+len1 );
                  len2 = tagbuff & 0x07;
                  unsigned long nValue2 = get_bac_unsigned( 4+len1, len2 );

                  sprintf(outstr, "%s, %lu, %lu", 
                          BAC_STRTAB_BACnetUnconfirmedServiceChoice.EnumString( service_choice, "UnConf" ),
                          nValue1, nValue2);
               }
               break;
            case 5: /* Unconfirmed Text Message Request */
               {
                  unsigned obj_id;
                  unsigned obj_type;
                  unsigned obj_instance;

                  for (int i = 0; i < 4; i++)
                     obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( i+3 );

                  obj_type = (obj_id >> 22) & 0x000003FF;
                  obj_instance = (obj_id & 0x003FFFFF);

                  sprintf(outstr, "%s, %s_%lu", 
                          BAC_STRTAB_BACnetUnconfirmedServiceChoice.EnumString( service_choice, "UnConf" ),
                          ObjectTypeString(obj_type), obj_instance);
               }
               break;
            case 7: /* Who-Has */
               {
                  unsigned char tagbuff, tagval; /* buffers for tags and tag values */
                  int len,newLen;
                  char temp[200] = "";
                  int flag = 0;

                  tagbuff = pif_get_byte(2);
                  tagval = (tagbuff&0xF0)>>4;

                  if (tagbuff & 0x08) { /* context tag */
                     if(tagval == 0){ /* device instance range expected */
                        flag = 1;
                        len = tagbuff & 0x07;
                        unsigned long nLow = get_bac_unsigned( 3, len );
                        tagbuff = pif_get_byte( 3+len );
                        tagval = (tagbuff&0xF0)>>4;

                        if( tagval == 1 ){
                           newLen = tagbuff & 0x07;
                           unsigned long nHigh = get_bac_unsigned( 4+len, newLen );
                           tagbuff = pif_get_byte( 4+len+newLen );
                           tagval = (tagbuff&0xF0)>>4;
                           sprintf(temp, "%lu-%lu", nLow, nHigh);
                        }
                        else{
                           sprintf(moreDetail, "Error!");
                        }
                     }

                     switch(tagval) {
                     case 2: /* object Identifier*/
                        {
                           unsigned  obj_id;
                           unsigned  obj_type;
                           unsigned  obj_instance;
                           int nOffset;

                           if(flag)
                              nOffset = 5+len+newLen;
                           else
                              nOffset = 3;

                           for (int i = 0; i < 4; i++)
                              obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( i+nOffset );

                           obj_type = (obj_id >> 22) & 0x000003FF;
                           obj_instance = (obj_id & 0x003FFFFF);

                           if(flag)
                           {
                              sprintf(moreDetail, "%s, %s_%lu", temp, ObjectTypeString(obj_type), obj_instance);
                           }
                           else
                           {
                              sprintf(moreDetail, "%s_%lu", ObjectTypeString(obj_type), obj_instance);
                           }
                        }
                        break;
                     case 3: /* object name */
                        {
                           int nOffset;
                           if(flag)
                              nOffset = len+newLen+6;
                           else
                              nOffset = 4;

                           len = tagbuff & 0x07;
                           if(len == 5)
                              len = pif_get_byte(nOffset-1);

                           char strBuff[MAX_INT_LINE];
                           switch(pif_get_byte(nOffset)){
                           case 0: /* ASCII */
                              pif_get_ascii(nOffset+1, len-1, strBuff, sizeof(strBuff)-1);
                              break;
                           case 1: /* MS DBCS */
                              {
                                 static char  hex[] = "0123456789ABCDEF";
                                 char *dst;
                                 dst = strBuff;
                                 len-=1;
                                 int i = nOffset+1;
                                 while (len--) {
                                    int x = pif_get_byte(i);
                                    *dst++ = hex[ (x >> 4) & 0x0F ];
                                    *dst++ = hex[ x & 0x0F ];
                                    i++;
                                 }
                                 *dst = 0;
                              }
                              break;
                           default:
                              sprintf(strBuff, "Error!");
                              break;
                           }

                           if(flag)
                              sprintf(moreDetail, "%s, %s", temp, strBuff);
                           else
                              sprintf(moreDetail, "%s", strBuff);
                        }
                        break;
                     default: 
                        sprintf(moreDetail, "Error!");
                        break;
                     }
                  }
                  else{
                     sprintf(moreDetail, "Error!");
                  }

                  sprintf(outstr,"%s, %s",
                          BAC_STRTAB_BACnetUnconfirmedServiceChoice.EnumString( service_choice, "UnConf" ), 
                          moreDetail);
               }
               break;
            case 8: /* Who-Is */
               if (pif_offset+2>=pif_end_offset)
                  sprintf(outstr,"%s, All Devices",BAC_STRTAB_BACnetUnconfirmedServiceChoice.EnumString( service_choice, "UnConf" ));
               else {
                  sprintf(outstr,"%s, %lu-%lu",BAC_STRTAB_BACnetUnconfirmedServiceChoice.EnumString( service_choice, "UnConf" ),
                  get_bac_unsigned(3,pif_get_byte(2)&0x07),
                  get_bac_unsigned(3+(pif_get_byte(2)&0x07)+1,pif_get_byte(3+(pif_get_byte(2)&0x07))&0x07));
                }
                break;
            default: 
               sprintf(outstr,"%s",BAC_STRTAB_BACnetUnconfirmedServiceChoice.EnumString( service_choice, "UnConf" ));
               break;
            }    // End switch on unconfirmed service choice
            break;
      case 2: /* Simple ACK */
         {
            ID = pif_get_byte(1);
            int service_type = pif_get_byte(2);
            sprintf(moreDetail, "%s ACK", BAC_STRTAB_BACnetConfirmedServiceChoice.EnumString(service_type, "SimpleAck"));
            sprintf(outstr,"%s, ID=%u, %s",BAC_STRTAB_PDU_types.EnumString(pdu_type, "PDU_Type"),ID,moreDetail);
         }
         break;
      case 3: /* Complex ACK */
         {
            ID = pif_get_byte(1);
            int nService = pif_get_byte(2 + 2*x);
            tagged = 3 + 2*x;
            SEG = x;
            if (SEG) {
               MOR = (pif_get_byte(0)&0x04)>>2;
               SEQ = pif_get_byte(2);
               sprintf(moreDetail,"Segment %u %s", SEQ, (MOR) ? "More follows" : "Final segment" );
            }
            else {
               switch(nService) {
               //Modified by Zhu Zhenhua, 2004-12-21, for task #544511
               case 12: /* Read Property Ack */   
                  {
                     // "tagged" is offset of first tag byte
                     unsigned obj_id = get_bac_unsigned( tagged+1, 4 );
                     unsigned obj_type = (obj_id >> 22) & 0x000003FF;
                     unsigned obj_instance = (obj_id & 0x003FFFFF);
                     tagged += 5;

                     len = pif_get_byte( tagged ) & 0x07;
                     unsigned int pID = get_bac_unsigned( tagged+1, len );
                     tagged += 1+len;

                     int lentemp;
                     unsigned int pid_index = -1;
                     tagbuff = pif_get_byte(tagged);
                     if((tagbuff & 0xF8) == 0x28) {
                        len = tagbuff & 0x07;
                        pid_index = (int)get_bac_unsigned(tagged+1, len);
                        tagged += 1+len;
                        lentemp = sprintf(moreDetail, "%s_%u, %s, Index = %d", ObjectTypeString(obj_type), obj_instance, PropertyIdentifierString(pID),pid_index);
                     }
                     else {
                        lentemp = sprintf(moreDetail, "%s_%u, %s", ObjectTypeString(obj_type), obj_instance, PropertyIdentifierString(pID));
                        pid_index = -1;
                     }

                     // Bypass the opening [3]
                     // DO NOT parse it: we want this first line of decoding to be the VALUE
                     pif_offset = tagged + 1;

                     // TODO: this dumps a bunch of lines into the detailLine, and then
                     // assumes that the first value line will be of the form
                     // "some text: value" and show the part to the right of the colon.
                     // Seems fragile: any change to string format will break the code.
                     BACnetSequence seq;
                     show_bac_ANY(seq, obj_type, pID, pid_index);
                     if (gCurrentInfo->detailCount > 0)
                     {
                        const char *pValue = strchr( gCurrentInfo->detailLine[0]->piLine, ':' );
                        if (pValue != NULL)
                        {   
                           SafeCopy( moreDetail + lentemp, ",", sizeof(moreDetail) - lentemp );
                           lentemp += 1;
                           SafeCopy( moreDetail + lentemp, pValue+1, sizeof(moreDetail) - lentemp );
                        }
                     }

                     // Eat the closing tag, else ~BACnetSequence complains 
                     // about unparsed APDU
                     pif_offset += 1;
                  }
                  break;
               default:
                  moreDetail[0] = 0;
                  break;
               }     // End switch on Complex-Ack service choice
            }

            sprintf(outstr,"%s-ACK, ID=%u, %s",
                    BAC_STRTAB_BACnetConfirmedServiceChoice.EnumString(nService,"ConfRQ"),
                    ID, moreDetail);
         }
         break;
      case 4: /* Segment ACK */
         ID = pif_get_byte(1);
         NAK = (pif_get_byte(0) & 0x02) >> 1;
         SRV = pif_get_byte(0) & 0x01;
         sprintf(outstr,"%s ID=%u NAK=%u SRV=%u SEQ=%u",
                 BAC_STRTAB_PDU_types.EnumString(pdu_type,"PDU_Type"), 
                 ID, NAK, SRV, pif_get_byte(2));
         break;
      case 5: /* Error */
         {
            ID = pif_get_byte(1);
            int error_type = pif_get_byte(2);
            tagged         = 3;
            sprintf(moreDetail, "%s", BAC_STRTAB_BACnetError.EnumString(error_type,"Error"));
            sprintf(outstr,"%s, ID=%u, %s",BAC_STRTAB_PDU_types.EnumString(pdu_type,"PDU_Type"), ID, moreDetail);
         }
         break;
      case 6: /* Reject */
         {
            ID = pif_get_byte(1);
            int reject_type = pif_get_byte(2);
            sprintf(moreDetail, "%s", BAC_STRTAB_BACnetReject.EnumString(reject_type,"Reject"));
            sprintf(outstr,"%s, ID=%u, %s",BAC_STRTAB_PDU_types.EnumString(pdu_type,"PDU_Type"), ID, moreDetail);
         }
         break;
      case 7: /* Abort */
         {
            ID = pif_get_byte(1);
            int abort_type = pif_get_byte(2);
            sprintf(moreDetail, "%s", BAC_STRTAB_BACnetAbort.EnumString(abort_type,"Abort"));
            sprintf(outstr,"%s, ID=%u, %s",BAC_STRTAB_PDU_types.EnumString(pdu_type,"PDU_Type"), ID, moreDetail);
         }
         break;
      }  // End billion line switch on PDU type

      // LJT - Added check that our output string does not exceed our summary line maximum
      if ( strlen(outstr) >= MAX_SUM_LINE )
      {
         // truncate our outstr with '...' on the end
         outstr[MAX_SUM_LINE-4]='.';
         outstr[MAX_SUM_LINE-3]='.';
         outstr[MAX_SUM_LINE-2]='.';
         outstr[MAX_SUM_LINE-1]='\0';
      }
      SafeCopy( get_sum_line(pi_data_bacnet_AL), outstr, MAX_SUM_LINE );
/*
      sprintf (get_sum_line (pi_data_bacnet_AL),
               "BACnet AL (%s)",outstr);
*/
   }   /* End of Summary Line */

   /* Set up for get_int_line calls */
   pi_data_current = pi_data_bacnet_AL;

   /* Detail line? */
   if (pi_data_bacnet_AL->do_int) {
      pif_init (pi_data_bacnet_AL, header, length);
      pif_header (length, "BACnet Application Layer Detail");
      buff = pif_get_byte(0);
      x = (buff & 0xF0)>>4;      /* mask header for APDU type */
      switch (x) {
      case 0: show_confirmed(buff);
         break;
      case 1: show_unconfirmed(buff);
         break;
      case 2: show_simple_ack(buff);
         break;
      case 3: show_complex_ack(buff);
         break;
      case 4: show_segment_ack(buff);
         break;
      case 5: show_error(buff);
         break;
      case 6: show_reject(buff);
         break;
      case 7: show_abort(buff);
         break;
      default: bac_show_byte("Error: Unknown PDU Type","%u");
         break;
      };

      pif_show_space();
   }   /* End of Detail Lines */

   return length;
}  /* End of Application Layer Interpreter */

/**************************************************************************/
int interp_Message( char *header, int length)  /* message interpreter */
/**************************************************************************/
{
   /* Summary line? */
   if (pi_data_Message->do_sum)
      SafeCopy( get_sum_line(pi_data_Message), header + 21, MAX_SUM_LINE );

   /* Detail line? */
   if (pi_data_bacnet_IP->do_int) {
      pif_init(pi_data_Message, header, length);
      pif_header(length, "Message Detail");
      bac_show_byte("Severity Code","%u");            // 1 byte
      bac_show_long_hl("Script Line","%u");           // 4 bytes
      pif_show_nbytes_hex( "Digest                      = %s", 16 ); // 16 bytes
      pif_show_space();
      pif_show_ascii( strlen(pif_get_addr()), "%s" ); // offset 21
      pif_show_space();
   }

   return length;
}

/**************************************************************************/
// Simple text message
int interp_TextMessage( char *header, int length)  /* message interpreter */
/**************************************************************************/
{
   /* Summary line? */
   if (pi_data_Message->do_sum)
      SafeCopy( get_sum_line(pi_data_Message), header, MAX_SUM_LINE );
   
   /* Detail line? */
   if (pi_data_bacnet_IP->do_int) {
      pif_init(pi_data_Message, header, length);
      pif_header(length, "Message Detail");
      pif_show_space();
      pif_show_ascii( strlen(pif_get_addr()), "%s" );
      pif_show_space();
   }
   
   return length;
}

/***************************************************************************/
void show_confirmed( unsigned char x )
/***************************************************************************/

/*  This function interprets BACnet confirmed services */

{
   /* -- define array of ptrs to confirmed service interpreter functions -- */

   void (*show_confirmed_service[max_confirmed_services])() = {
      show_acknowledgeAlarm,            /* 0 */
      show_confirmedCOVNotification,    /* 1 */
      show_confirmedEventNotification,  /* 2 */
      show_getAlarmSummary,             /* 3 */
      show_getEnrollmentSummary,        /* 4 */
      show_subscribeCOV,                /* 5 */
      show_atomicReadFile,              /* 6 */
      show_atomicWriteFile,             /* 7 */
      show_addListElement,              /* 8 */
      show_removeListElement,           /* 9 */
      show_createObject,                /* 10 */
      show_deleteObject,                /* 11 */
      show_readProperty,                /* 12 */
      show_readPropertyConditional,     /* 13 */
      show_readPropertyMultiple,        /* 14 */
      show_writeProperty,               /* 15 */
      show_writePropertyMultiple,       /* 16 */
      show_deviceCommunicationControl,  /* 17 */
      show_privateTransfer,             /* 18 */
      show_confirmedTextMessage,        /* 19 */
      show_reinitializeDevice,          /* 20 */
      show_vtOpen,                      /* 21 */
      show_vtClose,                     /* 22 */
      show_vtData,                      /* 23 */
      show_authenticate,                /* 24 */
      show_requestKey,                  /* 25 */
      show_ReadRange,                   /* 26 */
      NULL,                             /* 27 TODO: implement show_LifeSafetyOperation */
     show_subscribeCOV_Property,        /* 28 */
     show_getEventInformation           /* 29 */
   };

   /* --- display the confirmed service header detail --- */

   sprintf(outstr,"%"FW"s = X'%%02X'","First Header Octet");
   bac_show_flag(outstr,0xFF);
   pif_show_flagmask(0xF0,0x00,"BACnet-Confirmed-Request-PDU");
   pif_show_flagbit(0x08,"Segmented Message","Unsegmented Message");
   pif_show_flagbit(0x04,"More Follows","No More Follows");
   pif_show_flagbit(0x03,"Segmented Resp Accepted","Segmented Resp not Accepted");
   
   sprintf(outstr,"%"FW"s = X'%%02X'","MaxSegments/MaxResponse");
   bac_show_flag(outstr,0xFF);

   pif_show_flagmask(0x70, 0x00,"Unspecified number of segments accepted");
   pif_show_flagmask(0x70, 0x10,"2 segments accepted");
   pif_show_flagmask(0x70, 0x20,"4 segments accepted");
   pif_show_flagmask(0x70, 0x30,"8 segments accepted");
   pif_show_flagmask(0x70, 0x40,"16 segments accepted");
   pif_show_flagmask(0x70, 0x50,"32 segments accepted");
   pif_show_flagmask(0x70, 0x60,"64 segments accepted");
   pif_show_flagmask(0x70, 0x70,"More than 64 segments accepted");
   
   pif_show_flagmask(0x0F, 0x00,"Up to 50 Octets");
   pif_show_flagmask(0x0F, 0x01,"Up to 128 Octets");
   pif_show_flagmask(0x0F, 0x02,"Up to 206 Octets");
   pif_show_flagmask(0x0F, 0x03,"Up to 480 Octets");
   pif_show_flagmask(0x0F, 0x04,"Up to 1024 Octets");
   pif_show_flagmask(0x0F, 0x05,"Up to 1476 Octets");

   bac_show_byte("Invoke ID","%d");
   if (x & 0x08)  {               /* SEG = 1 */
     bac_show_byte("Sequence Number","%d");
     bac_show_byte("Proposed Window Size","%d");

    int service_type = pif_get_byte(0);
    sprintf(get_int_line(pi_data_current,pif_offset,1), "Service Choice %s",
           BAC_STRTAB_BACnetConfirmedServiceChoice.EnumString(service_type, "ConfRq"));
    pif_offset++;

     bac_show_nbytes( pif_end_offset - pif_offset, "[segmented data]" );
   }
   else
   {
      pif_show_space();
      if ((pif_get_byte(0) >= max_confirmed_services) ||
         (show_confirmed_service[pif_get_byte(0)] == NULL)) 
     {
         bac_show_byte("Error: Unsupported Confirmed Service","%u");
     }
      else {
         (*show_confirmed_service[pif_get_byte(0)])(); /* call the service interpreter function */
     }
   }
}

/***************************************************************************/
void show_unconfirmed( unsigned char )
/***************************************************************************/
  /* This function interprets BACnet unconfirmed services */
{
   void (*show_unconfirmed_service[max_unconfirmed_services])() = {
      show_iAm,                         /* 0 */
      show_iHave,                       /* 1 */
      show_unconfirmedCOVNotification,  /* 2 */
      show_unconfEventNotification,     /* 3 */
      show_unconfPrivateTransfer,       /* 4 */
      show_unconfTextMessage,           /* 5 */
      show_timeSynchronization,         /* 6 */
      show_whoHas,                      /* 7 */
      show_whoIs,                       /* 8 */
      show_UTCtimeSynchronization       /* 9 */
   };
   
   switch(pif_get_byte(1))                                                       //  ***002 begin
   {
   case 0:
      sprintf(get_int_line(pi_data_current,pif_offset,2,NT_ITEM_HEAD), "I-Am Request");
      break;
   case 1:
      sprintf(get_int_line(pi_data_current,pif_offset,2,NT_ITEM_HEAD), "I-Have Request");
      break;
   case 2:
      sprintf(get_int_line(pi_data_current,pif_offset,2,NT_ITEM_HEAD), "Unconfirmed COV Notification Request");
      break;
   case 3:
      sprintf(get_int_line(pi_data_current,pif_offset,2,NT_ITEM_HEAD), "Unconfirmed Event Notification Request");
      break;
   case 4:
      sprintf(get_int_line(pi_data_current,pif_offset,2,NT_ITEM_HEAD), "Unconfirmed Private Transfer Request");
      break;
   case 5:
      sprintf(get_int_line(pi_data_current,pif_offset,2,NT_ITEM_HEAD), "Unconfirmed Text Message Request");
      break;
   case 6:
      sprintf(get_int_line(pi_data_current,pif_offset,2,NT_ITEM_HEAD), "Time Synchronization Request");
      break;
   case 7:
      sprintf(get_int_line(pi_data_current,pif_offset,2,NT_ITEM_HEAD), "Who-Has Request");
      break;
   case 8:
      sprintf(get_int_line(pi_data_current,pif_offset,2,NT_ITEM_HEAD), "Who-Is Request");
      break;
   case 9:
      sprintf(get_int_line(pi_data_current,pif_offset,2,NT_ITEM_HEAD), "UTC Time Synchronization Request");
      break;
   default:
      // TODO: complain?
      break;
   }                                                                       //  ***002 end

   sprintf(outstr,"%"FW"s = X'%%02X'","First Header Octet");
   bac_show_flag(outstr,0xFF);
   pif_show_flagmask(0xF0,0x10,"BACnet-Unconfirmed-Request-PDU");
   pif_show_flagbit(0x0F,"Unused",NULL);
   
   pif_show_space();
   /* call the unconfirmed service interpreter function */
   if (pif_get_byte(0) >= max_unconfirmed_services)
      bac_show_byte("Error: Unknown Unconfirmed Service","%u");
   else
      (*show_unconfirmed_service[pif_get_byte(0)])();
}

/**************************************************************************/
void show_simple_ack( unsigned char )
/**************************************************************************/
{
   sprintf(outstr,"%"FW"s = X'%%02X'","First Header Octet");
   bac_show_flag(outstr,0xFF);
   pif_show_flagmask(0xF0,0x20,"BACnet-SimpleACK-PDU");
   pif_show_flagbit(0x0F,"Unused",NULL);
   bac_show_byte("Invoke ID","%d");
   bac_show_byte( BAC_STRTAB_BACnetConfirmedServiceChoice.EnumString(pif_get_byte(0), "ConfRq"),"%d");
}

/*************************************************************************/
void show_complex_ack( unsigned char x )
{
   void (*show_confirmed_service_ACK[max_confirmed_services])(void) = {
      0,0,0,
      show_getAlarmSummaryACK,         /* 3 */
      show_getEnrollmentSummaryACK,    /* 4 */
      0,
      show_atomicReadFileACK,          /* 6 */
      show_atomicWriteFileACK,         /* 7 */
      0,0,
      show_createObjectACK,            /* 10 */
      0,
      show_readPropertyACK,            /* 12 */
      show_readPropertyConditionalACK, /* 13 */
      show_readPropertyMultipleACK,    /* 14 */
      0,0,0,
      show_conf_PrivateTransferACK,    /* 18 */
      0, 0,
      show_vtOpenACK,                  /* 21 */
      0,
      show_vtDataACK,                  /* 23 */
      show_authenticateACK,            /* 24 */
      0,
      show_ReadRangeACK,               /* 26 */
     0,0,
     show_getEventInformationACK    /* 29 Added by Zhu Zhenhua, 2004-5-25*/
   };
   sprintf(outstr,"%"FW"s = X'%%02X'","First Header Octet");
   bac_show_flag(outstr,0xFF);
   pif_show_flagmask(0xF0,0x30,"BACnet-ComplexACK-PDU");
   pif_show_flagbit(0x08,"Segmented Message","Unsegmented Message");
   pif_show_flagbit(0x04,"More Follows","No More Follows");
   pif_show_flagbit(0x03,"Unused",NULL);
   
   pif_show_byte("Invoke ID                   = %d");
   if (x & 0x08) /* SEG = 1 */ {
     pif_show_byte   ("Sequence Number             = %d");
     bac_show_byte("Proposed Window Size","%d");

    int service_type = pif_get_byte(0);
    sprintf(get_int_line(pi_data_current,pif_offset,1), "Service Choice %s",
           BAC_STRTAB_BACnetConfirmedServiceChoice.EnumString(service_type, "ConfRq"));
    pif_offset++;

     bac_show_nbytes( pif_end_offset - pif_offset, "[segmented data]" );
   }
   else {
      /* call the confirmed service ACK interpreter function */
      if ((pif_get_byte(0) >= max_confirmed_services) ||
            (show_confirmed_service_ACK[pif_get_byte(0)] == NULL)) 
     {
         bac_show_byte("Error: Unsupported Complex ACK Type","%u");
     }
      else {
         (*show_confirmed_service_ACK[pif_get_byte(0)])();
     }
   }
}

/*************************************************************************/
void show_segment_ack( unsigned char )
/*************************************************************************/
{
   sprintf(outstr,"%"FW"s = X'%%02X'","First Header Octet");
   bac_show_flag(outstr,0xFF);
   pif_show_flagmask(0xF0,0x40,"BACnet-SegmentACK-PDU");
   pif_show_flagbit(0x0e,"Unused",NULL);
   pif_show_flagbit(0x01,"Ack From Server","Ack From Client");
   
   bac_show_byte("Original Invoke ID","%d");
   bac_show_byte("Sequence Number","%d");
   bac_show_byte("Actual Window Size","%d");
}

/**************************************************************************/
void show_error( unsigned char x )
/**************************************************************************/
{
   void (*show_confirmed_service_error[max_confirmed_services])() = {
      show_error_codes, // 0
      show_error_codes,
      show_error_codes,
      show_error_codes,
      show_error_codes,
      show_error_codes, // 5
      show_error_codes,
      show_error_codes,
      show_createObjectError,  /* 8 change list error - same as CreateObjectError*/
      show_createObjectError,  /* 9 change list error - same as CreateObjectError*/
      show_createObjectError, // 10 CreateObjectError
      show_error_codes,
      show_error_codes,
      show_error_codes,
      show_error_codes,
      show_error_codes, // 15
      show_writePropertyMultipleError, // 16
      show_error_codes,
      show_error_codes,
      show_error_codes,
      show_error_codes,  // 20
      show_error_codes,
      show_vtCloseError, // 22
      show_error_codes,
      show_error_codes,
      show_error_codes,  //25
      show_error_codes,  //26 ReadRange
      show_error_codes,  //27 life safety
      show_error_codes,  //28 subscribe COV
      show_error_codes   //29 get event info
   };
   sprintf(outstr,"%"FW"s = X'%%02X'","First Header Octet");
   bac_show_flag(outstr,0xFF);
   pif_show_flagmask(0xF0,0x50,"BACnet-Error-PDU");
   pif_show_flagbit(0x0F,"Unused",NULL);
   
   bac_show_byte("Invoke ID","%d");
   pif_show_space();
   int service = pif_get_byte(0);
   if (service < max_confirmed_services)
   {
        sprintf(outstr,"%s Service Error",
           BAC_STRTAB_BACnetConfirmedServiceChoice.EnumString(pif_get_byte(0), "ConfRq"));
      bac_show_byte(outstr,"%u");
     if (show_confirmed_service_error[service] != NULL)
     {
        (*show_confirmed_service_error[service])();
     }
     else
     {
         show_error_codes();
     }
   }
   else {
      if (pif_get_byte(0) == 0) {
         bac_show_byte("Other Error","%u");
         show_error_codes();
      }
      else
     {
         bac_show_byte("Error: Unsupported Error Designation","%u");
     }
   }
}

/**************************************************************************/
void show_reject( unsigned char )
/**************************************************************************/
{
   sprintf(outstr,"%"FW"s = X'%%02X'","First Header Octet");
   bac_show_flag(outstr,0xFF);
   pif_show_flagmask(0xF0,0x60,"BACnet-Reject-PDU");
   pif_show_flagbit(0x0F,"Unused",NULL);
   
   bac_show_byte("Original Invoke ID","%d");
   sprintf(outstr,"%"FW"s = %s = (%%u)","Reject Reason",
           BAC_STRTAB_BACnetReject.EnumString(pif_get_byte(0), "Reject"));
   pif_show_byte(outstr);
}

/**************************************************************************/
void show_abort( unsigned char )
/**************************************************************************/
{
   sprintf(outstr,"%"FW"s = X'%%02X'","First Header Octet");
   bac_show_flag(outstr,0xFF);
   pif_show_flagmask(0xF0,0x70,"BACnet-Abort-PDU");
   pif_show_flagbit(0x0E,"Unused",NULL);
   pif_show_flagbit(0x01,"Abort From Server","Abort From Client");
   
   bac_show_byte("Original Invoke ID","%d");
   sprintf(outstr,"%"FW"s = %s = (%%u)","Abort Reason",
           BAC_STRTAB_BACnetAbort.EnumString(pif_get_byte(0), "Abort"));
   pif_show_byte(outstr);
}

/**************************************************************************/
// Show a timestamp
// If theTag is >= 0, show wrapped in context tag
// If theTag is < 0, and title provided, show wrapped in title
// Else do not wrap
void show_time_stamp( BACnetSequence &theSeq, int theTag, const char *pTheTitle )
{
   bool named = (pTheTitle != NULL) && (*pTheTitle != 0);
   
   if (theTag >= 0)
   {
      theSeq.OpeningTag( theTag, pTheTitle );
   }
   else if (named)
   {
      theSeq.BeginWrap( pTheTitle );
   }

   theSeq.BeginChoice();
   theSeq.Time(           0, "time", BSQ_CHOICE );
   theSeq.Unsigned(       1, "sequenceNumber", BSQ_CHOICE );
   if (theSeq.OpeningTag( 2, "dateTime", BSQ_CHOICE ))
   {
      theSeq.Date( -1, "date", BSQ_CHOICE );
      theSeq.Time( -1, "time", BSQ_CHOICE );
      theSeq.ClosingTag();
   }
   theSeq.EndChoice();

   if (theTag >= 0)
   {
      theSeq.ClosingTag();
   }
   else if (named)
   {
      theSeq.EndWrap();
   }
}

/*************************************************************************/
// Show an Error
void show_error( BACnetSequence &theSeq )
{
   theSeq.Enumerated( -1, "error-class", &BAC_STRTAB_BACnetErrorClass );
   theSeq.Enumerated( -1, "error-code",  &BAC_STRTAB_BACnetErrorCode );
}

/*************************************************************************/
// Show an Error, wrapped in context tag
// Default tag is [0].  Default string is "errorType"
void show_wrapped_error( BACnetSequence &theSeq, int theTag, const char *pTheString )
{
   if (pTheString == NULL)
   {
      pTheString = "errorType";
   }
   
   if (theSeq.OpeningTag( theTag, pTheString ))
   {
      show_error( theSeq );
      theSeq.ClosingTag();
   }
}

/*************************************************************************/
// Show a BACnetPropertyReference
void show_bac_property_reference( BACnetSequence &theSeq )
{
   theSeq.PropertyIdentifier( 0, "propertyIdentifier" );
   theSeq.PropertyArrayIndex( 1, "propertyArrayIndex", BSQ_OPTIONAL );
}

/*************************************************************************/
// Show a BACnetObjectPropertyReference
void show_bac_object_property_reference( BACnetSequence &theSeq )
{
   theSeq.ObjectIdentifier(   0, "objectIdentifier" );
   theSeq.PropertyIdentifier( 1, "propertyIdentifier" );
   theSeq.PropertyArrayIndex( 2, "propertyArrayIndex", BSQ_OPTIONAL );
}

/*************************************************************************/
// Show a BACnetPropertyValue
void show_bac_property_value( BACnetSequence &theSeq, int theObjectType )
{
   theSeq.PropertyIdentifier( 0, "propertyIdentifier" );
   theSeq.PropertyArrayIndex( 1, "propertyArrayIndex", BSQ_OPTIONAL );
   if (theSeq.OpeningTag(     2, "value" ))
   {
      show_bac_ANY( theSeq, theObjectType, theSeq.LastPropertyID(), theSeq.LastPropertyIndex() );
      theSeq.ClosingTag();
   }

   theSeq.Unsigned(           3, "priority", BSQ_OPTIONAL );
}

/*************************************************************************/
// Show a BACnetObjectPropertyValue
void show_bac_object_property_value( BACnetSequence &theSeq, int theObjectType )
{
   theSeq.ObjectIdentifier(   0, "objectIdentifier" );
   theSeq.PropertyIdentifier( 1, "propertyIdentifier" );
   theSeq.PropertyArrayIndex( 2, "propertyArrayIndex", BSQ_OPTIONAL );
   if (theSeq.OpeningTag(     3, "value" ))
   {
      show_bac_ANY( theSeq, theSeq.LastObjectType(), theSeq.LastPropertyID(), theSeq.LastPropertyIndex() );
      theSeq.ClosingTag();
   }

   theSeq.Unsigned(           4, "priority", BSQ_OPTIONAL );
}

/*************************************************************************/
// Show a WriteAccessSpecification
void show_write_access_specification( BACnetSequence &theSeq )
{
   theSeq.ObjectIdentifier( 0, "objectIdentifier" );
   int objectType = theSeq.LastObjectType();
   theSeq.ListOf(           1, "listOfPropertyReferences" );
   while (theSeq.HasListElement())
   {
      show_bac_property_value( theSeq, objectType );
   }
}

/*************************************************************************/
// Show a ReadAccessSpecification
void show_read_access_spec( BACnetSequence &theSeq )
{
   theSeq.ObjectIdentifier( 0, "objectIdentifier" );
   theSeq.ListOf(           1, "listOfPropertyReferences" );
   while (theSeq.HasListElement())
   {
      show_bac_property_reference( theSeq );
   }
}

/*************************************************************************/
// Show a ReadAccessResult
void show_read_access_result( BACnetSequence &theSeq )
{
   theSeq.ObjectIdentifier( 0, "objectIdentifier" );

   // Save the object, since ANY parsing might overwrite the seq members
   unsigned int objectID = theSeq.LastObjectIdentifier();
   int objectType = theSeq.LastObjectType();
   
   theSeq.ListOf(           1, "listOfResults" );
   while (theSeq.HasListElement())
   {
      theSeq.PropertyIdentifier( 2, "propertyIdentifier" );
      theSeq.PropertyArrayIndex( 3, "propertyArrayIndex", BSQ_OPTIONAL );
      theSeq.BeginChoice();
      if (theSeq.OpeningTag( 4, "propertyValue", BSQ_CHOICE ))
      {
         // TODO: is this really the best way to do this?
         // For one thing, we don't really even know which DEVICE this
         // response is from.
         // Defined in EPICSTreeView.cpp.  Active only if the EPICS tree is open.
         // 
         // madanner 9/04 add calls to EPICS read property tracker
         ::EPICS_AddRPValue( objectID, theSeq.LastPropertyID(), theSeq.LastPropertyIndex(),
                        pif_get_addr(), pif_end_offset - pif_offset);

         show_bac_ANY( theSeq, objectType, theSeq.LastPropertyID(), theSeq.LastPropertyIndex() );
         theSeq.ClosingTag();
      }
      if (theSeq.OpeningTag( 5, "propertyAccessError", BSQ_CHOICE ))
      {
         theSeq.Enumerated( -1, "error-class", &BAC_STRTAB_BACnetErrorClass );
         theSeq.Enumerated( -1, "error-code",  &BAC_STRTAB_BACnetErrorCode );
         theSeq.ClosingTag();
      }
      theSeq.EndChoice();
   }
}

/**************************************************************************/
// Show a BACnetDeviceObjectPropertyReference
void show_bac_dev_obj_prop_ref( BACnetSequence &theSeq )
{
   theSeq.ObjectIdentifier(   0, "objectIdentifier" );
   theSeq.PropertyIdentifier( 1, "propertyIdentifier" );
   theSeq.PropertyArrayIndex( 2, "arrayIndex", BSQ_OPTIONAL );
   theSeq.ObjectIdentifier(   3, "deviceIdentifier", BSQ_OPTIONAL );
}

/**************************************************************************/
// Show a BACnetDeviceObjectPropertyValue
void show_bac_dev_obj_prop_val( BACnetSequence &theSeq )
{
   theSeq.ObjectIdentifier(   0, "deviceIdentifier" );
   theSeq.ObjectIdentifier(   1, "objectIdentifier" );
   theSeq.PropertyIdentifier( 2, "propertyIdentifier" );
   theSeq.PropertyArrayIndex( 3, "arrayIndex", BSQ_OPTIONAL );
   if (theSeq.OpeningTag(     4, "value" ))
   {
      show_bac_ANY( theSeq, theSeq.LastObjectType(), theSeq.LastPropertyID(), theSeq.LastPropertyIndex() );
      theSeq.ClosingTag();
   }
}

/**************************************************************************/
// Show a BACnetAddress
void show_bacnet_address( BACnetSequence &theSeq, const char *pTheTitle )
{
   if ((pTheTitle == NULL) || (*pTheTitle == 0))
   {
      pTheTitle = "BACnetAddress";
   }
   
   theSeq.BeginWrap( pTheTitle );
   theSeq.Unsigned(    -1, "network-number" );
   theSeq.OctetString( -1, "mac-address" );
   theSeq.EndWrap();
}

static const char* weeks[] =
{
   "invalid",  // 0
   "1st",
   "2nd",
   "3rd",
   "4th",
   "5th",
   "last"      // 6
};

/**************************************************************************/
// Show a BACnetCalendarEntry
void show_calendar_entry( BACnetSequence &theSeq )
{
   theSeq.BeginChoice();
   theSeq.Date(             0, "date", BSQ_CHOICE );
   if (theSeq.OpeningTag(   1, "dateRange", BSQ_CHOICE ))
   {
      theSeq.Date( -1, "startDate" );
      theSeq.Date( -1, "endDate" );
      theSeq.ClosingTag();
   }
   if (theSeq.VetPrimitive( 2, -1, BSQ_CHOICE ))
   {
      // Encoding is as an octet-string.  But we want to show 3 bytes:
      // - month 1-12, odd, even, any
      // - week of month (1-7), (8-14), (15-21), (22-28), (29-31), (last 7), or any
      // - day 1 to 7 or any
      theSeq.Parse();
      BacParser &par = theSeq.Parser();
      unsigned int len = par.DataLength();
      if (len != 3)
      {
         theSeq.Fail( "Expected BACnetWeekNDay.  Got length %u", len );
      }
      else
      {
         char *pStr = outstr;
         unsigned int offset = par.Offset();
         unsigned int x = pif_get_byte( offset );
         if (x == 255)
            pStr += sprintf(pStr,"any month, ");
         else if (x < 15)
            pStr += sprintf(pStr,"%s, ", BAC_STRTAB_month.EnumString(x));
         else
            theSeq.Fail( "Expected BACnetWeekNDay: got month %u", x );

         x = pif_get_byte( offset+1 );
         if (x == 255)
            pStr += sprintf(pStr,"any week, ");
         else if (x <= 6)
            pStr += sprintf(pStr,"%s week, ", weeks[x]);
         else
            theSeq.Fail( "Expected BACnetWeekNDay: got week %u", x );

         x = pif_get_byte( offset+2 );
         if (x == 255)
            sprintf(pStr,"any day");
         else if (x < 8)
            sprintf(pStr,BAC_STRTAB_day_of_week.EnumString(x));
         else
            theSeq.Fail( "Expected BACnetWeekNDay: got day %u", x );

         if (theSeq.IsOK())
         {
            sprintf(get_int_line(pi_data_current,pif_offset, offset + 3, NT_ITEM_HEAD), 
                   "[2] weekNDay: %s", outstr);

            // Show the octet version
            unsigned int len = show_tag();
            show_bac_octetstring(len);
            theSeq.Synch();
         }
      }
   }

   theSeq.EndChoice();
}

/**************************************************************************/
// Show a commandable value.  Used for Priority_Array element, and Relinquish_Default
void show_bac_commandable_value( BACnetSequence &seq, int theObjectType )
{
   switch (theObjectType) {
    case 1:  /* Analog_Out */
    case 2:  /* Analog_Value */
      seq.Real( -1, "" );
      break;
   case 4:  /* Binary_Output */
    case 5:  /* Binary_Value */
      seq.Enumerated( -1, "BACnetBinaryPV", &BAC_STRTAB_BACnetBinaryPV );
      break;
   case 14: /* Multistate_Output */
   case 19: /* Multistate_Value */
      seq.Unsigned( -1, "" );
      break;
   case 30: /* Access Door - Enumerated */
      seq.Enumerated( -1, "BACnetDoorValue", &BAC_STRTAB_BACnetDoorValue );
      break;
   case -1:
      // We don't have an object type to determine WHAT this is.
      seq.AnyTaggedItem(true);
      break;
   default:
      if (seq.OpeningTag( 0, "constructedValue" ))
      {
         seq.AnyTaggedItem();
         seq.ClosingTag();
      }
      break;
   }
}

/**************************************************************************/
// Show a BACnetPriorityValue
void show_bac_priority_value( BACnetSequence &theSeq, int theObjectType )
{
   theSeq.Parse();
   if ((theSeq.Parser().TagValue() == 0) && (theSeq.Parser().FixedTag()))
   {
      theSeq.Null( -1, "null" );
   }
   else
   {
      show_bac_commandable_value( theSeq, theObjectType );
   }
}

/**************************************************************************/
void show_acknowledgeAlarm( void )
/**************************************************************************/
  /* This function interprets AcknowledgeAlarm service requests */
{
   bac_show_byte("Acknowledge Alarm Request","%u");
   BACnetSequence seq;
   seq.Unsigned(         0, "acknowledgingProcessIdentifier" );
   seq.ObjectIdentifier( 1, "eventObjectIdentifier" );
   seq.Enumerated(       2, "eventStateAcknowledged", &BAC_STRTAB_BACnetEventState );
   show_time_stamp( seq, 3, "timeStamp" );
   seq.TextString(       4, "acknowledgementSource" );
   show_time_stamp( seq, 5, "timeOfAcknowledgement" );
}

/**************************************************************************/
void show_COVNotification( void )
{
   BACnetSequence seq;
   seq.Unsigned(         0, "subscriberProcessIdentifier" );
   seq.ObjectIdentifier( 1, "initiatingDeviceIdentifier" );
   seq.ObjectIdentifier( 2, "monitoredObjectIdentifier" );
   int objectType = seq.LastObjectType();
   seq.Unsigned(         3, "timeRemaining (seconds)" );
   seq.ListOf(           4, "listOfValues" ); 
   while (seq.HasListElement())
   {
      show_bac_property_value( seq, objectType );
   }
}

/**************************************************************************/
void show_confirmedCOVNotification( void )
/**************************************************************************/
  /* This function interprets ConfirmedCOVNotification requests */
{
   bac_show_byte("Confirmed COV Notification Request","%u");
   show_COVNotification();
}

/**************************************************************************/
void show_EventNotification( BACnetSequence &seq )
{
   seq.Unsigned(         0, "processIdentifier" );
   seq.ObjectIdentifier( 1, "initiatingDeviceIdentifier" );
   int objectType = seq.LastObjectType();
   seq.ObjectIdentifier( 2, "eventObjectIdentifier" );
   show_time_stamp( seq,   3, "timeStamp" );

   seq.Unsigned(   4, "notificationClass" );
   seq.Unsigned(   5, "priority" );
   seq.Enumerated( 6, "eventType", &BAC_STRTAB_BACnetEventType );
   seq.TextString( 7, "messageText", BSQ_OPTIONAL );

   seq.Enumerated( 8, "notifyType", &BAC_STRTAB_BACnetNotifyType );
   unsigned int notify_type = seq.LastEnumeration();

   bool gotOpt = seq.Boolean( 9, "ackRequired", BSQ_OPTIONAL );
   gotOpt &= seq.Enumerated( 10, "fromState", &BAC_STRTAB_BACnetEventState, BSQ_OPTIONAL );
    if (!gotOpt && (notify_type != 2))
   {
       seq.Fail("Missing required notification parameters");
    }

   seq.Enumerated( 11, "toState", &BAC_STRTAB_BACnetEventState );

   if (seq.OpeningTag( 12, "eventValues", BSQ_OPTIONAL ))
   {
      seq.BeginChoice();
      if (seq.OpeningTag( 0, "Change of Bitstring parameters", BSQ_CHOICE ))
      {
         seq.BitString( 0, "referencedBitstring" );
         seq.BitString( 1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 1, "Change of State parameters", BSQ_CHOICE ))
      {
         if (seq.OpeningTag( 0, "newState" ))
         {
            show_bac_property_states(seq);
            seq.ClosingTag();
         }

         seq.BitString( 1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 2, "Change of Value parameters", BSQ_CHOICE ))
      {
         if (seq.OpeningTag( 0, "newValue" ))
         {
            seq.BeginChoice();
            seq.BitString( 0, "changedBits", NULL, BSQ_CHOICE );
            seq.Real(      1, "changedValue", BSQ_CHOICE );
            seq.EndChoice();
            seq.ClosingTag();
         }

         seq.BitString( 1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 3, "Command Fail parameters", BSQ_CHOICE ))
      {
         seq.ListOf(    0, "commandValue" );
         while (seq.HasListElement())
         {
            seq.AnyTaggedItem();
         }

         seq.BitString( 1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.ListOf(    2, "feedbackValue" );
         while (seq.HasListElement())
         {
            seq.AnyTaggedItem();
         }

         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 4, "Floating Limit parameters", BSQ_CHOICE ))
      {
         seq.Real( 0, "referencedValue" );
         seq.BitString( 1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.Real( 2, "setpointValue" );
         seq.Real( 3, "errorLimit" );

         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 5, "Out of Range parameters", BSQ_CHOICE ))
      {
         seq.Real( 0, "exceedingValue" );
         seq.BitString( 1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.Real( 2, "deadband" );
         seq.Real( 3, "exceededLimit" );

         seq.ClosingTag();
      }
      else if (seq.ListOf( 6, "Complex Event parameters", BSQ_CHOICE ))
      {
         while (seq.HasListElement())
         {
            show_bac_property_value( seq, objectType );
         }
      }
      else if (seq.OpeningTag( 8, "Change of Life Safety parameters", BSQ_CHOICE ))
      {
         seq.Enumerated( 0, "newState", &BAC_STRTAB_BACnetLifeSafetyState );
         seq.Enumerated( 1, "newMode", &BAC_STRTAB_BACnetLifeSafetyMode );

         seq.BitString(  2, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.Enumerated( 3, "operationExpected", &BAC_STRTAB_BACnetLifeSafetyOperation );
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 9, "Extended Notification parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(   0, "vendor-id" );
         seq.Unsigned(   1, "extended-event-type" );
         seq.ListOf(     2, "parameters" );
         while (seq.HasListElement())
         {
            seq.BeginChoice();
            seq.Null( -1, "null", BSQ_CHOICE );
            seq.Real( -1, "real", BSQ_CHOICE );
            seq.Unsigned( -1, "unsigned", BSQ_CHOICE );
            seq.Boolean( -1, "boolean", BSQ_CHOICE );
            seq.Integer( -1, "integer", BSQ_CHOICE );
            seq.Double( -1, "double", BSQ_CHOICE );
            seq.OctetString( -1, "octet", BSQ_CHOICE );
            seq.TextString( -1, "octet", BSQ_CHOICE );
            seq.BitString( -1, "bitstring", NULL, BSQ_CHOICE );
            seq.Enumerated( -1, "enumerated", NULL, BSQ_CHOICE );
            seq.Date( -1, "date", BSQ_CHOICE );
            seq.Time( -1, "time", BSQ_CHOICE );
            seq.ObjectIdentifier( -1, "objectIdentifier", BSQ_CHOICE );
            if (seq.OpeningTag( 0, "propertyValue", BSQ_CHOICE ))
            {
               show_bac_dev_obj_prop_val( seq );
               seq.ClosingTag();
            }
            seq.EndChoice();
         }

         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 10, "Buffer-Ready Notification parameters", BSQ_CHOICE ))
      {
         if (seq.OpeningTag( 0, "buffer-property", BSQ_CHOICE ))
         {
            show_bac_dev_obj_prop_ref( seq );
            seq.ClosingTag();
         }
         seq.Unsigned( 1, "previous-notification" );
         seq.Unsigned( 2, "current-notification" );
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 11, "Unsigned-range Notification parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(  0, "exceeding-value" );
         seq.BitString( 1, "status-flags", &BAC_STRTAB_BACnetStatusFlags );
         seq.Unsigned(  2, "exceeded-limit" );
         seq.ClosingTag();
      }
      // Tag 12 is not used
      else if (seq.OpeningTag( 13, "Access Event parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(   0, "accessEvent" ); // TODO: should be Enumerated AccessEvent
         seq.BitString(  1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.Unsigned(   2, "accessEventTag" );
         show_time_stamp( seq, 3, "accessEventTime" );
         seq.OpeningTag( 4, "accessCredential" );
         show_bac_dev_obj_ref( seq );
         seq.ClosingTag();

         // Should be BACnetAuthenticationFactor
         if (seq.ListOf(     5, "authenticationFactor", BSQ_OPTIONAL ))
         {
            while (seq.HasListElement())
            {
               seq.AnyTaggedItem();
            }
         }

         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 14, "Double Out of Range parameters", BSQ_CHOICE ))
      {
         seq.Double(    0, "exceedingValue" );
         seq.BitString( 1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.Double(    2, "deadband" );
         seq.Double(    3, "exceededLimit" );

         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 15, "Signed Out of Range parameters", BSQ_CHOICE ))
      {
         seq.Integer(   0, "exceedingValue" );
         seq.BitString( 1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.Integer(   2, "deadband" );
         seq.Integer(   3, "exceededLimit" );

         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 16, "Unsigned Out of Range parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(  0, "exceedingValue" );
         seq.BitString( 1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.Unsigned(  2, "deadband" );
         seq.Unsigned(  3, "exceededLimit" );

         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 17, "Change of Character String parameters", BSQ_CHOICE ))
      {
         seq.TextString( 0, "changedValue" );
         seq.BitString(  1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );
         seq.TextString( 2, "alarmValue" );

         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 18, "Change of Status Flags parameters", BSQ_CHOICE ))
      {
         // Optional ABSTRACT-SYNTAX.&Type
         if (seq.ListOf(    0, "presentValue", BSQ_OPTIONAL ))
         {
            while (seq.HasListElement())
            {
               seq.AnyTaggedItem();
            }
         }

         seq.BitString( 1, "statusFlags", &BAC_STRTAB_BACnetStatusFlags );

         seq.ClosingTag();
      }

      else if (seq.OpeningTag( 19, "Change of Reliability parameters", BSQ_CHOICE ))
      {
         seq.Enumerated( 0, "reliability", &BAC_STRTAB_BACnetReliability );
         seq.BitString(  1, "status-flags", &BAC_STRTAB_BACnetStatusFlags );
         if (seq.ListOf( 2, "property-values" ))
         {
            while (seq.HasListElement())
            {
               show_bac_property_value( seq, objectType );
            }
         }
         seq.ClosingTag();
      }

      seq.EndChoice();
      seq.ClosingTag();
   }
}

/**************************************************************************/
void show_confirmedEventNotification( void )
/**************************************************************************/
/* This function interprets ConfirmedEventNotification service requests */
{
   bac_show_byte("Confirmed Event Notification","%u");
   BACnetSequence seq;
   show_EventNotification( seq );
}

/**************************************************************************/
void show_getAlarmSummary( void )
/**************************************************************************/
  /* This function interprets GetAlarmSummary service requests */
{
   bac_show_byte("Get Alarm Summary Request","%u");
}

//Added by Zhu Zhenhua, 2004-5-25
/**************************************************************************/
void show_getEventInformation( void )
/**************************************************************************/
  /* This function interprets GetEventInformation service requests */
{
   bac_show_byte("Get Event Information Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier( 0, "Last Received Object Identifier", BSQ_OPTIONAL );
}

//Added by Zhu Zhenhua, 2004-5-25
/**************************************************************************/
void show_getEventInformationACK( void )
/**************************************************************************/
/* This function interprets GetEventInformationACK service requests */
{
   bac_show_byte("Get Event Information Ack","%u");
   BACnetSequence seq;
   seq.ListOf( 0, "List of Event Summary" );
   while (seq.HasListElement())
   {
      seq.ObjectIdentifier( 0, "Object Identifier" );
      seq.Enumerated(       1, "Event State", &BAC_STRTAB_BACnetEventState );
      seq.BitString(        2, "Acknowledged Transitions", &BAC_STRTAB_BACnetEventTransitionBits );
      if (seq.OpeningTag(   3, "Event Time Stamps" ))
      {
         show_time_stamp( seq, -1, "to-off-normal time" );
         show_time_stamp( seq, -1, "to-fault time" );
         show_time_stamp( seq, -1, "to-normal time" );
         seq.ClosingTag();
      }
      seq.Enumerated( 4, "Notify Type", &BAC_STRTAB_BACnetNotifyType );
      seq.BitString(  5, "Event Enable", &BAC_STRTAB_BACnetEventTransitionBits );
      if (seq.OpeningTag( 6, "Event Priorities" ))
      {
         seq.Unsigned( -1, "to-off-normal" );
         seq.Unsigned( -1, "to-fault" );
         seq.Unsigned( -1, "to-normal" );
         seq.ClosingTag();
      }
   }
   seq.Boolean( 1, "more events" );
}

/**************************************************************************/
void show_getEnrollmentSummary( void )
/**************************************************************************/
  /* This function interprets GetEnrollment service requests */
{
   bac_show_byte("Get Enrollment Summary Request","%u");
   BACnetSequence seq;
   seq.Enumerated( 0, "acknowledgementFilter", &BAC_STRTAB_Acknowledgement_Filter );
   if (seq.OpeningTag( 1, "enrollmentFilter", BSQ_OPTIONAL ))
   {
      show_bac_recipient_process( seq );
      seq.ClosingTag();
   }
   seq.Enumerated( 2, "eventStateFilter Filter", &BAC_STRTAB_EventState_Filter, BSQ_OPTIONAL );
   seq.Enumerated( 3, "eventTypeFilter", &BAC_STRTAB_BACnetEventType, BSQ_OPTIONAL );
   if (seq.OpeningTag( 4, "priorityFilter", BSQ_OPTIONAL ))
   {
      seq.Unsigned( 0, "minPriority" );
      seq.Unsigned( 1, "maxPriority" );
      seq.ClosingTag();
   }
   seq.Unsigned(   5, "notificationClassFilter", BSQ_OPTIONAL );
}

/**************************************************************************/
void show_subscribeCOV( void )
/**************************************************************************/
{
   bac_show_byte("Subscribe COV Request","%u");
   BACnetSequence seq;
   seq.Unsigned(             0, "subscriberProcessIdentifier" );
   seq.ObjectIdentifier(     1, "monitoredObjectIdentifier" );
   bool conf = seq.Boolean(  2, "issueConfirmedNotifications", BSQ_OPTIONAL );
   bool life = seq.Unsigned( 3, "lifetime", BSQ_OPTIONAL );
   if (!life && !conf)
   {
      show_head_ascii("Subscription Cancellation Request");
   }
   else if (life && !conf)
   {
      ShowErrorDetail("Error: missing issueConfirmedNotifications");
   }
}

/**************************************************************************/
void show_subscribeCOV_Property( void )
/**************************************************************************/
{
   bac_show_byte("Subscribe COV Property Request","%u");
   BACnetSequence seq;
   seq.Unsigned(             0, "subscriberProcessIdentifier" );
   seq.ObjectIdentifier(     1, "monitoredObjectIdentifier" );
   bool conf = seq.Boolean(  2, "issueConfirmedNotifications", BSQ_OPTIONAL );
   bool life = seq.Unsigned( 3, "lifetime", BSQ_OPTIONAL );
   if (!life && !conf)
   {
      show_head_ascii("Subscription Cancellation Request");
   }
   else if (life && !conf)
   {
      ShowErrorDetail("Error: missing issueConfirmedNotifications");
   }

   if (seq.OpeningTag( 4, "monitoredPropertyIdentifier" ))
   {
      show_bac_property_reference( seq);
      seq.ClosingTag();
   }

   seq.Real( 5, "covIncrement", BSQ_OPTIONAL );
}

/**************************************************************************/
void show_atomicReadFile( void )
/**************************************************************************/
  /* This function interprets AtomicReadFile service requests */
{
   bac_show_byte("Atomic Read File Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier( -1, "fileIdentifier" );
   seq.BeginChoice();
   if (seq.OpeningTag( 0, "streamAccess", BSQ_CHOICE ))
   {
      seq.Integer(  -1, "fileStartPosition" );
      seq.Unsigned( -1, "requestedOctetCount" );
      seq.ClosingTag();
   }
   if (seq.OpeningTag( 1, "recordAccess", BSQ_CHOICE ))
   {
      seq.Integer(  -1, "fileStartRecord" );
      seq.Unsigned( -1, "requestedRecordCount" );
      seq.ClosingTag();
   }
   seq.EndChoice();
}
    
/**************************************************************************/
void show_atomicWriteFile( void )
/**************************************************************************/
  /* This function interprets AtomicWriteFile service requests */
{
   bac_show_byte("Atomic Write File Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier( -1, "fileIdentifier" );
   seq.BeginChoice();
   if (seq.OpeningTag( 0, "streamAccess", BSQ_CHOICE ))
   {
      seq.Integer(     -1, "fileStartPosition" );
      seq.OctetString( -1, "fileData" );
      seq.ClosingTag();
   }
   if (seq.OpeningTag( 1, "recordAccess", BSQ_CHOICE ))
   {
      seq.Integer(  -1, "fileStartRecord" );
      seq.Unsigned( -1, "recordCount" );
      seq.ListOf(       "fileRecordData" );
      while (seq.HasListElement())
      {
         seq.OctetString( -1, "record" );
      }
      seq.ClosingTag();
   }
   seq.EndChoice();
}

/**************************************************************************/
void show_addListElement( void )
/*************************************************************************/
  /* This function interprets AddListElement service requests */
{
   bac_show_byte("Add List Element Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier(   0, "objectIdentifier" );
   seq.PropertyIdentifier( 1, "propertyIdentifier" );
   seq.PropertyArrayIndex( 2, "propertyArrayIndex", BSQ_OPTIONAL );
   if (seq.OpeningTag(     3, "listOfElements" ))
   {
      show_bac_ANY( seq, seq.LastObjectType(), seq.LastPropertyID(), seq.LastPropertyIndex() );
      seq.ClosingTag();
   }
}

/*************************************************************************/
void show_removeListElement( void )
/*************************************************************************/
  /* This function interprets RemoveListElement service requests */
{
   bac_show_byte("Remove List Element Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier(   0, "objectIdentifier" );
   seq.PropertyIdentifier( 1, "propertyIdentifier" );
   seq.PropertyArrayIndex( 2, "propertyArrayIndex", BSQ_OPTIONAL );
   if (seq.OpeningTag(     3, "listOfElements" ))
   {
      show_bac_ANY( seq, seq.LastObjectType(), seq.LastPropertyID(), seq.LastPropertyIndex() );
      seq.ClosingTag();
   }
}

/*************************************************************************/
void show_createObject( void )
/*************************************************************************/
  /* This function interprets CreateObject service requests */
{
   bac_show_byte("Create Object Request","%u");
   int objectType = -1;
   BACnetSequence seq;
   seq.OpeningTag( 0, "objectSpecifier" );
   seq.BeginChoice();
   if (seq.Enumerated(       0, "objectType", &BAC_STRTAB_BACnetObjectType, BSQ_CHOICE))
   {
      objectType = seq.LastEnumeration();
   }
   if (seq.ObjectIdentifier( 1, "objectIdentifier", BSQ_CHOICE ))
   {
      objectType = seq.LastObjectType();
   }
   seq.EndChoice();
   seq.ClosingTag();

   if (seq.ListOf( 1, "listOfInitialValues", BSQ_OPTIONAL ))
   {
      while (seq.HasListElement())
      {
         show_bac_property_value( seq, objectType );
      }
   }
}

/*************************************************************************/
void show_deleteObject( void )
/*************************************************************************/
  /* This function interprets DeleteObject service requests */
{
   bac_show_byte("Delete Object Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier( -1, "objectIdentifier" );
}

/*************************************************************************/
void show_readProperty( void )
/*************************************************************************/
  /* This function interprets ReadProperty service requests */
{
   bac_show_byte("Read Property Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier(   0, "objectIdentifier" );
   seq.PropertyIdentifier( 1, "propertyIdentifier" );
   seq.PropertyArrayIndex( 2, "propertyArrayIndex", BSQ_OPTIONAL );
}

/*************************************************************************/
void show_readPropertyConditional( void )
/*************************************************************************/
  /* This function interprets ReadPropertyConditional service requests */
{
   bac_show_byte("Read Property Conditional Request","%u");
   BACnetSequence seq;
   if (seq.OpeningTag( 0, "objectSelectionCriteria" ))
   {
      seq.Enumerated( 0, "selectionLogic", &BAC_STRTAB_Selection_Logic );
      if (seq.ListOf( 1, "listOfSelectionCriteria", BSQ_OPTIONAL ))
      {
         while (seq.HasListElement())
         {
            seq.PropertyIdentifier( 0, "propertyIdentifier" );
            seq.PropertyArrayIndex( 1, "propertyArrayIndex", BSQ_OPTIONAL );
            seq.Enumerated(         2, "relationSpecifier", &BAC_STRTAB_Relation_Specifier );
            if (seq.OpeningTag(     3, "comparisonValue" ))
            {
               // TODO: this won't work correctly for those properties
               // which depend on object type
               show_bac_ANY( seq, -1, seq.LastPropertyID(), seq.LastPropertyIndex() );
               seq.ClosingTag();
            }
         }
      }
      seq.ClosingTag();
   }

   if (seq.OpeningTag( 0, "listOfPropertyReferences" , BSQ_OPTIONAL))
   {
      show_bac_property_reference(seq);
      seq.ClosingTag();
   }
}

/*************************************************************************/
void show_readPropertyMultiple( void )
/*************************************************************************/
  /* This function interprets ReadPropertyMultiple service requests */
{
   bac_show_byte("Read Property Multiple Request","%u");
   BACnetSequence seq;
   seq.ListOf( "listOfReadAccessSpecs" );
   while (seq.HasListElement())
   {
      show_read_access_spec( seq );
   }
}

/*************************************************************************/
void show_writeProperty( void )
/*************************************************************************/
  /* This function interprets WriteProperty service requests */
{
   bac_show_byte("Write Property Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier(   0, "Object Identifier", BSQ_REQUIRED );
   seq.PropertyIdentifier( 1, "Property Identifier", BSQ_REQUIRED );
   seq.PropertyArrayIndex( 2, "Property Array Index", BSQ_OPTIONAL );
   seq.OpeningTag(         3, "PropertyValue" );
      show_bac_ANY( seq, seq.LastObjectType(), seq.LastPropertyID(), seq.LastPropertyIndex() );
   seq.ClosingTag();
   seq.Unsigned(           4, "Priority", BSQ_OPTIONAL );
}

/*************************************************************************/
void show_writePropertyMultiple( void )
/*************************************************************************/
  /* This function interprets WritePropertyMultiple service requests */
{
   bac_show_byte("Write Property Multiple Request","%u");
   BACnetSequence seq;
   seq.ListOf( "listOfWriteAccessSpecifications" );
   while (seq.HasListElement())
   {
      show_write_access_specification( seq );
   }
}

/*************************************************************************/
void show_deviceCommunicationControl( void )
/*************************************************************************/
{
   bac_show_byte("Device Communication Control Request","%u");
   BACnetSequence seq;
   seq.Unsigned(   0, "timeDuration", BSQ_OPTIONAL );
   seq.Enumerated( 1, "enable-disable", &BAC_STRTAB_DeviceCommControl_Command );
   seq.TextString( 2, "password", BSQ_OPTIONAL );
}

/*************************************************************************/
// Show a private transfer.
// Used to show Confirmed-Request, Confirmed-ACK, and Unconfirmed
void show_private_transfer( void )
{
   BACnetSequence seq;
   seq.Unsigned(   0, "vendorID" );
   seq.Unsigned(   1, "serviceNumber" );
   if (seq.ListOf( 2, "serviceParameters", BSQ_OPTIONAL ))
   {
      while (seq.HasListElement())
      {
         seq.AnyTaggedItem();
      }
   }
}

/*************************************************************************/
void show_privateTransfer( void )
/*************************************************************************/
  /* This function interprets ConfirmedPrivateTransfer service requests */
{
   bac_show_byte("Private Transfer Request","%u");
   show_private_transfer();
}

/*************************************************************************/
void show_textMessage( void )
{
   BACnetSequence seq;
   seq.ObjectIdentifier( 0, "textMessageSource" );
   if (seq.OpeningTag(   1, "messageClass", BSQ_OPTIONAL ))
   {
      seq.BeginChoice();
      seq.Unsigned(   0, "numeric", BSQ_CHOICE );
      seq.TextString( 1, "character", BSQ_CHOICE );
      seq.EndChoice();
      seq.ClosingTag();
   }

   seq.Enumerated( 2, "messagePriority", &BAC_STRTAB_TextMessage_Priority );
   seq.TextString( 3, "message" );
}

/*************************************************************************/
void show_confirmedTextMessage( void )
/*************************************************************************/
{
   bac_show_byte("Confirmed Text Message Request","%u");
   show_textMessage();
}

/*************************************************************************/
void show_reinitializeDevice( void )
/*************************************************************************/
  /* This function interprets ReinitializeDevice service requests */
{
   bac_show_byte("Reinitialize Device Request","%u");
   BACnetSequence seq;
   seq.Enumerated( 0, "reinitializedStateOfDevice", &BAC_STRTAB_BACnetReinitializedStateOfDevice );
   seq.TextString( 1, "password", BSQ_OPTIONAL );
}

/*************************************************************************/
void show_vtOpen( void )
/*************************************************************************/
  /* This function interprets VT-Open service requests */
{
   bac_show_byte("VT-Open Request","%u");
   BACnetSequence seq;
   seq.Enumerated( -1, "vtCalss", &BAC_STRTAB_BACnetVTClass );
   seq.Unsigned(   -1, "localVTSessionIdentifier" );
}

/*************************************************************************/
void show_vtClose( void )
/*************************************************************************/
  /* This function interprets VT-Close service requests */
{
   bac_show_byte("VT-Close Request","%u");
   BACnetSequence seq;
   seq.ListOf( "listOfRemoteVTSessionIdentifiers" );
   while (seq.HasListElement())
   {
      seq.Unsigned( -1, "sessionID" );
   }
}

/*************************************************************************/
void show_vtData( void )
/*************************************************************************/
  /* This function interprets VT-Data service requests */
{
   bac_show_byte("VT Data Request","%u");
   BACnetSequence seq;
   seq.Unsigned(    -1, "vtSessionIdentifier" );
   seq.OctetString( -1, "vtNewData" );
   seq.Unsigned(    -1, "vtDataFlag" );
}

/**************************************************************************/
void show_authenticate( void )
/**************************************************************************/
{
   bac_show_byte("Authenticate Request","%u");
   BACnetSequence seq;
   seq.Unsigned(   0, "pseudoRandomNumber" );
   seq.Unsigned(   1, "expectedInvokeID", BSQ_OPTIONAL );
   seq.TextString( 2, "operatorName", BSQ_OPTIONAL );
   seq.TextString( 3, "operatorPassword", BSQ_OPTIONAL );
   seq.Boolean(    4, "startEncipheredSession", BSQ_OPTIONAL );
}

/**************************************************************************/
void show_requestKey( void )
/**************************************************************************/
{
   bac_show_byte("Request Key Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier(     -1, "requestingDeviceIdentifier" );
   show_bacnet_address( seq, "requestingDeviceAddress" );
   seq.ObjectIdentifier(     -1, "remoteDeviceIdentifier" );
   show_bacnet_address( seq, "remoteDeviceAddress" );
}

/*************************************************************************/
void show_iAm( void )
/*************************************************************************/
  /* This function interprets I-Am service requests */
{
   bac_show_byte("I-Am Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier( -1, "iAmDeviceIdentifier" );
   seq.Unsigned(         -1, "maxAPDULengthAccepted" );
   seq.Enumerated(       -1, "segmentationSupported", &BAC_STRTAB_BACnetSegmentation );
   seq.Unsigned(         -1, "vendorID" );
}

/*************************************************************************/
void show_iHave( void )
/*************************************************************************/
  /* This function interprets I-Have service requests */
{
   bac_show_byte("I-Have Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier( -1, "deviceIdentifier" );
   seq.ObjectIdentifier( -1, "objectIdentifier" );
   seq.TextString(       -1, "objectName" );
}

/**************************************************************************/
void show_unconfirmedCOVNotification( void )
/**************************************************************************/
  /* This function interprets unconfirmedCOVNotification requests */
{
   bac_show_byte("Unconfirmed COV Notification Request","%u");
   show_COVNotification();
}

/*************************************************************************/
void show_unconfEventNotification( void )
/*************************************************************************/
  /* This function interprets UnconfirmedEventNotification service requests */
{
   bac_show_byte("Unconfirmed Event Notification Request","%u");
   BACnetSequence seq;
   show_EventNotification( seq );
}

/*************************************************************************/
void show_unconfPrivateTransfer( void )
/*************************************************************************/
  /* This function interprets UnconfirmedPrivateTransfer service requests */
{
   bac_show_byte("Unconfirmed Private Transfer Request","%u");
   show_private_transfer();
}

/*************************************************************************/
void show_unconfTextMessage( void )
/*************************************************************************/
{
   bac_show_byte("Unconfirmed Text Message Request","%u");
   show_textMessage();
}

/*************************************************************************/
void show_timeSynchronization( void )
/*************************************************************************/
  /* This function interprets Time Synchronization requests */
{
   bac_show_byte("Time Synchronization Request","%u");
   BACnetSequence seq;
   seq.Date( -1, "date" );
   seq.Time( -1, "time" );
}
/*************************************************************************/
  /* This function interprets UTC Time Synchronization requests */

void show_UTCtimeSynchronization( void )
{
   bac_show_byte("UTC Time Synchronization Request","%u");
   BACnetSequence seq;
   seq.Date( -1, "date" );
   seq.Time( -1, "time" );
}

/*************************************************************************/
void show_whoHas( void )
/*************************************************************************/
  /* This function interprets Who-Has service requests */
{
   bac_show_byte("Who-Has Request","%u");
   BACnetSequence seq;
   bool gotLow  = seq.Unsigned( 0, "deviceInstanceRangeLowLimit", BSQ_OPTIONAL );
   bool gotHigh = seq.Unsigned( 1, "deviceInstanceRangeHighLimit", BSQ_OPTIONAL );
   if (gotLow ^ gotHigh)
   {
      seq.Fail( "Error: must have BOTH low and high range limit or NEITHER" );
   }

   seq.BeginChoice();
   seq.ObjectIdentifier( 2, "objectIdentifier", BSQ_CHOICE );
   seq.TextString(       3, "objectName", BSQ_CHOICE );
   seq.EndChoice();
}

/*************************************************************************/
void show_whoIs( void )
/*************************************************************************/
  /* This function interprets Who-Is service requests */
{
   bac_show_byte("Who-Is Request","%u");
   BACnetSequence seq;
   bool gotLow  = seq.Unsigned( 0, "deviceInstanceRangeLowLimit", BSQ_OPTIONAL );
   bool gotHigh = seq.Unsigned( 1, "deviceInstanceRangeHighLimit", BSQ_OPTIONAL );
   if (gotLow ^ gotHigh)
   {
      seq.Fail( "Error: must have BOTH low and high range limit or NEITHER" );
   }
}

/*************************************************************************/
void show_getAlarmSummaryACK( void )
/*************************************************************************/
  /* This function interprets GetAlarmSummary acknowledgements */
{
   bac_show_byte("Get Alarm Summary Acknowledgement","%u");
   BACnetSequence seq;
   seq.ListOf( "sequence of" );
   while (seq.HasListElement())
   {
      seq.ObjectIdentifier( -1, "objectIdentifier" );
      seq.Enumerated(       -1, "alarmState", &BAC_STRTAB_BACnetEventState );
      seq.BitString(        -1, "acknowledgedTransitions", &BAC_STRTAB_BACnetEventTransitionBits );
   }
}

/*************************************************************************/
void show_getEnrollmentSummaryACK( void )
/*************************************************************************/
  /* This function interprets GetEnrollmentSummary acknowledgements */
{
   bac_show_byte("Get Enrollment Summary Acknowledgement","%u");
   BACnetSequence seq;
   seq.ListOf( "sequence of" );
   while (seq.HasListElement())
   {
      seq.ObjectIdentifier( -1, "objectIdentifier" );
      seq.Enumerated(       -1, "eventType", &BAC_STRTAB_BACnetEventType );
      seq.Enumerated(       -1, "eventState", &BAC_STRTAB_BACnetEventState );
      seq.Unsigned(         -1, "priority" );
      seq.Unsigned(         -1, "notificationClass", BSQ_OPTIONAL );
   }
}

/*************************************************************************/
void show_atomicReadFileACK( void )
/*************************************************************************/
  /* This function interprets AtomicReadFile acknowledgements */
{
   bac_show_byte("Atomic Read File Acknowledgement","%u");
   BACnetSequence seq;
   seq.Boolean( -1, "endOfFile" );
   seq.BeginChoice();
   if (seq.OpeningTag( 0, "streamAccess", BSQ_CHOICE ))
   {
      seq.Integer(     -1, "fileStartPosition" );
      seq.OctetString( -1, "fileData" );
      seq.ClosingTag();
   }
   if (seq.OpeningTag( 1, "recordAccess", BSQ_CHOICE ))
   {
      seq.Integer(  -1, "fileStartRecord" );
      seq.Unsigned( -1, "returnedRecordCount" );
      seq.ListOf(       "fileRecordData" );
      while (seq.HasListElement())
      {
         seq.OctetString( -1, "record" );
      }
      seq.ClosingTag();
   }
   seq.EndChoice();
}

/*************************************************************************/
void show_atomicWriteFileACK( void )
/*************************************************************************/
  /* This function interprets AtomicWriteFile acknowledgements */
{
   bac_show_byte("Atomic Write File Acknowledgement","%u");
      BACnetSequence seq;
   seq.BeginChoice();
   seq.Integer( 0, "fileStartPosition", BSQ_CHOICE );
   seq.Integer( 1, "fileStartRecord", BSQ_CHOICE );
   seq.EndChoice();
}

/*************************************************************************/
void show_createObjectACK( void )
/*************************************************************************/
  /* This function interprets CreateObject acknowledgements */
{
   bac_show_byte("CreateObject Acknowledgement","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier( -1, "objectIdentifier" );
}

/*************************************************************************/
void show_readPropertyACK( void )
/*************************************************************************/
  /* This function interprets ReadProperty acknowledgements */
{
   bac_show_byte("Read Property ACK","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier(   0, "Object Identifier", BSQ_REQUIRED );
   seq.PropertyIdentifier( 1, "Property Identifier", BSQ_REQUIRED );
   seq.PropertyArrayIndex( 2, "Property Array Index", BSQ_OPTIONAL );
   if (seq.OpeningTag(     3, "PropertyValue" ))
   {
      // TODO: is this really the best way to do this?
      // For one thing, we don't really even know which DEVICE this
      // response is from.
      // Defined in EPICSTreeView.cpp.  Active only if the EPICS tree is open.
      // 
      // madanner 9/04 add calls to EPICS read property tracker
      ::EPICS_AddRPValue( seq.LastObjectIdentifier(), seq.LastPropertyID(), seq.LastPropertyIndex(),
                     pif_get_addr(), pif_end_offset - pif_offset);
   
      show_bac_ANY( seq, seq.LastObjectType(), seq.LastPropertyID(), seq.LastPropertyIndex() );
      seq.ClosingTag();
   }
}

/*************************************************************************/
void show_readPropertyConditionalACK( void )
/*************************************************************************/
  /* This function interprets ReadPropertyConditional acknowledgements */
{
   bac_show_byte("Read Property Conditional Acknowledgement","%u");
   BACnetSequence seq;
   seq.ListOf( "listOfReadAccessResults" );
   while (seq.HasListElement())
   {
      show_read_access_result( seq );
   }
}

/*************************************************************************/
void show_readPropertyMultipleACK( void )
/*************************************************************************/
  /* This function interprets ReadPropertyMultiple acknowledgements */
{
   bac_show_byte("Read Property Multiple Acknowledgement","%u");
   BACnetSequence seq;
   seq.ListOf( "listOfReadAccessResults" );
   while (seq.HasListElement())
   {
      show_read_access_result( seq );
   }
}

/*************************************************************************/
void show_conf_PrivateTransferACK( void )
/*************************************************************************/
  /* This function interprets ConfirmedPrivateTransfer acknowledgements */
{
   bac_show_byte("Private Transfer Acknowledgement","%u");
   show_private_transfer();
}

/*************************************************************************/
void show_vtOpenACK( void )
/*************************************************************************/
  /* This function interprets VT-Open acknowledgements */
{
   bac_show_byte("VT Open Acknowledgement","%u");
   BACnetSequence seq;
   seq.Unsigned( -1, "remoteVTSessionIdentifier" );
}

/*************************************************************************/
void show_vtDataACK( void )
/*************************************************************************/
  /* This function interprets VT-Data acknowledgements */
{
   bac_show_byte("VT Data Acknowledgement","%u");
   BACnetSequence seq;
   seq.Boolean( -1, "allNewDataAccepted" );
   bool allAccepted = seq.LastBoolean();
   bool gotCount = seq.Unsigned( -1, "acceptedOctetCount", BSQ_OPTIONAL );

   if (allAccepted ^ gotCount)
   {
      seq.Fail( "acceptedOctetCount must be present if and only if allNewDataAccepted is false" );
   }
}

/*************************************************************************/
void show_authenticateACK( void )
/*************************************************************************/
/* This function interprets Authenticate acknowledgements */
{
   bac_show_byte("Authenticate Acknowledgement","%u");
   BACnetSequence seq;
   seq.Unsigned( -1, "modifiedRandomNumber" );
}

/*************************************************************************/
// This function displays the ReadRange Request
//
void show_ReadRange ( void )
{
   bac_show_byte("Read Range Request","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier(   0, "objectIdentifier" );
   seq.PropertyIdentifier( 1, "propertyIdentifier" );
   seq.PropertyArrayIndex( 2, "propertyArrayIndex", BSQ_OPTIONAL );

   // This entire thing is OPTIONAL
   if (seq.HasMore())
   {
      seq.BeginChoice();
      if (seq.OpeningTag(     3, "byPosition", BSQ_CHOICE ))
      {
         seq.Unsigned( -1, "referenceIndex" );
         seq.Integer(  -1, "count" );
         seq.ClosingTag();
      }
      if (seq.OpeningTag(     6, "bySequenceNumber", BSQ_CHOICE ))
      {
         seq.Unsigned( -1, "referenceIndex" );
         seq.Integer(  -1, "count" );
         seq.ClosingTag();
      }
      if (seq.OpeningTag(     7, "byTime", BSQ_CHOICE ))
      {
         seq.Date( -1, "referenceDate" );
         seq.Time( -1, "referenceTime" );
         seq.Integer(  -1, "count" );
         seq.ClosingTag();
      }
      seq.EndChoice();
   }
}

/*************************************************************************/
// This function displays the data returned by a readRange ACK
//
void show_ReadRangeACK ( void )
{
   bac_show_byte("Read Range Acknowledgement","%u");
   BACnetSequence seq;
   seq.ObjectIdentifier(   0, "objectIdentifier" );
   seq.PropertyIdentifier( 1, "propertyIdentifier" );
   seq.PropertyArrayIndex( 2, "propertyArrayIndex", BSQ_OPTIONAL );
   seq.BitString(          3, "resultFlags", &BAC_STRTAB_BACnetResultFlags );
   seq.Unsigned(           4, "itemCount" );
   seq.ListOf(             5, "itemData" );
   while (seq.HasListElement())
   {
      show_bac_ANY( seq, seq.LastObjectType(), seq.LastPropertyID(), seq.LastPropertyIndex() );
   }

   seq.Unsigned(           6, "firstSequenceNumber", BSQ_OPTIONAL );
}

/*************************************************************************/
// Display CreateObject error.
// Also used for ChangeList-Error (AddListElement and RemoveListElement)
void show_createObjectError( void )
{
   BACnetSequence seq;
   show_wrapped_error( seq, 0 );
   seq.Unsigned( 1, "firstFailedElementNumber" );
}

/*************************************************************************/
// Show an unwrapped Error
// Called directly from Error PDU dispatch table
void show_error_codes( void )
{
   BACnetSequence seq;
   show_error( seq );
}

/*************************************************************************/
void show_writePropertyMultipleError( void )
/*************************************************************************/
/* This function interprets WritePropertyMultiple errors */
{
   BACnetSequence seq;
   show_wrapped_error( seq, 0 );
   if (seq.OpeningTag( 1, "firstFailedWriteAttempt" ))
   {
      show_bac_object_property_reference(seq);
      seq.ClosingTag();
   }
}

/*************************************************************************/
void show_vtCloseError( void )
/*************************************************************************/
/* This function interprets VT-Close errors */
{
   BACnetSequence seq;
   show_wrapped_error( seq, 0 );
   if (seq.ListOf( 1, "listOfVTSessionIdentifiers", BSQ_OPTIONAL ))
   {
      while (seq.HasListElement())
      {
         seq.Unsigned( -1, "uncloseable sessions ID" );
      }
   }
}

/**************************************************************************/
/* The functions that follow are used to display BACnet application data   */
/* types. They may be called following context tags where the IMPLICIT     */
/* keyword is presumed (and therefore no primitive tag is available to     */
/* determine length or data type) or from show_tag().                      */
/**************************************************************************/

/**************************************************************************/
void show_bac_bitstring( unsigned int len )
/**************************************************************************/
/* Displays a bit string with no interpretation of the bit semantics */
{
   unsigned int i;
   
   /* the tag should be displayed before calling this function */
   bac_show_byte("Number of unused bits","%u");
   sprintf(outstr,"%"FW"s = X'%%02X'","Bitstring Values");
   for (i=0; i<len-1; i++) {
      bac_show_flag(outstr,0xFF);
      pif_show_flagbit(0x80,"","");
      pif_show_flagbit(0x40,"","");
      pif_show_flagbit(0x20,"","");
      pif_show_flagbit(0x10,"","");
      pif_show_flagbit(0x08,"","");
      pif_show_flagbit(0x04,"","");
      pif_show_flagbit(0x02,"","");
      pif_show_flagbit(0x01,"","");
   };
}

// Append len PDU bytes beginning at flag+offset to string data to *pOut.
// First byte is character-set specifier
void get_bac_charstring(unsigned int tagLen, char *pOut, unsigned int bufLen, unsigned int flag, unsigned int offset )
{
   char c = pif_get_byte(flag+offset);
   switch(c)
   {
   case 0: /* ASCII */
   case 5: /* ISO 8859-1 */
      // Append as characters
      // TODO: worry about bytes greater than 127 for ANSI/UTF-8, for 8859-1 depending on installed fonts
      *pOut++ = '"';
      {
         // Limit to buffer length
         if (tagLen >= bufLen-3)
         {
            tagLen = bufLen-3;
         }
         pOut = pif_get_ascii(flag+1+offset, tagLen-1, pOut, MAX_TEXT_ITEM);
      }
      *pOut++ = '"';
      *pOut   = 0;
      break;

   case 1: /* MS DBCS */
   case 2: /* JIS C 6226 */
   case 3: /* ISO 10646(UCS-4) */
   case 4: /* ISO 10646(UCS-2) */
      // Append as a lump of hex
      {
         *pOut++ = 'X';
         *pOut++ = '\'';

         static char hex[] = "0123456789ABCDEF";
         int i = flag+1;

         // Limit to buffer length
         if (tagLen >= bufLen-3)
         {
            tagLen = bufLen-3;
         }
         while (--tagLen) {
            int x = pif_get_byte(i+offset);
            *pOut++ = hex[ (x >> 4) & 0x0F ];
            *pOut++ = hex[ x & 0x0F ];
            i++;
         }
         *pOut++ = '\'';
         *pOut   = 0;
      }
      break;

   default:
      sprintf(pOut, "Error: unknown character set %u", c);
      break;
   }
}

/**************************************************************************/
void show_bac_charstring( unsigned int len)
/**************************************************************************/
{
   unsigned char charset;
   
   charset = pif_get_byte(0);
   switch(charset){
      case 0: /* ASCII */
             bac_show_byte("ASCII/UTF-8 Character Encoding","%u");
             sprintf(outstr,"%"FW"s = %%s","Character string");
             pif_show_ascii(len-1, outstr);
             break;
      case 1: /* MS DBCS */
             bac_show_byte("MS Double Byte Character Encoding","%u");
             sprintf(outstr,"%"FW"s = X'%%s'","Character string");
             pif_show_nbytes_hex(outstr, len-1);
             break;
      case 2: /* JIS C 6226 */
             bac_show_byte("JIS X 0208","%u");
             sprintf(outstr,"%"FW"s = X'%%s'","Character string");
             pif_show_nbytes_hex(outstr, len-1);
             break;
      case 3: /* ISO 10646(UCS-4) */
             bac_show_byte("ISO 10646(UCS-4)","%u");
             sprintf(outstr,"%"FW"s = X'%%s'","Character string");
             pif_show_nbytes_hex(outstr, len-1);
             break;
      case 4: /* ISO 10646(UCS-2) */
             bac_show_byte("ISO 10646(UCS-2)","%u");
             sprintf(outstr,"%"FW"s = X'%%s'","Character string");
             pif_show_nbytes_hex(outstr, len-1);
             break;
      case 5: /* ISO 8859-1 */
             bac_show_byte("ISO 8859-1","%u");
             sprintf(outstr,"%"FW"s = X'%%s'","Character string");
             pif_show_nbytes_hex(outstr, len-1);
             break;
      default:  /* invalid character set */
             ShowErrorDetail("Error: unknown character set %u", charset);
      };
}

/**************************************************************************/
void show_bac_octetstring( unsigned int len )
/**************************************************************************/
{
   sprintf(outstr,"%"FW"s = X'%%s'","Octet string");
   pif_show_nbytes_hex(outstr,len);
}

/**************************************************************************/
// Convert four bytes at offset to a date in pOut
// Format is YYYY/MON/DOM,DOW
void date_as_string( char *pOut, int offset )
{
    // Year
   int x = pif_get_byte(offset);
   pOut += sprintf( pOut, (x == 255) ? "XXXX/" : "%u/", 1900+x );
   
   // Month
   x = pif_get_byte(1+offset);
   if (x == 255) {
      pOut += sprintf( pOut, "XXX/" );
   }
   else if ((x>0) && (x<15)) {
      pOut += sprintf( pOut, "%s/", BAC_STRTAB_month.EnumString(x));
   }
   else {
      pOut += sprintf( pOut, "Invalid:%u/", x);
   }
   
   // Day of month
   x = pif_get_byte(2+offset);
   if (x == 255) {
      pOut += sprintf( pOut, "XX, " );
   }
   else if (x == 32) {
      pOut += sprintf( pOut, "last-day, " );
   }
   else if (x == 33) {
      pOut += sprintf( pOut, "odd-days, " );
   }
   else if (x == 34) {
      pOut += sprintf( pOut, "even-days, " );
   }
   else {
      pOut += sprintf( pOut, "%02u, ", x );
   }
   
   // Day of week
   x = pif_get_byte(3+offset);
   if (x == 255) {
      sprintf( pOut, "XXX" );
   }
   else if ((x>0) && (x<8)) {
      sprintf( pOut, BAC_STRTAB_day_of_week.EnumString(x) );
   }
   else {
      sprintf( pOut, "Invalid:%u", x );
   }
}

/**************************************************************************/
// Show a BACnet DATE.
void show_bac_date( unsigned int len )
/**************************************************************************/
{
   if (len != 4)
   {
      ShowErrorDetail("Error: expected date, but length is %u", len);
      return;
   }

   int x = pif_get_byte(0);
   if (x == 255)
      sprintf(outstr,"Unspecified");
   else
      sprintf(outstr,"%u",1900+x);
   show_str_eq_str("Year",outstr,1);
   pif_offset++;

   x = pif_get_byte(0);
   if (x == 255)
      sprintf(outstr,"Unspecified");
   else {
      if ((x>0) && (x<15)) 
         sprintf(outstr,BAC_STRTAB_month.EnumString(x));
      else 
         sprintf(outstr,BAC_STRTAB_month.EnumString(0));
   }
   show_str_eq_str("Month",outstr,1);
   pif_offset++;

   x = pif_get_byte(0);
   if (x == 255)
      sprintf(outstr,"Unspecified");
   else if (x == 32)
      sprintf(outstr,"Last Day");
   else if (x == 33)
      sprintf(outstr,"Odd Days");
   else if (x == 34)
      sprintf(outstr,"Even Days");
   else
      sprintf(outstr,"%u",x);
   show_str_eq_str("Day of Month",outstr,1);
   pif_offset++;

   x = pif_get_byte(0);
   if (x == 255)
      sprintf(outstr,"Unspecified");
   else {
      if ((x>0) && (x<8)) 
         sprintf(outstr,BAC_STRTAB_day_of_week.EnumString(x));
      else 
         sprintf(outstr,BAC_STRTAB_day_of_week.EnumString(0));
   }
   show_str_eq_str("Day of Week",outstr,1);
   pif_offset++;
}

/**************************************************************************/
void show_bac_double( unsigned int len )
/**************************************************************************/
{
   CString dStr;
   double dx;
   unsigned char fstr[8];
   unsigned int i;

   if (len != 8)
   {
      ShowErrorDetail("Error: expected double, but length is %u", len);
     return;
   }

#if LITTLEENDIAN
   for (i=0;i<8;i++) 
      fstr[i] = pif_get_byte(7-i);
#else
   for (i=0;i<8;i++) 
      fstr[i] = pif_get_byte(i);
#endif
   dx = *(double*)fstr;

   sprintf(outstr,"%"FW"s = %%s","Value of double");
   sprintf(get_int_line(pi_data_current,pif_offset,8),outstr,DoubleToString(dStr,dx));
   pif_offset += 8;
}

/**************************************************************************/
void show_bac_real( unsigned int len )
/**************************************************************************/
{
   CString fStr;
   float fx;
   unsigned char fstr[4];
   unsigned int i;

   if (len != 4)
   {
      ShowErrorDetail("Error: expected real, but length is %u", len);
     return;
   }

#if LITTLEENDIAN
   for (i=0;i<4;i++) 
      fstr[i] = pif_get_byte(3-i);
#else
   for (i=0;i<4;i++) 
      fstr[i] = pif_get_byte(i);
#endif   
   fx = *(float *)fstr;

   // We use FloatToString() to get nice treatment of NaN and inf
   sprintf(outstr,"%"FW"s = %%s","Value of float");
   sprintf(get_int_line(pi_data_current,pif_offset,4),outstr,FloatToString(fStr,fx));
   pif_offset += 4;
}

/**************************************************************************/
void show_bac_signed ( unsigned int len )
/**************************************************************************/
{
   unsigned int val = 0;
   if (pif_get_byte(0) & 0x80)
   {
      // Value is negative: fill with ones
      val = (unsigned int)-1;
   }

   for (unsigned int ix = 0; ix < len; ix++)
   {
      val = (val << 8) | pif_get_byte(ix);
   }

   sprintf(outstr,"%"FW"s = %%d", "Value (%u-octet signed)");

   // get a detail line
   sprintf( get_int_line( pif_pi, pif_offset, len ), outstr, len, val );

   // update the offset
   pif_offset += len;
}

/**************************************************************************/
void show_bac_time( unsigned int len )
/**************************************************************************/
{
   unsigned char x;
   char tempstr[80];

   if (len != 4)
   {
      ShowErrorDetail("Error: expected time, but length is %u", len);
     return;
   }

   x = pif_get_byte(0);
   if (x == 255)
      sprintf(outstr,"XX:");
   else
      sprintf(outstr,"%u:",x);

   x = pif_get_byte(1);
   if (x == 255)
      strcat(outstr,"XX:");
   else {
      sprintf(tempstr,"%02u:",x);
      strcat(outstr,tempstr);
   }

   x = pif_get_byte(2);
   if (x == 255)
      strcat(outstr,"XX.");
   else {
      sprintf(tempstr,"%02u.",x);
      strcat(outstr,tempstr);
   }

   x = pif_get_byte(3);
   if (x == 255)
      strcat(outstr,"XX");
   else {
      sprintf(tempstr,"%02u",x);
      strcat(outstr,tempstr);
   }
   
   strcat(outstr,"  (\"XX\" = Unspecified)");
   show_str_eq_str("Time",outstr,4);
   pif_offset += 4;
}

/**************************************************************************/
// Show an unsigned integer and advance the cursor
void show_bac_unsigned( unsigned int len )
/**************************************************************************/
{
   unsigned int value = get_bac_unsigned( 0, len );
   sprintf(outstr, "%"FW"s (%u-octet unsigned) = %%u", "Value", len);
   if (len <= 4)
   {
      sprintf(get_int_line(pi_data_current,pif_offset,len),outstr, value);
   }
   pif_offset += len;
}

/**************************************************************************/
// Show an enumerated value as an integer and advance the cursor
void show_bac_enumerated( unsigned int len )
/**************************************************************************/
{
   unsigned int value = get_bac_unsigned( 0, len );
   sprintf(outstr, "%"FW"s (%u-octet enumeration) = %%u", "Value", len);
   if (len <= 4)
   {
      sprintf(get_int_line(pi_data_current,pif_offset,len),outstr, value);
   }
   pif_offset += len;
}

/**************************************************************************/
unsigned int get_bac_unsigned( int delta, int len )
/**************************************************************************/
/* Returns as an unsigned long any 1,2,3, or 4-octed unsigned integer.
 * Does not advance byte cursor */
{
   switch (len) {
     case 1: return (long)pif_get_byte(delta);
     
     case 2: return (long)pif_get_word_hl(delta);

     case 3: return pif_get_long_hl(delta-1)&0x00FFFFFF;

     case 4: return pif_get_long_hl(delta);
     }

     ShowErrorDetail("Error: expected unsigned, but length is %u", len);
     return 0;
}

/*************************************************************************/
/* These functions are used to interpret context specific and application */
/* tags.                                                                  */
/*************************************************************************/

/**************************************************************************/
// Show tagged value.
// Returns the length of the data, with cursor pointing to next tag or end of PDU.
// 
// Show deconstructed tag, optionally followed by value
unsigned int show_tagged_data( bool showData )
{
   unsigned int lloc,tloc;
   unsigned int tagbuff = pif_get_byte(0);
   bool contextTag = (tagbuff & 0x08) != 0;
   bool pairedTag  = (tagbuff & 0x07) > 5;
   unsigned int type = (tagbuff & 0xF0) >> 4;
   if (type < 15)
      tloc = 0;           /* Type in tag octet */  
   else {
      tloc = 1;           /* Extended type in first octet following */
      type = pif_get_byte(1);
   }
   
   unsigned int len = (tagbuff & 0x07);
   if (len == 5)        /* Extended length? */
      lloc = 1;         /* Length follows tag */
   else
      lloc = 0;           /* Length in tag octet */
   
   if (contextTag)
   {
      // Context tag
      sprintf( outstr,"%"FW"s = X'%%02X'",
             (len == 6) ? "Opening Context Tag" : 
             (len == 7) ? "Closing Context Tag" : 
             "SD Context Tag" );
      bac_show_flag(outstr,0xFF);      // Show tag byte, advance cursor

      sprintf( outstr, 
             (tloc) ? "[extended]" : "[%u]", 
             type );
      pif_show_flagbit( 0xF0, outstr, NULL ); // Show top 4 bits of tag byte (cursor-1)
   }
   else
   {
      // Application tag
      sprintf(outstr,"%"FW"s = X'%%02X'","Application Tag");
      bac_show_flag(outstr,0xFF);      // Show tag byte, advance cursor
      
      pif_show_flagmask(0xF0,0x00,"Null");
      pif_show_flagmask(0xF0,PRIM_BOOLEAN*16,"Boolean");
      pif_show_flagmask(0xF0,PRIM_UNSIGNED*16,"Unsigned Integer");
      pif_show_flagmask(0xF0,PRIM_SIGNED*16,"Signed Integer");
      pif_show_flagmask(0xF0,PRIM_REAL*16,"IEEE Floating Point");
      pif_show_flagmask(0xF0,PRIM_DOUBLE*16,"IEEE Double Floating Point");
      pif_show_flagmask(0xF0,PRIM_OCTET_STRING*16,"Octet String");
      pif_show_flagmask(0xF0,PRIM_CHARACTER_STRING*16,"Character String");
      pif_show_flagmask(0xF0,PRIM_BIT_STRING*16,"Bit String");
      pif_show_flagmask(0xF0,PRIM_ENUMERATED*16,"Enumerated");
      pif_show_flagmask(0xF0,PRIM_DATE*16,"Date");
      pif_show_flagmask(0xF0,PRIM_TIME*16,"Time");
      pif_show_flagmask(0xF0,PRIM_OBJECT_IDENTIFIER*16,"Object Identifier");
      pif_show_flagmask(0xF0,0x0d*16,"Reserved for ASHRAE");
      pif_show_flagmask(0xF0,0x0e*16,"Reserved for ASHRAE");
      pif_show_flagmask(0xF0,0x0f*16,"Non-standard type");
   }
   
   pif_show_flagbit(0x08,"Context Specific Tag","Application Tag");
   
   switch (type + ((contextTag) ? 1000 : 0)) {
   case PRIM_NULL:
      pif_show_flagbit(0x07,"Unused","Unused");
      len = 0;
      break;
   case PRIM_BOOLEAN:
      pif_show_flagbit(0x07,"TRUE","FALSE");
      len = 0;
      break;
   default:
      if (len < 5) {
         bac_show_flagmask(0x07,"Length = %d");
      }
      else {
         pif_show_flagmask(0x07, 0x05, "Extended Length");
         pif_show_flagmask(0x07, 0x06, "Opening Tag");
         pif_show_flagmask(0x07, 0x07, "Closing Tag");
      }
      break;
   }
   
   if (tloc == 1)
   {
      // Show extedned tag, move cursor
      bac_show_byte("Extended tag type","[%u]");
   }
   
   if (lloc != 0) {
      len = pif_get_byte(0);
      switch (len) {
      case 254: 
         pif_show_byte("Length in next 2 octets");
         len = pif_get_word_hl(0);
         sprintf(outstr,"%"FW"s = %%u","Length of data");
         pif_show_word_hl(outstr);
         break;
         
      case 255: 
         pif_show_byte("Length in next 4 octets");
         len = pif_get_long_hl(0);
         sprintf(outstr,"%"FW"s = %%lu","Length of data");
         pif_show_long_hl(outstr);
         break;
         
      default: 
         bac_show_byte("Length of data","%u");
         break;
      }
   }
   
   if (showData)
   {
      if (!contextTag)
      {
         // Application tag
         switch (type) {
         case PRIM_NULL:        
            break; // value shown above: changes len
         case PRIM_BOOLEAN:  
            break; // value shown above: changes len
         case PRIM_UNSIGNED: 
            show_bac_unsigned(len);
            break;
         case PRIM_SIGNED:   
            show_bac_signed(len);
            break;
         case PRIM_REAL:     
            show_bac_real(len);
            break;
         case PRIM_DOUBLE:   
            show_bac_double(len);
            break;
         case PRIM_BIT_STRING:
            // This doesn't add anything useful, so just show the hex
            // show_bac_bitstring(len);
            sprintf(outstr,"%"FW"s = %%s","Bit string");
            pif_show_nbytes_hex(outstr,len);
            break;
         case PRIM_CHARACTER_STRING:
            show_bac_charstring(len);
            break;
         case PRIM_OCTET_STRING:
            sprintf(outstr,"%"FW"s = X'%%s'","Octet string");
            pif_show_nbytes_hex(outstr,len);
            break;
         case PRIM_ENUMERATED:
            show_bac_enumerated(len);
            break;
         case PRIM_DATE:     
            show_bac_date(len);
            break;
         case PRIM_TIME:     
            show_bac_time(len);
            break;
         case PRIM_OBJECT_IDENTIFIER:
            show_bac_object_identifier(len);
            break;
         default:       /* non-standard */
            sprintf(outstr,"%"FW"s = X'%%s'","Unknown Type: ");
            pif_show_nbytes_hex(outstr, len);
         }  /* end of type switch */
      }
      else
      {
         // Context tag
         if (!pairedTag) {
            if (len > 0) {
               // Don't know what this really is, so show hex bytes
               sprintf(outstr,"%"FW"s = X'%%s'", "tagged data: " );
               pif_show_nbytes_hex(outstr, len);
            }
         }
         else {
            if (len == 6) {
               // Opening tag.  Recursively call ourself until
               // the closing tag, or the end of the PDU.
               unsigned int firstContent = pif_offset;
               while (pif_offset < pif_end_offset) {
                  tagbuff = pif_get_byte(0);
                  unsigned int nextType = (tagbuff & 0xF0)>>4;
                  if (nextType >= 15) {
                     nextType = pif_get_byte(1); 
                  }

                  len = (tagbuff & 0x07);
                  if ((nextType == type) && (len == 7)) {
                     // Matching closing tag.  Show it and return
                     show_tagged_data();
                     break;
                  }

                  // Call ourself to decode and display this item as our child
                  show_head_tagged_data();
                  show_tagged_data();
               }

               // Return value is the number of bytes eaten as "data"
               // for the initial tag
               len = pif_offset - firstContent;
            }
            else {
               // Closing tag.  No data follows.
               len = 0;
            }
         }
      }
   }

   return len;
}

/**************************************************************************/
// Show deconstructed tag, whether application or contest
// Returns length of tag data
// Advances cursor past tag byte(s)
unsigned int show_tag(void)
{
   return show_tagged_data( false );
}

/*************************************************************************/
/* These functions are used to interpret all of the BACnet base types    */
/* that appear either as parameters in services or as properties.    */
/*************************************************************************/

/**************************************************************************/
// Show BACnetActionCommand
void show_bac_action_command( BACnetSequence &seq )
/**************************************************************************/
{
   seq.ObjectIdentifier(   0, "deviceIdentifier", BSQ_OPTIONAL );
   seq.ObjectIdentifier(   1, "objectIdentifier" );
   seq.PropertyIdentifier( 2, "propertyIdentifier" );
   seq.PropertyArrayIndex( 3, "propertyArrayIndex", BSQ_OPTIONAL );

   if (seq.OpeningTag(     4, "propertyValue" ))
   {
      show_bac_ANY( seq, seq.LastObjectType(), seq.LastPropertyID(), seq.LastPropertyIndex() );
      seq.ClosingTag();
   }
   seq.Unsigned(           5, "priority", BSQ_OPTIONAL );
   seq.Unsigned(           6, "postDelay", BSQ_OPTIONAL );
   seq.Boolean(            7, "quitOnFailure" );
   seq.Boolean(            8, "writeSuccessful" );
}

/**************************************************************************/
// Show BACnetActionList
void show_bac_action_list( BACnetSequence &seq )
{
   seq.ListOf( 0, "action" );
   while (seq.HasListElement())
   {
      show_bac_action_command( seq );
   }
}

/**************************************************************************/
// Show BACnetScale
void show_bac_scale( BACnetSequence &seq )
{
   seq.BeginChoice();
   seq.Real(    0, "floatingScale", BSQ_CHOICE );
   seq.Integer( 1, "integerScale", BSQ_CHOICE );
   seq.EndChoice();
}

/**************************************************************************/
// Show list of BACnetEventParameter
void show_bac_event_parameters( BACnetSequence &seq )
/**************************************************************************/
{
   seq.ListOf();
   while (seq.HasListElement())
   {
      seq.BeginChoice();
      if (seq.OpeningTag( 0, "Change of Bitstring parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(  0, "time-delay" );
         seq.BitString( 1, "bitmask" );
         seq.ListOf(    2, "list-of-bitstring-values" );
         while (seq.HasListElement())
         {
            seq.BitString( -1, "value" );
         }
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 1, "Change of State parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(  0, "time-delay" );
         seq.ListOf(    1, "list-of-values" );
         while (seq.HasListElement())
         {
            show_bac_property_states(seq);
         }
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 2, "Change of Value parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(       0, "time-delay" );
         if (seq.OpeningTag( 1, "cov-criteria" ))
         {
            seq.BeginChoice();
            seq.BitString( 0, "bitmask", NULL, BSQ_CHOICE );
            seq.Real(      1, "referenced-property-increment", BSQ_CHOICE );
            seq.EndChoice();
            seq.ClosingTag();
         }
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 3, "Command Fail parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(       0, "time-delay" );
         if (seq.OpeningTag( 1, "feedback-property-reference" ))
         {
            show_bac_dev_obj_prop_ref( seq );
            seq.ClosingTag();
         }
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 4, "Floating Limit parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(       0, "time-delay" );
         if (seq.OpeningTag( 1, "setpoint-reference" ))
         {
            show_bac_dev_obj_prop_ref( seq );
            seq.ClosingTag();
         }
         seq.Real(           2, "low-diff-limit" );
         seq.Real(           3, "high-diff-limit" );
         seq.Real(           4, "deadband" );
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 5, "Out of Range parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(       0, "time-delay" );
         seq.Real(           1, "low-limit" );
         seq.Real(           2, "high-limit" );
         seq.Real(           3, "deadband" );
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 8, "Change of Life Safety parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(       0, "time-delay" );
         seq.ListOf(         1, "list-of-life-safety-alarm-values" );
         while (seq.HasListElement())
         {
            seq.Enumerated( -1, "value", &BAC_STRTAB_BACnetLifeSafetyState );
         }

         seq.ListOf(         2, "list-of-life-safety-alarm-values" );
         while (seq.HasListElement())
         {
            seq.Enumerated( -1, "value", &BAC_STRTAB_BACnetLifeSafetyState );
         }
         
         if (seq.OpeningTag( 3, "mode-property-reference" ))
         {
            show_bac_dev_obj_prop_ref( seq );
            seq.ClosingTag();
         }

         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 9, "Extended Notification Parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(   0, "vendor-id" );
         seq.Unsigned(   1, "extended-event-type" );
         seq.ListOf(     2, "parameters" );
         while (seq.HasListElement())
         {
            seq.BeginChoice();
            seq.Null( -1, "null", BSQ_CHOICE );
            seq.Real( -1, "real", BSQ_CHOICE );
            seq.Unsigned( -1, "integer", BSQ_CHOICE );
            seq.Boolean( -1, "boolean", BSQ_CHOICE );
            seq.Double( -1, "double", BSQ_CHOICE );
            seq.OctetString( -1, "octet", BSQ_CHOICE );
            seq.BitString( -1, "bitstring", NULL, BSQ_CHOICE );
            seq.Enumerated( -1, "enumerated", NULL, BSQ_CHOICE );
            if (seq.OpeningTag( 0, "PropertyValue", BSQ_CHOICE ))
            {
               show_bac_dev_obj_prop_ref( seq );
               seq.ClosingTag();
            }
            seq.EndChoice();
         }

         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 10, "Buffer-Ready Notification Parameters", BSQ_CHOICE ))
      {
         seq.Unsigned( 0, "notification-threshold" );
         seq.Unsigned( 1, "previous-notification-count" );
         seq.ClosingTag();
      }
      else if (seq.OpeningTag( 11, "Unsigned-range Notification Parameters", BSQ_CHOICE ))
      {
         seq.Unsigned(  0, "time-delay" );
         seq.Unsigned(  1, "low-limit" );
         seq.Unsigned(  2, "high-limit" );
         seq.ClosingTag();
      }

      seq.EndChoice();
   }
}

/**************************************************************************/
void show_bac_object_identifier( unsigned int len )
/**************************************************************************/
{  /* Display and interpret a BACnet Object Identifier */
   long  obj_id;
   int   obj_type;
   long  obj_instance;
   
   if (len != 4)
   {
      ShowErrorDetail("Error: expected object identifier, but length is %u", len);
     return;
   }

   for (int i = 0; i < 4; i++)
      obj_id = (obj_id << 8) | (unsigned char)pif_get_byte( i );
   
   obj_type = (obj_id >> 22) & 0x000003FF;
   obj_instance = (obj_id & 0x003FFFFF);
   
   bac_show_nbytes( 4, "BACnet Object Identifier" );
   
   // TODO:
   if(obj_type > 63) { /* proprietary object type */
      sprintf(get_int_line(pi_data_current, pif_offset - 4, 4),
         "               Proprietary Object Type");
      sprintf(get_int_line(pi_data_current, pif_offset - 4, 4),
         "               Object Type = %u", obj_type );
      sprintf(get_int_line(pi_data_current, pif_offset - 4, 4),
         "               Instance Number = %lu", obj_instance );
   } else { /* standard object type */
      sprintf(get_int_line(pi_data_current, pif_offset - 4, 4),
         "               Standard Object Type");
      sprintf(get_int_line(pi_data_current, pif_offset - 4, 4),
         "               Object Type = %s",ObjectTypeString(obj_type));
      sprintf(get_int_line(pi_data_current, pif_offset - 4, 4),
         "               Instance Number = %lu", obj_instance );
   }
}

/**************************************************************************/
// Show a BACnetDeviceObjectReference
void show_bac_dev_obj_ref( BACnetSequence &seq )
/**************************************************************************/
{
   seq.ObjectIdentifier( 0, "deviceIdentifier", BSQ_OPTIONAL );
   seq.ObjectIdentifier( 1, "objectIdentifier" );
}

/**************************************************************************/
// Show BACnetPropertyStates
void show_bac_property_states( BACnetSequence &seq )
/**************************************************************************/
{
   seq.BeginChoice();
   seq.Boolean( 0, "boolean-value", BSQ_CHOICE );
   seq.Enumerated( 1, "binary-value",     &BAC_STRTAB_BACnetBinaryPV, BSQ_CHOICE );
   seq.Enumerated( 2, "event-type",       &BAC_STRTAB_BACnetEventType, BSQ_CHOICE );
   seq.Enumerated( 3, "polarity",         &BAC_STRTAB_BACnetPolarity, BSQ_CHOICE );
   seq.Enumerated( 4, "program-change",   &BAC_STRTAB_BACnetProgramRequest, BSQ_CHOICE );
   seq.Enumerated( 5, "program-state",    &BAC_STRTAB_BACnetProgramState, BSQ_CHOICE );
   seq.Enumerated( 6, "reason-for-halt",  &BAC_STRTAB_BACnetProgramError, BSQ_CHOICE );
   seq.Enumerated( 7, "reliability",      &BAC_STRTAB_BACnetReliability, BSQ_CHOICE );
   seq.Enumerated( 8, "state",            &BAC_STRTAB_BACnetEventState, BSQ_CHOICE );
   seq.Enumerated( 9, "system-status",    &BAC_STRTAB_BACnetDeviceStatus, BSQ_CHOICE );
   seq.Enumerated( 10, "units",           &BAC_STRTAB_BACnetEngineeringUnits, BSQ_CHOICE );
   seq.Unsigned(   11, "unsigned-value",  BSQ_CHOICE );

   seq.Enumerated( 12, "life-safety-mode", &BAC_STRTAB_BACnetLifeSafetyMode, BSQ_CHOICE );
   seq.Enumerated( 13, "life-safety-state", &BAC_STRTAB_BACnetLifeSafetyState, BSQ_CHOICE );
   seq.Enumerated( 14, "restart-reason",  &BAC_STRTAB_BACnetRestartReason, BSQ_CHOICE );
   seq.Enumerated( 15, "door-alarm-state", &BAC_STRTAB_BACnetDoorAlarmState, BSQ_CHOICE );
   seq.EndChoice();
}

/**************************************************************************/
// Show a BACnetRecipient
void show_bac_recipient( BACnetSequence &seq )
/**************************************************************************/
{
   seq.BeginChoice();
   seq.ObjectIdentifier( 0, "device", BSQ_CHOICE );
   if (seq.OpeningTag(   1, "address", BSQ_CHOICE ))
   {
      seq.Unsigned(    -1, "network-number" );
      seq.OctetString( -1, "mac-address" );
      seq.ClosingTag();
   }
   seq.EndChoice();
}

/****************************************************************************************
This function is used to decode and display the results of a read range or read property
result that is constructed similarly.
List of BACnetLogRecord, as property from ANY
*****************************************************************************************/ 
void show_log_buffer( BACnetSequence &seq )
{
   seq.ListOf( "listOfRecords" );
   while (seq.HasListElement())
   {
      if (seq.OpeningTag( 0, "timeStamp" ))
      {
         seq.Date( -1, "date" );
         seq.Time( -1, "time" );
         seq.ClosingTag();
      }

      if (seq.OpeningTag( 1, "logDatum" ))
      {
         seq.BeginChoice();
         seq.BitString(  0, "log-status", &BAC_STRTAB_BACnetLogStatus, BSQ_CHOICE );
         seq.Boolean(    1, "boolean-value", BSQ_CHOICE );
         seq.Real(       2, "real-value", BSQ_CHOICE );
         seq.Enumerated( 3, "enum-value", NULL, BSQ_CHOICE );
         seq.Unsigned(   4, "unsigned-value", BSQ_CHOICE );
         seq.Integer(    5, "signed-value", BSQ_CHOICE );
         seq.BitString(  6, "bitstring-value", NULL, BSQ_CHOICE );
         seq.Null(       7, "null-value", BSQ_CHOICE );
         if (seq.OpeningTag( 8, "failure", BSQ_CHOICE ))
         {
            show_error( seq );
            seq.ClosingTag();
         }
         seq.Real(           9, "time-change", BSQ_CHOICE );
         if (seq.OpeningTag( 10, "any-value", BSQ_CHOICE ))
         {
            // Allow an arbitrary sequence of tagged items until closing tag
            seq.AnyTaggedItem();
            seq.ClosingTag();
         }

         seq.EndChoice();
         seq.ClosingTag();
      }

      seq.BitString( 2, "statusFlags", &BAC_STRTAB_BACnetStatusFlags, BSQ_OPTIONAL );
   }
}

// BACnetEventLogRecord
void show_event_log_buffer( BACnetSequence &seq )
{
   seq.ListOf( "listOfRecords" );
   while (seq.HasListElement())
   {
      if (seq.OpeningTag( 0, "timeStamp" ))
      {
         seq.Date( -1, "date" );
         seq.Time( -1, "time" );
         seq.ClosingTag();
      }

      if (seq.OpeningTag( 1, "logDatum" ))
      {
         seq.BeginChoice();
         seq.BitString(      0, "log-status", &BAC_STRTAB_BACnetLogStatus, BSQ_CHOICE );
         if (seq.OpeningTag( 1, "notification", BSQ_CHOICE ))
         {
            show_EventNotification( seq );
            seq.ClosingTag();
         }
         seq.Real(           2, "time-change", BSQ_CHOICE );
         seq.EndChoice();
         seq.ClosingTag();
      }
   }
}

// BACnetLogMultipleRecord
void show_log_multiple_buffer( BACnetSequence &seq )
{
   seq.ListOf( "listOfRecords" );
   while (seq.HasListElement())
   {
      if (seq.OpeningTag( 0, "timeStamp" ))
      {
         seq.Date( -1, "date" );
         seq.Time( -1, "time" );
         seq.ClosingTag();
      }

      if (seq.OpeningTag( 1, "logData" ))
      {
         seq.BeginChoice();
         seq.BitString(  0, "log-status", &BAC_STRTAB_BACnetLogStatus, BSQ_CHOICE );
         if (seq.ListOf( 1, "log-data", BSQ_CHOICE ))
         {
            while (seq.HasListElement())
            {
               seq.BeginChoice();
               seq.Boolean(    0, "boolean-value", BSQ_CHOICE );
               seq.Real(       1, "real-value", BSQ_CHOICE );
               seq.Enumerated( 2, "enum-value", NULL, BSQ_CHOICE );
               seq.Unsigned(   3, "unsigned-value", BSQ_CHOICE );
               seq.Integer(    4, "signed-value", BSQ_CHOICE );
               seq.BitString(  5, "bitstring-value", NULL, BSQ_CHOICE );
               seq.Null(       6, "null-value", BSQ_CHOICE );
               if (seq.OpeningTag( 7, "failure", BSQ_CHOICE ))
               {
                  show_error( seq );
                  seq.ClosingTag();
               }
               if (seq.OpeningTag( 8, "any-value", BSQ_CHOICE ))
               {
                  // Allow an arbitrary sequence of tagged items until closing tag
                  seq.AnyTaggedItem();
                  seq.ClosingTag();
               }
               seq.EndChoice();
            }
         }
   
         seq.Real(       2, "time-change", BSQ_CHOICE );
         seq.EndChoice();
         seq.ClosingTag();
      }
   }
}

/**************************************************************************/
// Show BACnetRecipientProcess
void show_bac_recipient_process( BACnetSequence &seq )
{
   if (seq.OpeningTag( 0, "recipient" ))
   {
      show_bac_recipient( seq );
      seq.ClosingTag();
   }
   seq.Unsigned(       1, "processIdentifier" );
}

/**************************************************************************/
// Show BACnetSpecialEvent
void show_bac_special_event( BACnetSequence &seq )
{
   seq.BeginChoice();
   if (seq.OpeningTag( 0, "calendarEntry", BSQ_CHOICE ))
   {
      show_calendar_entry( seq );
      seq.ClosingTag();
   }
   seq.ObjectIdentifier( 1, "calendarReference", BSQ_CHOICE );
   seq.EndChoice();

   seq.ListOf( 2, "listOfTimeValues" );
   while (seq.HasListElement())
   {
      show_bac_time_value( seq );
   }

   seq.Unsigned( 3, "eventPriority" );
}

/**************************************************************************/
// BACnetShedLevel
void show_bac_shed_level( BACnetSequence &seq )
/**************************************************************************/
{
   seq.BeginChoice();
   seq.Unsigned( 0, "percent", BSQ_CHOICE );
   seq.Unsigned( 1, "level", BSQ_CHOICE );
   seq.Real(     2, "amount", BSQ_CHOICE );
   seq.EndChoice();
}

/**************************************************************************/
// Show a BACnetTimeValue
void show_bac_time_value( BACnetSequence &theSeq )
{
   theSeq.Time( -1, "time" );

   // Any primitive type
   theSeq.BeginWrap( "value" );
   theSeq.AnyTaggedItem( false );
   theSeq.EndWrap();
}

/**************************************************************************/
// Show BACnetVtSession
void show_bac_VT_session( BACnetSequence &theSeq )
/**************************************************************************/
{
   theSeq.Unsigned( -1, "local-vtSessionID" );
   theSeq.Unsigned( -1, "remote-vtSessionID" );
   show_bacnet_address( theSeq, "remote-vtAddress" );
}

//Lei Chengxin 2003-7-25
void show_head_obj_id( unsigned int offset , const char* type , int tagval )
{
   unsigned int len = pif_get_byte(offset-1) & 0x07;
   if (len != 4)
   {
      ShowErrorDetail("Error: expected object identifier, but length is %u", len);
      return;
   }

   unsigned long obj_id = get_bac_unsigned(offset, len);
   unsigned int  obj_type = (obj_id >> 22) & 0x000003FF;
   unsigned int  obj_instance = (obj_id & 0x003FFFFF);
   
   sprintf( outstr, "%s,%lu", ObjectTypeString(obj_type), obj_instance );
   
   if(tagval == -1)
   {
      sprintf(get_int_line(pi_data_current,pif_offset,len+1,NT_ITEM_HEAD), 
            "%s:  %s", type, outstr);
   }
   else
   {
      sprintf(get_int_line(pi_data_current,pif_offset,len+1,NT_ITEM_HEAD), 
            "[%u] %s:  %s", tagval, type, outstr);
   }
}

//Lei Chengxin 2003-7-30
void show_head_unsigned(unsigned int offset, const char* type, int tagval)
{
   int len = pif_get_byte(offset-1)&0x07;
   unsigned long value = get_bac_unsigned(offset, len);
   if(tagval == -1)
      sprintf(get_int_line(pi_data_current,pif_offset,len+offset,NT_ITEM_HEAD), 
         "%s:  %lu", type, value);
   else
      sprintf(get_int_line(pi_data_current,pif_offset,len+offset,NT_ITEM_HEAD), 
         "[%u] %s:  %lu", tagval, type, value);
}

// Show an enumeration using the table, or numerically if out of
// range of the table, or no table provided
void show_head_enumerated(unsigned int offset, const char* type, int tagval, 
                    const char* const table[], unsigned int tableLen)
{
   int len = pif_get_byte(offset-1)&0x07;
   unsigned long value = get_bac_unsigned(offset, len);
   if ((table != NULL) && (value < tableLen))
   {
      if(tagval == -1)
         sprintf(get_int_line(pi_data_current,pif_offset,len+offset,NT_ITEM_HEAD), 
               "%s:  %s (%u)", type, table[value], value);
      else
         sprintf(get_int_line(pi_data_current,pif_offset,len+offset,NT_ITEM_HEAD), 
               "[%u] %s:  %s (%u)", tagval, type, table[value], value);
   }
   else
   {
      show_head_unsigned( offset, type, tagval );
   }
}

//Lei Chengxin 2003-7-31
void show_head_ascii(const char* type)
{
   sprintf(get_int_line(pi_data_current,pif_offset,0,NT_ITEM_HEAD), "%s", type);
}

void show_head_char_string(unsigned int offset, const char* type, int tagval)
{
   // Reparse the tag, since we may have extended length
   BacParser parser( offset );
   int strLength = parser.DataLength();
   offset = parser.Offset();
   
   char strBuff[MAX_INT_LINE];
    get_bac_charstring( strLength, strBuff, sizeof(strBuff), 0, offset );

   char *pBuf = get_int_line(pi_data_current, pif_offset, offset+strLength, NT_ITEM_HEAD);
   if (tagval == -1)
      strLength = sprintf(pBuf, "%s:  ", type);
   else
      strLength = sprintf(pBuf, "[%d] %s:  ", tagval, type);

   SafeCopy( pBuf + strLength, strBuff, MAX_INT_LINE - strLength );
}

void show_head_date(unsigned int offset, const char* type, int tagval)
{
   unsigned int len = pif_get_byte(offset-1)&0x07;
   if (len != 4)
   {
      ShowErrorDetail("Error: expected date, but length is %u", len);
      return;
   }

   date_as_string( outstr, offset );
   
   if(tagval == -1)
      sprintf(get_int_line(pi_data_current,pif_offset,5,NT_ITEM_HEAD), 
         "%s:  %s", type, outstr);
   else
      sprintf(get_int_line(pi_data_current,pif_offset,5,NT_ITEM_HEAD), 
         "[%d] %s:  %s", tagval, type, outstr);
}

//Lei Chengxin 2003-7-31
void show_head_time(unsigned int offset, const char* type, int tagval)
{
   unsigned char x;
   char tempstr[80];

   unsigned int len = pif_get_byte(offset-1)&0x07;
   if (len != 4)
   {
      ShowErrorDetail("Error: expected time, but length is %u", len);
      return;
   }

   x = pif_get_byte(offset);
   if (x == 255)
      sprintf(outstr,"XX:");
   else
      sprintf(outstr,"%u:",x);
   
   x = pif_get_byte(1+offset);
   if (x == 255)
      strcat(outstr,"XX:");
   else {
      sprintf(tempstr,"%02u:",x);
      strcat(outstr,tempstr);
   }
   
   x = pif_get_byte(2+offset);
   if (x == 255)
      strcat(outstr,"XX.");
   else {
      sprintf(tempstr,"%02u.",x);
      strcat(outstr,tempstr);
   }
   
   x = pif_get_byte(3+offset);
   if (x == 255)
      strcat(outstr,"XX");
   else {
      sprintf(tempstr,"%02u",x);
      strcat(outstr,tempstr);
   }
   if(tagval == -1)
      sprintf(get_int_line(pi_data_current,pif_offset,5,NT_ITEM_HEAD), 
         "%s:  %s", type, outstr);
   else
      sprintf(get_int_line(pi_data_current,pif_offset,5,NT_ITEM_HEAD), 
         "[%d] %s:  %s", tagval, type, outstr);
}

//Lei Chengxin 2003-7-31
// Offset points at first data byte
void show_head_property_ID( unsigned int offset , const char* type , int tagval )
{
   show_head_enumerated( offset, type, tagval, 
                    BAC_STRTAB_BACnetPropertyIdentifier.m_pStrings, 
                    BAC_STRTAB_BACnetPropertyIdentifier.m_nStrings );
}

// Show the tag and value of an item.
// Does not affect the cursor.
void show_head_tagged_data( void )
{
   unsigned char tagbuff;
   unsigned int dataType, len, val;
   
   tagbuff = pif_get_byte(0);
   bool contextTag = (tagbuff & 0x08) != 0;
   dataType = tagbuff>>4;

   // Note that datatypes that may have more than 4 bytes of data
   // will need to reparse the full length.
   len = pif_get_byte(0) & 0x07;

   if (!contextTag) {
      switch (dataType) {
      case PRIM_NULL:
         sprintf(get_int_line(pi_data_current,pif_offset,1,NT_ITEM_HEAD), "NULL");
         break;
      case PRIM_BOOLEAN:
         val = pif_get_byte(0) & 0x07;
         sprintf(get_int_line(pi_data_current,pif_offset,1,NT_ITEM_HEAD), "Boolean: %s",
                (val == 0) ? "FALSE" :
               (val == 1) ? "TRUE" :
               "Illegal value");
         break;
      case PRIM_UNSIGNED: 
         show_head_unsigned(1, "Unsigned", -1 );
         break;
      case PRIM_SIGNED: 
         show_head_signed(1, "Signed", -1 );
         break;
      case PRIM_REAL: 
         show_head_real(1, "Real", -1 );
         break;
      case PRIM_DOUBLE: 
         show_head_double(0, "Double", -1 );
         break;
      case PRIM_BIT_STRING:
         show_head_bit_string(0, "Bit String", -1 );
         break;
      case PRIM_CHARACTER_STRING:
         show_head_char_string(0, "Character String", -1 );
         break;
      case PRIM_OCTET_STRING:
         show_head_octet_string(0, "Octet String", -1 );
         break;
      case PRIM_ENUMERATED:
         show_head_unsigned(1, "Enumerated", -1 );
         break;
      case PRIM_DATE:
         show_head_date(1, "Date", -1 );
         break;
      case PRIM_TIME: 
         show_head_time(1, "Time", -1 );
         break;
      case PRIM_OBJECT_IDENTIFIER:
         show_head_obj_id(1, "Object Identifier", -1 );
         break;
      default: 
         ShowErrorDetail("Error: claimed application tag %u", dataType );
         break;
      }  /* end of type switch */
   }
    else 
   {
      // Context tagged item
      // Show as level 1 item
      // - if fixed-length, highlight that length
      // - if paired, highlight up until the closing tag

      // Reparse, since we may have extended tag value or length
      BacParser parser(0);
      if (parser.FixedTag())
      {
         // Fixed-delimiter item.  
         // Show it as hex (since we don't know the actual type)
         // and highlight all of its data
         if (parser.DataLength() > 0)
         {
            char *pStr = outstr;
            pStr += sprintf( pStr, "hex data:" );
            for (unsigned int ix = 0; ix < parser.DataLength(); ix++)
            {
               if (ix >= MAX_SUM_LINE/3)
               {
                  // Limit the length
                  pStr += sprintf( pStr, "..." );
                  break;
               }
               pStr += sprintf( pStr, " %02X", pif_get_byte(parser.Offset()+ix) );
            }
            *pStr = 0;
         }
         else
         {
            strcpy( outstr, "no data" );
         }
         
         parser.EatData();
         sprintf( get_int_line(pi_data_current,pif_offset, parser.Offset(), NT_ITEM_HEAD),
                  "[%u] Context tag with %s", 
                parser.TagValue(), outstr );
      }
      else if (parser.OpeningTag())
      {
         // Opening tag
         unsigned int tagValue = parser.TagValue();
         parser.EatData(); // bypass the contents
         parser.ParseTag();   // bypass the closing tag, so Offset() includes it

         // Show using NT_OPEN_TAG so tree can auto-expand
         sprintf( get_int_line(pi_data_current,pif_offset, parser.Offset(), NT_OPEN_TAG),
                  "[%u] Opening Context tag", 
                tagValue );
      }
      else
      {
         // Closing tag.
         sprintf( get_int_line(pi_data_current,pif_offset, parser.Offset(), NT_ITEM_HEAD),
                  "[%u] Closing Context tag", 
                parser.TagValue() );
      }
   }
}

//Lei Chengxin 2003-8-23
// offset points at tag octet
void show_head_octet_string( unsigned int offset , const char* type , int tagval )
{
   // Reparse the tag, since we may have extended length
   BacParser parser( offset );
   int strLength = parser.DataLength();
   offset = parser.Offset();

   char *pStr = outstr;
   *pStr = 0;
   for (int i = 0; i < strLength; i++)
   {
      if ((pStr - outstr) >= MAX_SUM_LINE)
      {
         // Limit the length
         strcpy( pStr, "..." );
         break;
      }
      pStr += sprintf( pStr, "%02X", pif_get_byte(offset+i) );
   }

   if(tagval == -1)
      sprintf(get_int_line(pi_data_current,pif_offset, offset + strLength, NT_ITEM_HEAD), 
             "%s:  X'%s'", type, outstr);
   else
      sprintf(get_int_line(pi_data_current,pif_offset, offset + strLength, NT_ITEM_HEAD), 
             "[%d] %s:  X'%s'", tagval, type, outstr);
}

// offset points at first data octet
void show_head_signed( unsigned int offset , const char* type , int tagval )
{
   int len = pif_get_byte(offset-1) & 0x07;
   unsigned int val = 0;
   if (pif_get_byte(offset) & 0x80)
   {
      val = (unsigned int)-1;
   }

   for (int ix = 0; ix < len; ix++)
   {
      val = (val << 8) | pif_get_byte(offset + ix);
   }

   if(tagval == -1)
      sprintf(get_int_line(pi_data_current,pif_offset,len+1,NT_ITEM_HEAD), 
         "%s:  %d", type, (int)val);
   else
      sprintf(get_int_line(pi_data_current,pif_offset,len+1,NT_ITEM_HEAD), 
         "[%d] %s:  %d", tagval, type, (int)val);
}

void show_head_real( unsigned int offset , const char* type , int tagval )
{
   float fx;
   unsigned char fstr[4];
   unsigned int i;

   unsigned int len = pif_get_byte(offset-1)&0x07;
   if (len != 4)
   {
      ShowErrorDetail("Error: expected real, but length is %u", len);
      return;
   }
      
#if LITTLEENDIAN
   for (i=0;i<4;i++) 
      fstr[i] = pif_get_byte(offset+3-i);
#else
   for (i=0;i<4;i++) 
      fstr[i] = pif_get_byte(offset+i);
#endif
   fx = *(float*)fstr;

   // We use FloatToString() to get nice treatment of NaN and inf
   CString fStr;
   if (tagval == -1)
      sprintf(get_int_line(pi_data_current,pif_offset,4+1,NT_ITEM_HEAD), 
              "%s:  %s", type, FloatToString(fStr,fx));
   else
      sprintf(get_int_line(pi_data_current,pif_offset,4+1,NT_ITEM_HEAD), 
              "[%d] %s:  %s", tagval, type, FloatToString(fStr,fx));
}

// offset points at tag octet
void show_head_double( unsigned int offset, const char* type , int tagval )
{
   double dx;
   unsigned char fstr[8];
   unsigned int i;

   // Reparse the tag, since we may have extended length
   BacParser parser( offset );
   if (parser.DataLength() != 8)
   {
      ShowErrorDetail("Error: expected double, but length is %u", parser.DataLength());
      return;
   }
   offset = parser.Offset();
      
#if LITTLEENDIAN
   for (i=0;i<8;i++) 
      fstr[i] = pif_get_byte(offset+7-i);
#else
   for (i=0;i<8;i++) 
      fstr[i] = pif_get_byte(offset+i);
#endif
   dx = *(double*)fstr;

   // We use DoubleToString() to get nice treatment of NaN and inf
   CString fStr;
   if(tagval == -1)
      sprintf(get_int_line(pi_data_current,pif_offset,offset+8,NT_ITEM_HEAD), 
              "%s:  %s", type, DoubleToString(fStr,dx));
   else
      sprintf(get_int_line(pi_data_current,pif_offset,offset+8,NT_ITEM_HEAD), 
              "[%d] %s:  %s", tagval, type, DoubleToString(fStr,dx));
}

// offset points at tag octet
void show_head_bit_string( unsigned int offset, const char* type, int tagval )
{
   // Reparse the tag, since we may have extended length
   BacParser parser( offset );
   offset = parser.Offset();
   int byteLength = parser.DataLength();
   int unusedBits = pif_get_byte(offset);
   if ((byteLength < 2) || (unusedBits > 7))
   {
      ShowErrorDetail("Error: bitstring claims %u bytes with %u unused bits", 
                  byteLength, unusedBits);
      return;
   }

   int val, jx = 1, mask = 0;
   int bitLength = (byteLength - 1)*8 - unusedBits;
   char *pStr = outstr;
   for (int ix = 0; ix < bitLength; ix++)
   {
      if ((pStr - outstr) >= MAX_SUM_LINE)
      {
         // Limit the length
         pStr += sprintf( pStr, "..." );
         break;
      }

      if (mask == 0)
      {
         mask = 0x80;
         val = pif_get_byte(offset + jx);
         jx += 1;
         if (ix != 0)
            *pStr++ = ' ';
      }

      *pStr++ = (val & mask) ? 'T' : 'F';
      mask = mask >> 1;
   }
   *pStr = 0;

   if(tagval == -1)
      sprintf(get_int_line(pi_data_current,pif_offset, offset+byteLength, NT_ITEM_HEAD), 
             "%s:  {%s}", type, outstr);
   else
      sprintf(get_int_line(pi_data_current,pif_offset, offset+byteLength, NT_ITEM_HEAD), 
             "[%d] %s:  {%s}", tagval, type, outstr);
}

// Show an opening or closing context tag by name.
// If showContent is true,
// - show the tagging
// - show the contents
// - show the closing context tag, using the same name
// - advance cursor past the closing tag
// Else the cursor is not moved
//
// Return true if the specified context tag is found, else false
//
bool show_head_context_tag( unsigned int offset, const char* type, int tagval, bool showContent )
{
   // Reparse the tag, since we may have extended length
   BacParser parser( offset );
   if (parser.TagValue() != (unsigned int)tagval)
   {
      ShowErrorDetail("Error: expected tag %u (%s), got %u", 
            tagval, type, parser.TagValue());
      return false;
   }

   bool isOpening = parser.OpeningTag();

   // Bypass the item, so we can highlight it
   parser.EatData();

   if (isOpening)
   {
      // Parse closing tag, so we highlight its length with our own
      parser.ParseTag();

      // Use NT_OPEN_TAG, so this can be auto-expanded in the view tree
      sprintf(get_int_line(pi_data_current,pif_offset, parser.Offset(), NT_OPEN_TAG), 
            "[%d] %s:  Paired tag", tagval, type);
      if (showContent)
      {
         // Deconstruct the tag and show the contents, advancing cursor
         show_tagged_data();

         // Deconstruct the closing tag and advance cursor
         // Don't show as a header line, since we are indenting this under the 
         //   show_head_context_tag(0, type, tagval, true);
         show_tag();
      }
   }
   else
   {
      sprintf(get_int_line(pi_data_current,pif_offset, parser.Offset(), NT_ITEM_HEAD), 
            "[%d] %s:  Closing tag", tagval, type);
      if (showContent)
      {
         // Deconstruct the tag and show the contents, advancing cursor
         show_tagged_data();
      }
   }

   return true;
}

// Show a BACnetCOVSubscription
void show_bac_COV_Subscription( BACnetSequence &seq )
{
   if (seq.OpeningTag( 0, "recipient" ))
   {
      show_bac_recipient_process( seq );
      seq.ClosingTag();
   }
   if (seq.OpeningTag( 1, "monitoredPropertyReference" ))
   {
      show_bac_object_property_reference( seq );
      seq.ClosingTag();
   }
   seq.Boolean(        2, "issueConfirmedNotifications" );
   seq.Unsigned(       3, "timeRemaining" );
   seq.Real(           4, "covIncrement", BSQ_OPTIONAL );
}

// Show a BACnetDestination
void show_bac_destination( BACnetSequence &seq )
{
   seq.BitString( -1, "validDays", &BAC_STRTAB_BACnetDaysOfWeek );
   seq.Time(      -1, "fromTime" );
   seq.Time(      -1, "toTime" );
   show_bac_recipient(seq);
   seq.Unsigned(  -1, "processIdentifier" );
   seq.Boolean(   -1, "issueConfirmedNotifications" );
   seq.BitString( -1, "transitions", &BAC_STRTAB_BACnetEventTransitionBits );
}

}
