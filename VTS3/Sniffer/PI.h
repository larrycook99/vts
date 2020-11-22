/*
	This file was originally part of a Protocol Interpreter for the 
	Network General Sniffer (tm).

    In the interests of clarity
	- Unused definitions and prototypes have been removed
	- The decoder functions and globals are put into the namespace "NetworkSniffer"
	  to limit the potential for collision with other code.  
	- The keywords "near" and "far" have been removed.

	WARNING

	None of this code is thread safe.  There is one global that points to the "current"
	BACnetPIInfo object.  Because in VTS this is called from the main GUI thread and 
	doesn't let other messages interfere, this should be OK.
*/

//
//	BACnetPIDetail
//
#define MAX_SUM_LINE    143             /* maximum width of a summary line */
#define MAX_INT_LINE    550             /* maximum width of a detail line */ // Increased line to allow 255 bytes
#define MAX_TEXT_ITEM   128				/* limit on strings within a line */

// Used to control/suggest how detail lines are displayed and expanded
enum PID_NODE_TYPE
{
	NT_NORMAL,		// normal line, let view categorize it
	NT_ITEM_HEAD,	// "head" of an item.  May be used as a hint by view
	NT_OPEN_TAG,	// Beginning of a context-tagged sequence.  Used by view to control expansion
	NT_ROOT,		// Reset to root of tree
	NT_ERROR		// Indicate an error line
};

struct BACnetPIDetail {
	int			piOffset;
	int			piLen;
	char		piLine[MAX_INT_LINE];

	PID_NODE_TYPE	piNodeType;
};

typedef BACnetPIDetail *BACnetPIDetailPtr;

//
// BACnetPIInfo
//
class BACnetPIInfo {
	public:
		// list the protocol interpreter types
		enum ProtocolType
			{ ipProtocol = 0
			, ethernetProtocol
			, arcnetProtocol
			, mstpProtocol
			, ptpProtocol
			, msgProtocol		// message from application with severity code, line number, digest, and text
			, textMsgProtocol	// message from application, text only
			};

		char				*piBuffer;		// beginning of buffer
		int					piLen;			// length
		int					piOffset;		// offset for current pi

		bool				doSummary;		// generate summary line
		bool				doDetail;		// generate detail lines

		char				summaryLine[MAX_SUM_LINE];

		BACnetPIDetailPtr	*detailLine;
		int					detailCount;
		int					detailMax;

		BACnetPIInfo( bool summary = false, bool detail = false );
		~BACnetPIInfo( void );

		void SetPIMode( bool summary, bool detail );	// set the mode for all of the PI's
		void Interpret( ProtocolType protocol, char *header, int length );
	};

typedef BACnetPIInfo *BACnetPIInfoPtr;

extern BACnetPIInfoPtr		gCurrentInfo;

/* put all the rest of this into a namespace */
#define USENAMESPACE	1

