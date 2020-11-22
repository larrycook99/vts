
#include "stdafx.h"

#include "pi.h"

//-----

BACnetPIInfoPtr		gCurrentInfo;

namespace NetworkSniffer {
	int interp_bacnet_IP( char *, int );
	int interp_bacnet_ETHERNET( char *, int );
	int interp_bacnet_ARCNET( char *, int );
	int interp_bacnet_MSTP( char *, int );
	int interp_bacnet_PTP( char *, int );
	int interp_bacnet_BVLL( char *, int );
	int interp_bacnet_NL( char *, int );
	int interp_bacnet_AL( char *, int );
	int interp_Message( char *, int );
	int interp_TextMessage( char *, int );

	struct pi_data pi_data_bacnet_IPRec, *pi_data_bacnet_IP = &pi_data_bacnet_IPRec;
	struct pi_data pi_data_bacnet_ETHERNETRec, *pi_data_bacnet_ETHERNET = &pi_data_bacnet_ETHERNETRec;
	struct pi_data pi_data_bacnet_ARCNETRec, *pi_data_bacnet_ARCNET = &pi_data_bacnet_ARCNETRec;
	struct pi_data pi_data_bacnet_MSTPRec, *pi_data_bacnet_MSTP = &pi_data_bacnet_MSTPRec;
	struct pi_data pi_data_bacnet_PTPRec, *pi_data_bacnet_PTP = &pi_data_bacnet_PTPRec;
	struct pi_data pi_data_bacnet_BVLLRec, *pi_data_bacnet_BVLL = &pi_data_bacnet_BVLLRec;
	struct pi_data pi_data_bacnet_NLRec, *pi_data_bacnet_NL = &pi_data_bacnet_NLRec;
	struct pi_data pi_data_bacnet_ALRec, *pi_data_bacnet_AL = &pi_data_bacnet_ALRec;
	struct pi_data pi_data_MessageRec, *pi_data_Message = &pi_data_MessageRec;
	}

//
//	BACnetPIInfo::BACnetPIInfo
//

BACnetPIInfo::BACnetPIInfo( bool summary, bool detail )
{
	summaryLine[0] = 0;
	detailLine = NULL;
	detailCount = 0;
	detailMax = 0;

	SetPIMode( summary, detail );
}

//
//	BACnetPIInfo::~BACnetPIInfo
//

BACnetPIInfo::~BACnetPIInfo( void )
{
	// toss any detail lines allocated
	for (int i = 0; i < detailCount; i++ )
		delete detailLine[i];

	delete[] detailLine;
}

//
//	BACnetPIInfo::SetPIMode
//

void BACnetPIInfo::SetPIMode( bool summary, bool detail )
{
	doSummary = summary;
	doDetail = detail;

	NetworkSniffer::pi_data_bacnet_IP->do_sum = summary;
	NetworkSniffer::pi_data_bacnet_IP->do_int = detail;
	NetworkSniffer::pi_data_bacnet_ETHERNET->do_sum = summary;
	NetworkSniffer::pi_data_bacnet_ETHERNET->do_int = detail;
	NetworkSniffer::pi_data_bacnet_ARCNET->do_sum = summary;
	NetworkSniffer::pi_data_bacnet_ARCNET->do_int = detail;
	NetworkSniffer::pi_data_bacnet_MSTP->do_sum = summary;
	NetworkSniffer::pi_data_bacnet_MSTP->do_int = detail;
	NetworkSniffer::pi_data_bacnet_PTP->do_sum = summary;
	NetworkSniffer::pi_data_bacnet_PTP->do_int = detail;
	NetworkSniffer::pi_data_bacnet_BVLL->do_sum = summary;
	NetworkSniffer::pi_data_bacnet_BVLL->do_int = detail;
	NetworkSniffer::pi_data_bacnet_NL->do_sum = summary;
	NetworkSniffer::pi_data_bacnet_NL->do_int = detail;
	NetworkSniffer::pi_data_bacnet_AL->do_sum = summary;
	NetworkSniffer::pi_data_bacnet_AL->do_int = detail;
	NetworkSniffer::pi_data_Message->do_sum = summary;
	NetworkSniffer::pi_data_Message->do_int = detail;
}

