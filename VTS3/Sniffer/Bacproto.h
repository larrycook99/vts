/*  This version of BACPROTO.H is compatible with BACSN015.C  */
// Revision: March 1 2014 Updated to 135-2012

class BACnetSequence;


/* ----  property identifiers ---*/
// 1 March 2013: use this enum rather than (DOH!) defining AGAIN here
#include <Propid.h>

   /* ----- prototypes for interpreter functions ----- */

int     interp_bacnet_IP( char *header, int length);
int     interp_bacnet_ETHERNET( char *header, int length);
int     interp_bacnet_ARCNET( char *header, int length);
int     interp_bacnet_MSTP( char *header, int length);
int     interp_bacnet_PTP( char *header, int length);
int     interp_bacnet_BVLL( char *header, int length);
int     interp_bacnet_NL( char *header, int length);
int     interp_bacnet_AL( char *header, int length );
int     interp_Message( char *header, int length);
int     interp_TextMessage( char *header, int length);

/* ----- prototypes for interpreters of the 8 PDU types ----- */

void	show_confirmed(unsigned char);
void    show_unconfirmed(unsigned char);
void    show_simple_ack(unsigned char);
void    show_complex_ack(unsigned char);
void    show_segment_ack(unsigned char);
void    show_error(unsigned char);
void    show_reject(unsigned char);
void    show_abort(unsigned char);

/* ----- prototypes for confirmed service interpreter functions ----- */

void    show_acknowledgeAlarm(void);
void    show_confirmedCOVNotification(void);
void    show_confirmedEventNotification(void);
void    show_getAlarmSummary(void);
void    show_getEnrollmentSummary(void);
void    show_getEventInformation(void); //Added by Zhu Zhenhua, 2004-5-25
void    show_getEventInformationACK(void); //Added by Zhu Zhenhua, 2004-5-25
void    show_subscribeCOV(void);
void    show_subscribeCOV_Property(void);
void    show_atomicReadFile(void);
void    show_atomicWriteFile(void);
void    show_addListElement(void);
void    show_removeListElement(void);
void    show_createObject(void);
void    show_deleteObject(void);
void    show_readProperty(void);
void    show_readPropertyConditional(void);
void    show_readPropertyMultiple(void);
void    show_writeProperty(void);
void    show_writePropertyMultiple(void);
void    show_deviceCommunicationControl(void);
void    show_privateTransfer(void);
void    show_confirmedTextMessage(void);
void    show_reinitializeDevice(void);
void    show_vtOpen(void);
void    show_vtClose(void);
void    show_vtData(void);
void    show_authenticate(void);
void    show_requestKey(void);
void    show_ReadRange(void);
/* ----- prototypes for unconfirmed service interpreter functions ----- */

void     show_iAm(void);
void     show_iHave(void);
void     show_unconfirmedCOVNotification(void);
void     show_unconfEventNotification(void);
void     show_unconfPrivateTransfer(void);
void     show_unconfTextMessage(void);
void     show_timeSynchronization(void);
void     show_whoHas(void);
void     show_whoIs(void);
void     show_UTCtimeSynchronization(void);
/* ----- prototypes for complex ACK interpreter functions ----- */

void     show_getAlarmSummaryACK(void);
void     show_getEnrollmentSummaryACK(void);
void     show_atomicReadFileACK(void);
void     show_atomicWriteFileACK(void);
void     show_createObjectACK(void);
void     show_readPropertyACK(void);
void     show_readPropertyConditionalACK(void);
void     show_readPropertyMultipleACK(void);
void     show_conf_PrivateTransferACK(void);
void     show_vtOpenACK(void);
void     show_vtDataACK(void);
void     show_authenticateACK(void);
void     show_ReadRangeACK(void);
/* ----- prototypes for error interpreter functions ----- */

void	 show_wrapped_error( BACnetSequence &theSeq, int theTag = 0, const char *pTheString = NULL );
void     show_error_codes(void);
void     show_createObjectError(void);
void     show_writePropertyMultipleError(void);
void     show_vtCloseError(void);

/*  ----- prototypes for general purpose pif routines ----- */

void     bac_show_byte(const char *,const char *);
void     bac_show_bipaddr(const char *);
void     bac_show_flag(const char *, unsigned char);
void     bac_show_flagmask (unsigned char, const char *);
void     bac_show_nbytes(unsigned int, const char *);
void     bac_show_word_hl(const char *, const char *);
void     bac_show_long_hl(const char *, const char *);
void     show_str_eq_str(const char *, const char *, unsigned int);
unsigned int get_bac_unsigned(int offset, int length);

