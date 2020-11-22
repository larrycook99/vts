// DUDTOOL.H


#ifndef __DUDTOOL_H_INCLUDED
#define __DUDTOOL_H_INCLUDED

#define ptDate 200  // parsetype BACnetDate
#define ptTime 201  // parsetype BACnetTime
#define lobj   202  // array of objects, (parse type)
#define vtse   203  // list of active  vt sessions (parse type)
#define uwarr  204  // array of uw

// property access functions 
int  ai(char far* Buffer, PVMessage far* msg);
int  ao(char far* Buffer, PVMessage far* msg);
int  av(char far* Buffer, PVMessage far* msg);
int  bi(char far* Buffer, PVMessage far* msg);
int  bo(char far* Buffer, PVMessage far* msg);
int  bv(char far* Buffer, PVMessage far* msg);
int  cal(char far* Buffer, PVMessage far* msg);
int  com(char far* Buffer, PVMessage far* msg);
int  dev(char far* Buffer, PVMessage far* msg);
int  eve(char far* Buffer, PVMessage far* msg);
int  flo(char far* Buffer, PVMessage far* msg);
int  gru(char far* Buffer, PVMessage far* msg);
int  loo(char far* Buffer, PVMessage far* msg);
int  msi(char far* Buffer, PVMessage far* msg);
int  mso(char far* Buffer, PVMessage far* msg);
int  nofy(char far* Buffer, PVMessage far* msg);
int  prg(char far* Buffer, PVMessage far* msg);
int  sced(char far* Buffer, PVMessage far* msg);


// encode simple properties:
octet far* ecBOOL(octet far* p, octet tag, boolean bval);
octet far* ecUINT(octet far* p, octet tag, dword property_id);

// encode list properties:
octet far* ePROPREF(octet far* p, const BACnetPropertyReference far* pv, int TagNr= -1);
octet far* eLOPREF(octet far* p, const BACnetObjectPropertyReference far* pv, int TagNr= -1);    
octet far* eSETREF(octet far* p, const BACnetObjectPropertyReference far* pv, int TagNr= -1);    
octet far* eRASLIST(octet far* p, const BACnetReadAccessSpecification far * pv, int TagNr= -1);
octet far* eRECIPLIST(octet far* p, const BACnetDestination far* pv, int TagNr= -1); 
octet far* eVTCL(octet far* p, const BACnetVTClassList far* pv, int TagNr= -1);
octet far* eDABIND(octet far* p, const BACnetAddressBinding far* pv, int TagNr= -1);
octet far* eTIMEVALUE(octet far* p, BACnetTimeValue far* ptv, int TagNr= -1);
octet far* eRECIP(octet far* p, const BACnetRecipient far* pv, int TagNr= -1);     
octet far* eCALIST(octet far* p, const BACnetCalendarEntry far* pv, int TagNr= -1);

// encode array properties:
octet far* eLOBJ(octet far* p, const BACnetObjectIdentifier far* pv, int ArrayIndex= -1, int TagNr= -1);
octet far* ePAF(octet far* p, const float far* pv, int ArrayIndex= -1, int TagNr= -1);
octet far* ePAB(octet far* p, const enum BACnetBinaryPV far* pv, int ArrayIndex= -1, int TagNr= -1);
octet far* ePAU(octet far* p, const word far* pv, int ArrayIndex= -1, int TagNr= -1);
octet far* eUWARR(octet far* p, const word far* pv, int num, int ArrayIndex= -1, int TagNr= -1);
octet far* eACTEXT(octet far* p, char far** pv, int num, int ArrayIndex= -1, int TagNr= -1);
octet far* eACT(octet far* p, BACnetActionCommand far** pv, int num, int ArrayIndex= -1, int TagNr= -1);
octet far* eWSCHED(octet far* p, BACnetTimeValue far** pv, int num, int ArrayIndex= -1, int TagNr= -1);
octet far* eXSCHED(octet far* p, BACnetExceptionSchedule far* pv, int ArrayIndex= -1, int TagNr= -1);

// encode struct properties:
octet far* eBACnetDate(octet far* p, const BACnetDate far* pv, int TagNr= -1);
octet far* eBACnetTime(octet far* p, const BACnetTime far* pv, int TagNr= -1);
octet far* eDT(octet far* p, const BACnetDateTime far* pv, int TagNr= -1);
octet far* eDTRANGE(octet far* p, const BACnetDateRange far* pv, int TagNr= -1);
octet far* eEVPARM(octet far* p, const BACnetEventParameter far* pv, int TagNr= -1);
octet far* eSKEYS(octet far* p, const BACnetSessionKey far* pv, int TagNr= -1);
octet far* eSTAVALS(octet far* p,const octet far* pv, int TagNr= -1);
octet far* eVTSE(octet far* p, const BACnetVTSession far* pv, int TagNr= -1);

octet far* eASN_1_AnyType(octet far* p, PVMessage far* msg);
int EncodeFromDatabase(char far* Buffer, PVMessage far* msg);

void CreateTestValues(void);
void DeleteTestValues(void);


void	CvtFltStr(double fval, char far* Buffer);
double	CvtStrFlt(char far* Buffer);
int     MyRand(int range);
void    Hex(char far* Buffer, octet Flag);


extern char gBuffer[256];


#endif // __DUDTOOL_H_INCLUDED