namespace NetworkSniffer {
	void ShowDecoderException( const char *pMessage );
};

//
//	BACnetPIInfo::Interpret
//
void BACnetPIInfo::Interpret( ProtocolType proto, char *header, int length )
{
	// flush the current contents (if any)
	for (int i = 0; i < detailCount; i++ )
		delete detailLine[i];

	// reset the output
	summaryLine[0] = 0;
	detailCount = 0;

	// set the context
	gCurrentInfo = this;

	if ((header == NULL) || (length == 0))
	{
		// TODO: Not sure why, but after changing log file, we get called with a
		// null packet.  A real fix would avoid GETTING the packet.
		// For now, just return, lest decoding it throw an exception.
		return;
	}

	// set the beginning of the buffers
	piBuffer = header;
	piLen = length;

	// set the PI mode to match the settings for this
	SetPIMode( doSummary, doDetail );

	// TODO: This doesn't seem to be necessary
//	if (doDetail)//only create space for timestamp, do not print any information here
//	{
//		if (gCurrentInfo->detailCount == 0)
//		{
//			// Allocate an initial entry
//			NetworkSniffer::get_int_line( NULL, 0, 0 );
//		}
//		gCurrentInfo->detailLine[0]->piLine[0] = 0;
//	}
	
	try 
	{
		// call one of the known interpreters
		switch (proto) {
			case ipProtocol:
				NetworkSniffer::interp_bacnet_IP( header, length );
				break;
			case ethernetProtocol:
				NetworkSniffer::interp_bacnet_ETHERNET( header, length );
				break;
			case arcnetProtocol:
				NetworkSniffer::interp_bacnet_ARCNET( header, length );
				break;
			case mstpProtocol:
				NetworkSniffer::interp_bacnet_MSTP( header, length );
				break;
			case ptpProtocol:
				NetworkSniffer::interp_bacnet_PTP( header, length );
				break;
			case msgProtocol:
				NetworkSniffer::interp_Message( header, length );
				break;
			case textMsgProtocol:
				NetworkSniffer::interp_TextMessage( header, length );
				break;
			default:
				throw( "unknown protocol" );
				break;
		}
	}

	// NOTE: The original code just caught (...) and did {}.
	// Since most such exceptions indicate the presence of a bug in the
	// decoder, it seems preferable to SEE the problem.
	//
	// The ideal case would be to know who is THROWING the exception.
	// One might look at SetUnhandledExceptionFilter or ThreadExceptionEventHandler
	// to see that would help figure out WHAT was caught
	catch (int ex) {
		CString str;
		str.Format( "int(%u)", ex );
		NetworkSniffer::ShowDecoderException( (LPCTSTR)str );
	}
	catch (const char *pEx) {
		NetworkSniffer::ShowDecoderException( pEx );
	}
	catch (const CString &ex) {
		NetworkSniffer::ShowDecoderException( (LPCTSTR)ex );
	}
	catch (const CObject &ex) {
		CString str;
		str.Format( "CObject (%p)", &ex );
		NetworkSniffer::ShowDecoderException( (LPCTSTR)str );
	}
	catch (const void *pEx) {
		CString str;
		str.Format( "pointer(%p)", pEx );
		NetworkSniffer::ShowDecoderException( (LPCTSTR)str );
	}
	catch (...) {
		// Null pointers and the like come here...
		NetworkSniffer::ShowDecoderException( "..." );
	}
}

//-----

//
//	Everything else, all of the other global variables and functions, belong to the 
//	NetworkSniffer namespace.
//