/*  ----- prototypes for displaying other PDU components ----- */

void ShowErrorDetail( const char *pFormat = NULL, ... );

// Show deconstructed tag followed by value
unsigned int show_tagged_data( bool showData = true );

// Show deconstructed tag, whether application or contest
// Returns length of tag data
// Advances cursor past tag byte(s)
unsigned int show_tag(void);

/*  ----- prototypes for displaying primitive data types ----- */

void     show_bac_ANY( BACnetSequence &seq, int obj_type, unsigned int prop_id, int prop_idx );
void     show_bac_bitstring(unsigned int);
void     show_bac_charstring(unsigned int);
void     show_bac_octetstring(unsigned int);
void     show_bac_date(unsigned int len);
void     show_bac_double(unsigned int len);
void     show_bac_object_identifier(unsigned int len);
void     show_bac_real(unsigned int len);
void     show_bac_signed(unsigned int);
void     show_bac_time(unsigned int len);
void     show_bac_unsigned(unsigned int);
void	 show_bac_enumerated( unsigned int len );

/*  ----- prototypes for displaying BACnet base types ----- */

void	 show_bac_action_list( BACnetSequence &seq );
void     show_bac_action_command( BACnetSequence &seq );
void	 show_bacnet_address( BACnetSequence &theSeq, const char *pTheTitle );
void     show_bac_event_parameters( BACnetSequence &seq );
void     show_bac_dev_obj_ref( BACnetSequence &theSeq );
void	 show_read_access_spec( BACnetSequence &theSeq );
void	 show_read_access_result( BACnetSequence &theSeq );
void	 show_bac_object_property_reference( BACnetSequence &theSeq );
void     show_bac_property_states( BACnetSequence &theSeq );
void	 show_bac_recipient( BACnetSequence &theSeq );
void     show_bac_recipient_process( BACnetSequence &theSeq );
void     show_bac_special_event( BACnetSequence &theSeq );
void	 show_bac_shed_level( BACnetSequence &theSeq );
void	 show_bac_time_value( BACnetSequence &theSeq );
void     show_bac_VT_session( BACnetSequence &theSeq );
void     show_log_buffer( BACnetSequence &theSeq );
void	 show_event_log_buffer( BACnetSequence &theSeq );
void	 show_log_multiple_buffer( BACnetSequence &theSeq );
void     show_bac_COV_Subscription( BACnetSequence &theSeq );
void     show_bac_destination( BACnetSequence &theSeq );
void	 show_bac_commandable_value( BACnetSequence &theSeq, int theObjectType );
void	 show_bac_priority_value( BACnetSequence &theSeq, int theObjectType );
void	 show_time_stamp( BACnetSequence &theSeq, int theTag, const char *pTheTitle );

void	 show_bac_dev_obj_prop_ref( BACnetSequence &theSeq );
void	 show_bac_dev_obj_prop_val( BACnetSequence &theSeq );
void	 show_calendar_entry( BACnetSequence &theSeq );
void	 show_bac_scale( BACnetSequence &theSeq );

/*  ----- functions to add new tree node to the detail view ----- */

void	 show_head_obj_id( unsigned int , const char* , int );		// Lei Chengxin 2003-7-25		
void	 show_head_unsigned( unsigned int offset, const char* type, int tagval);		// Lei Chengxin 2003-7-30
void	 show_head_enumerated(unsigned int offset, const char* type, int tagval, 
							  const char* const table[], unsigned int tableLen);

void	 show_head_ascii( const char* );								// Lei Chengxin 2003-7-31
void	 show_head_char_string( unsigned int , const char* , int );	// Lei Chengxin 2003-7-31
void	 show_head_time( unsigned int , const char* , int );			// Lei Chengxin 2003-7-31
void	 show_head_date( unsigned int , const char* , int );
void	 show_head_property_ID( unsigned int , const char* , int );	// Lei Chengxin 2003-7-31
void     show_head_tagged_data( void );
void	 show_head_octet_string( unsigned int , const char* , int );	// Lei Chengxin 2003-8-23
void	 show_head_signed( unsigned int , const char* , int );		// Lei Chengxin 2003-8-23
void	 show_head_real( unsigned int , const char* , int );			// Lei Chengxin 2003-8-23
void	 show_head_double( unsigned int , const char* , int );
void     show_head_bit_string( unsigned int , const char* , int );	// Lei Chengxin 2003-8-23

bool     show_head_context_tag( unsigned int offset, const char* type, int tagval, bool showContent );
