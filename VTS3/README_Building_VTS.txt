Building VTS from source

Microsoft seems to totally redesign their solution/workspace/project build system
every few years.  And VTS has been around through a bunch of them.

The source tree includes several vintages of project files.  If none match your
version of Visual Studio, try to open one of the earlier vintage, and see if
Visual Studio will convert it for you.

vts.dsw and the *.dsp are for Visual Studio 6
This is a really ancient version, but it is what was used to generate the
pre-built releases up until v3.5.5, and we are a sentimental bunch.
   Verified to build under XP SP3 on 1 March 2014 with three warnings:
   C:\VTS3_new\VTSPacketDB.cpp(167) : warning C4244: 'argument' : conversion from 'unsigned __int64' to 'long', possible loss of data
   C:\VTS3_new\VTSPacketDB.cpp(218) : warning C4244: 'argument' : conversion from 'unsigned __int64' to 'long', possible loss of data
   C:\VTS3_new\VTSPacketDB.cpp(260) : warning C4244: 'argument' : conversion from 'unsigned __int64' to 'unsigned long', possible loss of data

vts_2003.sln and the *.vcproj are for Visual Studio 2003
   I no longer have tools to try this

vts.sln and the *.vcxproj at for Visual Studio 2010
   Verified to build 1 March 2014 with one warning:
   VTS.cpp(220): warning C4996: 'CWinApp::Enable3dControlsStatic': CWinApp::Enable3dControlsStatic is no longer needed. You should remove this call.
   c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\atlmfc\include\afxwin.h(5347) : see declaration of 'CWinApp::Enable3dControlsStatic'

To put together a release, READ the file release.bat, which tells how but
no longer does it.