#if USENAMESPACE
namespace NetworkSniffer {
#endif

/* Are we a little endian (PC) or big endian (Macintosh)
 * (Conditional, to avoid warnings if WinSock2.h etc. is brought in) */
#ifndef LITTLEENDIAN
	#define LITTLEENDIAN	1
#endif
#ifndef BIGENDIAN
	#define BIGENDIAN		0
#endif

// sprintf of the byte at the current offset.
// format string prstr should have %s for hdr, %u for the byte, and %s for valu
// Offset pointer is advanced by one.
void xsprintf( char *dst, const char *prstr, const char *hdr, const char *valu );

/*----------------------------------------------------------------------------

								Miscellaneous symbols

-----------------------------------------------------------------------------*/

#ifndef TRUE
typedef int  boolean;                   /* word-sized boolean */
#define TRUE            1               /* Note: as a word, the msbyte is zero */
#define FALSE           0
#endif

typedef unsigned long int DLONG[2];

typedef long FRNUM;                     /* frame number type */
typedef unsigned long FRLOC;			/* location of frame header */
typedef unsigned int PORT;				/* port type */

#ifndef MSOFT                   
#define MSOFT 1                         /* define the compiler */
#endif

#if MSOFT
/* WAS pifdecl.h - start */

/*  Function prototype declarations for Protocol Interpreters   */

void	pif_init(struct pi_data *,void *,int );
char	*pif_get_ascii( int offset, int len, char result_str[], int max_chars );
//char	*pif_get_ebcdic(int ,int ,char *);
//char	*pif_get_lstring(int ,char *);
char	*pif_line(int );
void	pif_show_ascii(int ,char *);
void	pif_append_ascii(const char *, const char *); /* JJB */
// void	pif_show_ebcdic(int ,char *);
// void	pif_show_lstring(char *);
void	pif_show_space(void);
// void	pif_autoscroll(void);
void	pif_header(int ,char *);
// void	pif_trailer(void);
int		pif_show_byte(char *);
void	pif_show_flag(char *, int);
//void	pif_flag_w_hl(char *, int);
//void	pif_flag_w(char *, int);
int		pif_show_flagbit(int ,char *,char *);
// boolean pif_flagbit_w(int, char *, char *);
// boolean pif_flagbit_w_hl(int, char *, char *);
int		pif_show_flagmask(int ,int ,char *);
// void	pif_show_flag_string (char, int, char *, char **);
// void	pif_show_flag_value (char, int, char *);
// void	pif_show_2byte(char *);
// int	pif_show_word(char *);

// NOTE: at one time these functions did a hack compare
// on the format string to see if they should print as
// SIGNED values.  That capability has been removed.
int		pif_show_word_hl(char *);
long	pif_show_slong_hl(char *);  // 3 byte encoded
long    pif_show_nbyte_uint(char *, int len);
long	pif_show_long_hl(char *);

void    pif_show_nbytes_hex (char *, int);
int     pif_off_end (void);
char	*pif_off_endp (void);

// Values for get_int_line node type
char	*get_int_line (struct pi_data *, int offset, int length, PID_NODE_TYPE nodeType = NT_NORMAL);
char	*get_sum_line (struct pi_data *);
char	*get_cur_int_line( void ); /* JJB */

// Get the index of the next detail line
// Used to fix up the length of previous detail lines after subsequent
// items have been parsed.  Simplifies various wrapping items.
int get_next_detail_index( void );

// Set the data length of the specified detail line to theOffset.
// Used to fix up the length of previous detail lines after subsequent
// items have been parsed.  Simplifies various wrapping items.
void set_detail_line_length( int theLine, int theOffset );

// Safe strcpy that won't exceed maxLength (including null terminator)
// (Note that strncpy is NOT equivalent, as it doesn't guarantee null terminated output)
void SafeCopy( char *pDest, const char *pSource, int maxLength );

// Safe strcat that won't exceed maxLength (including null terminator)
void SafeAppend( char *pDest, const char *pSource, int maxLength );


int		rev_word(int );
long	rev_long(long );

/* end of WAS pifdecl.h */

#endif


#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))   /* minimum value */
#endif
#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))   /* maximum value */
#endif

#define assert(b,s) if (!(b)) bad_assertion (s) /* check logic error */

/*
        The Protocol Interpreter data structure, whose address
        is return by the register function register_pi().
*/

struct  pi_data {
        boolean     do_sum;         /* generate summary lines? */
        boolean     do_int;         /* generate interpretation (detail) lines? */
        boolean     do_count;       /* only count summary lines? */
        boolean     do_names;       /* add symbolic station names? */
        boolean     recursive;      /* is this a recursive call? */
        boolean     selected;       /* is this PI selected in the menu? */
		struct forcepi_rule	*forcepi_rules;	/* linked list of "Force PI" rules */
        };

/*-----------------------------------------------------------------------------

                        External references

------------------------------------------------------------------------------*/

extern  char *  near msg_origin;        /* ptr to start of frame or message */

/*-----------------------------------------------------------------------------

                        PIF symbols

------------------------------------------------------------------------------*/

/*      PIF external routines     */

#ifndef PIF_MAIN
extern  struct  pi_data * pif_pi;  /* Ptr to current pi */
extern  int     pif_offset;        /* Current offset */
extern  int     pif_end_offset;    /* Offset for last+1 byte */
extern  int     pif_flagbit_indent;/* Number of blanks for flagbit */
extern  char    pif_header_msg[];       /* Saved header message */

extern  long    rev_long();             /* Reverse a long word */
extern  char    *pif_line();            /* get a detail line buffer */
#endif


/* The following MACROs will check the last byte addressed for End of Frame 
   If the last byte is greater than pif_end_offset the MACRO will execute
   pif_off_end and return to the most recent end-of-frame trap routine. */

// #Change made by Buddy Lott to correct VTS crashes #1608485
//#define pif_get_byte(n)      ((unsigned char)(*(msg_origin + pif_offset + (n))))
// TODO: Turn these into real functions (perhaps inlined).
// Otherwise calls like pif_get_byte( offset++ ) double increment
// Bugs like that we don't need...
#define pif_get_byte(n) (( pif_offset + (int)(n) >= pif_end_offset) ? 0 : ((unsigned char)(*(msg_origin + pif_offset + (n)))))

#if LITTLEENDIAN
/*  These are little endian defines - JJB */
#define pif_get_word(n)      (*(short *)(msg_origin + pif_offset + (n)))
#define pif_get_word_hl(n)   (rev_word(*(short *)(msg_origin + pif_offset + (n))))
#define pif_get_long(n)      (*(long *)(msg_origin + pif_offset + (n)))
#define pif_get_long_hl(n)   (rev_long(*(long *)(msg_origin + pif_offset + (n))))
#endif

#if BIGENDIAN
/* These are big endian defines - JJB */
#define pif_get_word(n)      (rev_word(*(short *)(msg_origin + pif_offset + (n))))
#define pif_get_word_hl(n)   (*(short *)(msg_origin + pif_offset + (n)))
#define pif_get_long(n)      (rev_long(*(long *)(msg_origin + pif_offset + (n))))
#define pif_get_long_hl(n)   (*(long *)(msg_origin + pif_offset + (n)))
#endif

#define pif_get_addr()       (msg_origin + pif_offset)
#define pif_set(p)           (pif_offset = ((char *)(p) - msg_origin))
#define pif_skip(n)          (pif_offset += (n))
#define pif_eof(n)           (pif_offset + n >= pif_end_offset)


#if USENAMESPACE
}
#endif

/* end of pi.h */