namespace NetworkSniffer {

//-----

struct  pi_data		gPIData;	/* global pi_data */

//-----

struct  pi_data * pif_pi;		/* Ptr to current pi */
int     pif_offset;				/* Current offset */
int     pif_end_offset;			/* Offset for last+1 byte */
int     pif_flagbit_indent;		/* Number of blanks for flagbit */
char    pif_header_msg[];       /* Saved header message */

char *  near msg_origin;        /* ptr to start of frame or message */

//-----

// Show information about an exception
void ShowDecoderException( const char *pMessage )
{
	// TODO: Add more text about how to report this error.
	// - how to copy to clipboard
	// - could we add a report-to-web feature?
	CString str;
	str.Format( "Exception during decoding: %s\n"
				"\n"
				"Near offset %d in PDU",
				pMessage, pif_offset
			  );
	for (int ix=0; ix < pif_end_offset; ix++)
	{
		char buf[ 20 ];
		if ((ix % 16) == 0)
		{
			sprintf( buf, "\n" );
			str += buf;
		}

		sprintf( buf, " %02X", msg_origin[ix] & 0xFF );
		str += buf;
	}

	::AfxMessageBox( str );
}

char *get_sum_line( struct pi_data * )
{
	// no more than one at a time
	if (gCurrentInfo->summaryLine[0])
		throw (-1);
	
	return gCurrentInfo->summaryLine;
}

// Get a new BACnetPIDetail line.  Return text buffer in which to build
char *get_int_line( struct pi_data *, int offset, int length, PID_NODE_TYPE nodeType )
{
	BACnetPIDetailPtr	dp;
	
	if (gCurrentInfo->detailCount >= gCurrentInfo->detailMax)
	{
		// Expand the array
		gCurrentInfo->detailMax += 2000;
		BACnetPIDetailPtr *pOld = gCurrentInfo->detailLine;
		gCurrentInfo->detailLine = new BACnetPIDetailPtr[ gCurrentInfo->detailMax ];

		// Copy any previous lines into the new array
		for (int ix = 0; ix < gCurrentInfo->detailCount; ix++)
		{
			gCurrentInfo->detailLine[ix] = pOld[ix];
		}

		delete[] pOld;
	}
	
	// allocate a new detail line buffer
	gCurrentInfo->detailLine[gCurrentInfo->detailCount++] = dp = new BACnetPIDetail;
	dp->piOffset = gCurrentInfo->piOffset + offset;
	dp->piLen = length;
	dp->piLine[0] = 0;
	dp->piNodeType = nodeType;
	
	// return a pointer to the character buffer
	return dp->piLine;
}

// Get a pointer to the current detail line
char *get_cur_int_line( void )
{
	if ((gCurrentInfo->detailCount == 0) || 
		(gCurrentInfo->detailLine == NULL) ||
		(gCurrentInfo->detailLine[gCurrentInfo->detailCount-1] == NULL))
	{
		throw("Bug: no current detail line");
	}

	return gCurrentInfo->detailLine[gCurrentInfo->detailCount-1]->piLine;
}

// Get the index of the next detail line
// Used to fix up the length of previous detail lines after subsequent
// items have been parsed.  Simplifies various wrapping items.
int get_next_detail_index( void )
{
	return gCurrentInfo->detailCount;
}

// Set the data length of the specified detail line to theOffset.
// Used to fix up the length of previous detail lines after subsequent
// items have been parsed.  Simplifies various wrapping items.
void set_detail_line_length( int theLine, int theOffset )
{
	if ((theLine >= 0) && (theLine < gCurrentInfo->detailCount))
	{
		gCurrentInfo->detailLine[theLine]->piLen = 
			gCurrentInfo->piOffset + theOffset - 
			gCurrentInfo->detailLine[theLine]->piOffset;
	}
}


//-----

//
//	rev_word
//
//	Returns the parameter value with the bytes reversed, used for translating from 
//	big endian to little endian and vice versa.
//

int rev_word( int arg )
{
	return	((arg & 0x00FF) << 8)
		|	((arg & 0xFF00) >> 8)
		;
}

//
//	rev_long
//
//	Returns the parameter value with the bytes reversed, used for translating from 
//	big endian to little endian and vice versa.
//

long rev_long( long arg )
{
	return	((arg & 0xFF000000) >> 24)
		|	((arg & 0x00FF0000) >> 8)
		|	((arg & 0x0000FF00) << 8)
		|	((arg & 0x000000FF) << 24)
		;
}

//-----

//
//	pif_init
//
//	Initialize a protocol interpreter control block.  The real sniffer has an 
//	elaborate registration mechanism for making sure the the interpreter gets 
//	called in the correct "mode" and there are a bunch of fields that are 
//	not implemented.
//

void pif_init( struct pi_data *pi_data_your_protocol, void *p, int len )
{
	/* set up the current pi_data */
	pif_pi = pi_data_your_protocol;
	
	/* set up the globals */
	msg_origin = (char *)p;
	pif_offset = 0;
	pif_end_offset = len;
	pif_flagbit_indent = 0;
	
	/* stash the offset from the true beginning */
	gCurrentInfo->piOffset = ((char *)p - gCurrentInfo->piBuffer);
	
	pif_pi->do_sum = gCurrentInfo->doSummary;	/* generate summary lines? */
	pif_pi->do_int = gCurrentInfo->doDetail;	/* generate interpretation (detail) lines? */
	pif_pi->do_count = false;	/* only count summary lines? */
	pif_pi->do_names = true;	/* add symbolic station names? */
	pif_pi->recursive = false;	/* is this a recursive call? */
	pif_pi->selected = true;	/* is this PI selected in the menu? */
	pif_pi->forcepi_rules = 0;	/* linked list of "Force PI" rules */
}

//
//	pif_header
//
//	Generate a header line for a block of octets that are about to be interpreted 
//	in detail.
//

void pif_header( int len, char *header_string )
{
	char	*s
	;
	
	// generate the header line in the detail
	// NOTE: these headers may be compared for in the DetailView.
	// Any change here may break that view...
	s = get_int_line( pif_pi, pif_offset, len );
	sprintf( s, "----- %s -----", header_string );
	
	// output a blank line
	pif_show_space();
}

//
//	pif_line
//
//  Get a new BACnetPIDetail line for the current offset.  Return text buffer in which to build

char *pif_line( int len )
{
	char *s = get_int_line( pif_pi, pif_offset, len );
	
	pif_offset += len;
	return s;
}

//
//	pif_get_ascii
//
//  Copy len bytes of ASCII from the specified offset
//
// TODO: Note that afer Addendum 135-2008k, this may get UTF-8 encoded strings.  
// It would be nice to display them properly, along with 8859-1 and UNICODE.
// This would involve
// - place UTF-8 in the output buffer here
// - modify the display windows to show the appropriate glyphs
//
// Ideally
// - Combine all the replications of string-showing (BACnetCharacterString, by-hand 
//   during decode, etc) into one place
// - Pass in the encoding to the display function so we know how to interpret the data
// Display
// - If ANSI and 0 to 127: show it
// - If 8859-1, show it
// - If ANSI/UTF-8 and 128 to 255: decode as UTF-8.  If UNICODE equivalents are 0 to FF, show as 8859-1
// - If UNICODE and 0 to FF, show as 8859-1
// - COULD handle MBCS and JIS, but payback is low
// - Show anything else in hex

char *pif_get_ascii( int offset, int len, char result_str[], int max_chars )
{
	unsigned char c;
	char	*src = pif_get_addr() + offset;
	char	*dst = result_str;
	bool	truncated = (len > max_chars);
	if (truncated)
		max_chars -= 3;		// room for ...
	
	// Transfer ASCII printable chars
	// TODO: we can probably assume that 8859-1 is available.
	// BUT: if we allow UTF-8 strings here, DON'T show them directly
	while (len-- && max_chars-- && ((c = (unsigned char)*src++) != 0))
	{
		*dst++ = ((c >= ' ') && (c <= 0x7F)) ? c : '.';	// ASCII / ANSI X3.4
	}

	if (truncated)
	{
		strcpy( dst, "..." );
		dst += 3;
	}
	
	// null terminate the end and return a pointer to it
	*dst = 0;
	return dst;
}

//
//	pif_show_ascii
//
//  Show len bytes of ASCII from the current offset as a new detail line, using
//  the specified format string.  Advance offset pointer.
//
// TODO: Note that afer Addendum 135-2008k, this may get UTF-8 encoded strings.  
// It would be nice to display them properly, along with 8859-1 and UNICODE.
// This would involve
// - place UTF-8 in the output buffer here
// - modify the display windows to show the appropriate glyphs
//
// Screwily, many calls to this function pass len=0 and DO NOT have anything to
// decode from the buffer.  Make a new function for those guys, so we
// can add a character set specifier parameter here.

void pif_show_ascii( int len, char *prstr )
{
	char *s, buff[ MAX_INT_LINE ];
	int spaceLeft = MAX_INT_LINE - strlen(prstr); // keeps track of how much of the buffer may be used.

	// get a detail line
	s = get_int_line( pif_pi, pif_offset, len );

	if (len <= spaceLeft)
	{
		// get the ASCII data and format it in the buffer (truncate as needed)
		pif_get_ascii( 0, len, buff, MAX_INT_LINE-1 );
		sprintf( s, prstr, buff );
	}
	else
	{
		// Buffer is FULL already.  Replace the last three characters with "...
		sprintf( s + MAX_INT_LINE - 4, "..." );
	}
	
	// update the offset
	pif_offset += len;
}

//
//	pif_append_ascii
//
//  Append ASCII to the current detail line using the specified format string
//
//  CAUTION: this function does not check for detail buffer overflow.
//  It SHOULD NOT be used for strings decoded from the wire, etc. whose
//  length may vary.  Or see pif_show_ascii for ideas on extending it.

void pif_append_ascii( const char *fmt, const char *data )
{
	char *s = get_cur_int_line();

	sprintf( s + strlen(s), fmt, data );
}

//
//	pif_show_nbytes_hex
//
//  Show byte_count bytes from the current offset as hex on a new detail line,
//  using the specified format string.  Advance offset pointer.

/* replaced with method below in order to correct buffer overruns
void pif_show_nbytes_hex( char *prstr, int byte_count )
{
	static char	hex[] = "0123456789ABCDEF"
	;
	char		*s, *dst
	,			buff[ MAX_INT_LINE ]
	;
	
	// get a detail line
	s = get_int_line( pif_pi, pif_offset, byte_count );
	
	// build a hex string
	dst = buff;
	while (byte_count--) {
		int x = pif_get_byte(0);
		*dst++ = hex[ (x >> 4) & 0x0F ];
		*dst++ = hex[ x & 0x0F ];
		pif_offset++;
	}
	*dst = 0;
	
	// format the data into the detail line
	sprintf( s, prstr, buff );
}
*/
// new method provided by Buddy Lott for bug #1606849
void pif_show_nbytes_hex( char *prstr, int byte_count )
{
	static char hex[] = "0123456789ABCDEF";
	char *s, *dst
	,	buff[ MAX_TEXT_ITEM + 4]; /* use the extra 3 bytes for a '...' when data is dropped*/
	
	int spaceleft = MAX_TEXT_ITEM - strlen(prstr); // keeps track of how much of the buffer should be used.

	// since each byte takes at least 2 characters, lets make sure that the space left is even
	spaceleft = (spaceleft%2)?spaceleft-1:spaceleft;

	// fill in the buffer '.' mark some dropped data!
	memset(buff,'.',MAX_TEXT_ITEM + 3);

	// get a detail line
	s = get_int_line( pif_pi, pif_offset, byte_count );

	// build a hex string
	dst = buff;
	while (spaceleft > 0 && byte_count--) {
		int x = pif_get_byte(0);
		*dst++ = hex[ (x >> 4) & 0x0F ];
		*dst++ = hex[ x & 0x0F ];
		spaceleft -=2;
		pif_offset++;
	}

	if ( byte_count > 0 )
	{
		*(dst+3) = 0;
		pif_offset += byte_count;
	}
	else
	{
		*dst = 0;
	}

	// format the data into the detail line
	sprintf( s, prstr, buff );
}

//
//	pif_show_nbyte_uint
//
//  Show an N-byte integer from the current offset as a new detail line, 
//  using the specified format string.  Advance offset pointer by N.
long pif_show_nbyte_uint(char *prstr, int len)
{
	unsigned long val = 0;
	for (int ix = 0; ix < len; ix++)
	{
		val = (val << 8) | pif_get_byte(ix);
	}

	// get a detail line
	sprintf( get_int_line( pif_pi, pif_offset, len ), prstr, val );	

	// update the offset
	pif_offset += len;	
	return val;
}

//
//	pif_show_long_hl
//
//  Show a 4-byte unsigned integer from the current offset as a new detail line, 
//  using the specified format string.  Advance offset pointer.
long pif_show_long_hl( char *prstr )
{
	return pif_show_nbyte_uint(prstr, 4);
}

// TODO: this isn't used anywhere
//	pif_show_slong_hl
//
//  Show a 3-byte unsigned integer from the current offset as a new detail line, 
//  using the specified format string.  Advance offset pointer.
long pif_show_slong_hl( char *prstr )
{
	return pif_show_nbyte_uint(prstr, 3);
}

//
//	pif_show_word_hl
//
//  Show a 2-byte unsigned integer from the current offset as a new detail line, 
//  using the specified format string.  Advance offset pointer.

int pif_show_word_hl( char *prstr )
{
	return (int)pif_show_nbyte_uint(prstr, 2);
}

//
//	pif_show_byte
//
//  Show a 1-byteunsigned  integer from the current offset as a new detail line, 
//  using the specified format string.  Advance offset pointer.

int pif_show_byte( char *prstr )
{
	return (int)pif_show_nbyte_uint(prstr, 1);
}

//
//	pif_show_flag
//
//  Show a byte from the current offset as a set of flags on a new detail line, 
//  followed by the string and the byte value in decimal.  Advance offset pointer.

// JLH TODO: why show a flag byte in DECIMAL?
// Isn't used anywhere
void pif_show_flag( char *prstr, int mask )
{
	char	*s
	,		buff[ MAX_INT_LINE ]
	;
	int		flag
	;
	
	// get a detail line
	s = get_int_line( pif_pi, pif_offset, 1 );
	for (int i = 0; i < pif_flagbit_indent; i++)
		*s++ = ' ';
	
	// get the byte
	flag = pif_get_byte(0);
	
	// format the mask
	*s++ = (mask & 0x80) ? ((flag & 0x80) ? '1' : '0') : '.';
	*s++ = (mask & 0x40) ? ((flag & 0x40) ? '1' : '0') : '.';
	*s++ = (mask & 0x20) ? ((flag & 0x20) ? '1' : '0') : '.';
	*s++ = (mask & 0x10) ? ((flag & 0x10) ? '1' : '0') : '.';
	*s++ = ' ';
	*s++ = (mask & 0x08) ? ((flag & 0x08) ? '1' : '0') : '.';
	*s++ = (mask & 0x04) ? ((flag & 0x04) ? '1' : '0') : '.';
	*s++ = (mask & 0x02) ? ((flag & 0x02) ? '1' : '0') : '.';
	*s++ = (mask & 0x01) ? ((flag & 0x01) ? '1' : '0') : '.';
	
	strcpy( buff, prstr );
	strcat( buff, " = %d" );
	sprintf( s, buff, flag );
	
	// update the offset
	pif_offset += 1;
}

//
//	pif_show_flagbit
//
//  Show a byte from the current-1 offset as a set of flags on a new detail line, 
//  followed by either the true-string or false-string.  No effect on offset pointer.

int pif_show_flagbit( int bit, char *truestr, char *falsestr )
{
	// get a detail line
	char *s = get_int_line( pif_pi, pif_offset-1, 1 );
	
	// get the previous byte
	int		flag = pif_get_byte(-1);
	
	// indent to line up '='
	for (int i = 0; i < pif_flagbit_indent; i++)
		*s++ = ' ';
	
	// format the mask
	*s++ = (bit & 0x80) ? ((flag & 0x80) ? '1' : '0') : '.';
	*s++ = (bit & 0x40) ? ((flag & 0x40) ? '1' : '0') : '.';
	*s++ = (bit & 0x20) ? ((flag & 0x20) ? '1' : '0') : '.';
	*s++ = (bit & 0x10) ? ((flag & 0x10) ? '1' : '0') : '.';
	*s++ = ' ';
	*s++ = (bit & 0x08) ? ((flag & 0x08) ? '1' : '0') : '.';
	*s++ = (bit & 0x04) ? ((flag & 0x04) ? '1' : '0') : '.';
	*s++ = (bit & 0x02) ? ((flag & 0x02) ? '1' : '0') : '.';
	*s++ = (bit & 0x01) ? ((flag & 0x01) ? '1' : '0') : '.';
	*s++ = ' ';
	*s++ = '=';
	*s++ = ' ';
	
	if (truestr && !falsestr)
		strcpy( s, truestr );
	else
	if ((bit & flag) != 0)
		strcpy( s, truestr );
	else
		strcpy( s, falsestr );
	
	return ((bit & flag) != 0);
}

//
//	pif_show_flagmask
//
//  Compare the masked byte from current-1 offset to the specified value.
//  If they match, show the byte as a set of flags on a new detail line, 
//  followed by the string.  Else show nothing.  No effect on offset pointer.

int pif_show_flagmask( int maskbits, int value, char *prstr )
{
	// get the previous byte
	int		flag = pif_get_byte(-1)
	;
	
	// if the masked bits don't match the value, skip it
	if ((flag & maskbits) != value)
		return 0;
	
	// get a detail line
	char *s = get_int_line( pif_pi, pif_offset-1, 1 )
	;
	
	// indent to line up '='
	for (int i = 0; i < pif_flagbit_indent; i++)
		*s++ = ' ';
	
	// format the mask
	*s++ = (maskbits & 0x80) ? ((flag & 0x80) ? '1' : '0') : '.';
	*s++ = (maskbits & 0x40) ? ((flag & 0x40) ? '1' : '0') : '.';
	*s++ = (maskbits & 0x20) ? ((flag & 0x20) ? '1' : '0') : '.';
	*s++ = (maskbits & 0x10) ? ((flag & 0x10) ? '1' : '0') : '.';
	*s++ = ' ';
	*s++ = (maskbits & 0x08) ? ((flag & 0x08) ? '1' : '0') : '.';
	*s++ = (maskbits & 0x04) ? ((flag & 0x04) ? '1' : '0') : '.';
	*s++ = (maskbits & 0x02) ? ((flag & 0x02) ? '1' : '0') : '.';
	*s++ = (maskbits & 0x01) ? ((flag & 0x01) ? '1' : '0') : '.';
	*s++ = ' ';
	*s++ = '=';
	*s++ = ' ';
	
	strcpy( s, prstr );
	
	// success
	return 1;
}

//
//	pif_show_space
//
//	Simply outputs a blank detail line. NOP for this implementation.
//

void pif_show_space( void )
{

/*	disabled for TreeList Detail View
	char	*s
	;
	
	// get a detail line, make sure it's empty
	s = get_int_line( pif_pi, pif_offset, 0 );
	*s = 0;
*/
}

//
//	xsprintf
//
//  sprintf of the byte at the current offset.
//  format string prstr should have %s for hdr, %u for the byte, and %s for valu
//  Offset pointer is advanced by one.
//
// Historical note:
//	The sprintf() function in the real Sniffer (tm) has an extension to allow the 
//	pif_offset to be updated as a side effect.  It takes the type of object 
//	to know how far to update.  In the BACnet sniffer functions, only '%>ku'
//	is used (which uses one byte, yet another extension) so that's the only 
//	thing I check for.  Oh, and it's also used only once.
// Reality check:
//  This was just too disgusting, so I edited the 20 callers to just pass
//  %u and eliminated the voodoo here.  Not like we are going back to Sniffer.
//
void xsprintf( char *dst, const char *prstr, const char *hdr, const char *valu )
{
	// now use the real sprintf
	sprintf( dst, prstr, hdr, (unsigned char)pif_get_byte(0), valu );
	
	// update passed the byte
	pif_offset += 1;
}

// Safe strcpy that won't exceed maxLength (including null terminator)
// (Note that strncpy is NOT equivalent, as it doesn't guarantee null terminated output)
void SafeCopy( char *pDest, const char *pSource, int maxLength )
{
	if (maxLength > 0)
	{
		int len = strlen( pSource );
		if (len < maxLength)
		{
			strcpy( pDest, pSource );
		}
		else
		{
			// Truncate, showing "..." as last characters in buffer
			if (maxLength >= 4)
			{
				len = maxLength - 4;
				if (len > 0)
				{
					memcpy( pDest, pSource, len );
				}

				strcpy( pDest + len, "..." );
			}
			else
			{
				// No room for ..., just ensure a null terminator
				pDest[ maxLength - 1 ] = 0;
			}
		}
	}
}

// Safe strcat that won't exceed maxLength (including null terminator)
void SafeAppend( char *pDest, const char *pSource, int maxLength )
{
	int len = strlen( pDest );
	if (len < maxLength-1)
	{
		// Append, up to what will fit
		SafeCopy( pDest + len, pSource, maxLength - len );
	}
}

// end of the namespace
}
