# Microsoft Visual C++ generated build script - Do not modify

PROJ = NALSERV
DEBUG = 1
PROGTYPE = 1
CALLER = p:\bacnet\vts\vts.exe
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = P:\BACNET\NALSERV\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = NALSERV.C   
FIRSTCPP =             
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /Zp1 /W3 /Zi /ALw /Od /D "_DEBUG" /Fc /FR /GD /Fd"NALSERV.PDB"
CFLAGS_R_WDLL = /nologo /W3 /ALw /O1 /D "NDEBUG" /FR /GD 
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /MAP:FULL
LIBS_D_WDLL = oldnames libw ldllcew macapi bacprim 
LIBS_R_WDLL = oldnames libw ldllcew commdlg.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = NALSERV.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL)
LFLAGS = $(LFLAGS_D_WDLL)
LIBS = $(LIBS_D_WDLL)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WDLL)
LFLAGS = $(LFLAGS_R_WDLL)
LIBS = $(LIBS_R_WDLL)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = NALSERV.SBR


NALSERV_DEP = p:\bacnet\macapi\macapi.h \
	p:\bacnet\nalserv\nalsapi.h \
	p:\bacnet\nalserv\error.h \
	p:\bacnet\nalserv\service.h \
	p:\bacnet\nalserv\db.h \
	p:\bacnet\nalserv\stdobj.h \
	p:\bacnet\nalserv\bacprim.h


NALSERV_RCDEP = p:\bacnet\nalserv\nalserv.ico \
	p:\bacnet\nalserv\redled.bmp \
	p:\bacnet\nalserv\greenled.bmp


all:	$(PROJ).DLL $(PROJ).BSC

NALSERV.OBJ:	NALSERV.C $(NALSERV_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c NALSERV.C

NALSERV.RES:	NALSERV.RC $(NALSERV_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r NALSERV.RC


$(PROJ).DLL::	NALSERV.RES

$(PROJ).DLL::	NALSERV.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
NALSERV.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) NALSERV.RES $@
	@copy $(PROJ).CRF MSVC.BND
	implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	NALSERV.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) NALSERV.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
