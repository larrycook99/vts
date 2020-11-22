
/*****************************************************/
/*                                                   */
/*      polarver.h -- Vendor info for driver builds  */
/*													 */
/* 						    						 */
/*****************************************************/




/*---------------------------------------------------*/
/* the following section defines values used in the  */
/* version data structure for all files, and which   */
/* do not change.                                    */
/*---------------------------------------------------*/

#define VER_COMPANYNAME_STR		"PolarSoft Inc."

#define VER_LEGALTRADEMARKS_STR	\
"PolarSoft\256 is a registered trademark of PolarSoft Inc."

#define VER_LEGALCOPYRIGHT_YEARS    "1998-1999"

#define VER_LEGALCOPYRIGHT_STR		\
	"Copyright \251 PolarSoft Inc. " 	\
	VER_LEGALCOPYRIGHT_YEARS


/* default is nodebug */
#if DBG
#define VER_DEBUG				VS_FF_DEBUG
#else
#define VER_DEBUG				0
#endif

/* default is release */
#if BETA
#define VER_PRERELEASE			VS_FF_PRERELEASE
#else
#define VER_PRERELEASE			0
#endif

#define VER_FILEFLAGSMASK  VS_FFI_FILEFLAGSMASK
#define VER_FILEOS		   VOS_NT_WINDOWS32
#define VER_FILEFLAGS	   (VER_PRERELEASE | VER_DEBUG)


