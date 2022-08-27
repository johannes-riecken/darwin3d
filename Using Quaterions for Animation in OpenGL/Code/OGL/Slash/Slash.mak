# Microsoft Developer Studio Generated NMAKE File, Based on Slash.dsp
!IF "$(CFG)" == ""
CFG=Slash - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Slash - Win32 Debug.
!ENDIF

!IF "$(CFG)" != "Slash - Win32 Release" && "$(CFG)" != "Slash - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "Slash.mak" CFG="Slash - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Slash - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Slash - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE
!ERROR An invalid configuration is specified.
!ENDIF

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Slash - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0"

ALL : "$(OUTDIR)\Slash.exe"

!ELSE

ALL : "$(OUTDIR)\Slash.exe"

!ENDIF

CLEAN :
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\OGLView.obj"
	-@erase "$(INTDIR)\Quatern.obj"
	-@erase "$(INTDIR)\SetRot.obj"
	-@erase "$(INTDIR)\Skeleton.obj"
	-@erase "$(INTDIR)\Slash.obj"
	-@erase "$(INTDIR)\Slash.pch"
	-@erase "$(INTDIR)\Slash.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\Slash.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Slash.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c\

CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Slash.res" /d "NDEBUG"
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Slash.bsc"
BSC32_SBRS= \

LINK32=link.exe
LINK32_FLAGS=opengl32.lib glu32.lib glaux.lib /nologo /subsystem:windows\
 /incremental:no /pdb:"$(OUTDIR)\Slash.pdb" /machine:I386\
 /out:"$(OUTDIR)\Slash.exe"
LINK32_OBJS= \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\OGLView.obj" \
	"$(INTDIR)\Quatern.obj" \
	"$(INTDIR)\SetRot.obj" \
	"$(INTDIR)\Skeleton.obj" \
	"$(INTDIR)\Slash.obj" \
	"$(INTDIR)\Slash.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Slash.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Slash - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0"

ALL : "$(OUTDIR)\Slash.exe"

!ELSE

ALL : "$(OUTDIR)\Slash.exe"

!ENDIF

CLEAN :
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\OGLView.obj"
	-@erase "$(INTDIR)\Quatern.obj"
	-@erase "$(INTDIR)\SetRot.obj"
	-@erase "$(INTDIR)\Skeleton.obj"
	-@erase "$(INTDIR)\Slash.obj"
	-@erase "$(INTDIR)\Slash.pch"
	-@erase "$(INTDIR)\Slash.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\Slash.exe"
	-@erase "$(OUTDIR)\Slash.ilk"
	-@erase "$(OUTDIR)\Slash.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Slash.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c\

CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Slash.res" /d "_DEBUG"
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Slash.bsc"
BSC32_SBRS= \

LINK32=link.exe
LINK32_FLAGS=opengl32.lib glu32.lib glaux.lib /nologo /subsystem:windows\
 /incremental:yes /pdb:"$(OUTDIR)\Slash.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\Slash.exe" /pdbtype:sept
LINK32_OBJS= \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\OGLView.obj" \
	"$(INTDIR)\Quatern.obj" \
	"$(INTDIR)\SetRot.obj" \
	"$(INTDIR)\Skeleton.obj" \
	"$(INTDIR)\Slash.obj" \
	"$(INTDIR)\Slash.res" \
	"$(INTDIR)\StdAfx.obj"

"$(OUTDIR)\Slash.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<


!IF "$(CFG)" == "Slash - Win32 Release" || "$(CFG)" == "Slash - Win32 Debug"
SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "Slash - Win32 Release"

DEP_CPP_MAINF=\
	".\MainFrm.h"\
	".\OGLView.h"\
	".\Quatern.h"\
	".\Skeleton.h"\
	".\Slash.h"\


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ELSEIF  "$(CFG)" == "Slash - Win32 Debug"

DEP_CPP_MAINF=\
	".\MainFrm.h"\
	".\OGLView.h"\
	".\Quatern.h"\
	".\Skeleton.h"\
	".\Slash.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"GL\gl.h"\
	{$(INCLUDE)}"GL\glu.h"\


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ENDIF

SOURCE=.\OGLView.cpp

!IF  "$(CFG)" == "Slash - Win32 Release"

DEP_CPP_OGLVI=\
	".\Model.h"\
	".\OGLView.h"\
	".\Quatern.h"\
	".\SetRot.h"\
	".\Skeleton.h"\
	".\Slash.h"\


"$(INTDIR)\OGLView.obj" : $(SOURCE) $(DEP_CPP_OGLVI) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ELSEIF  "$(CFG)" == "Slash - Win32 Debug"

DEP_CPP_OGLVI=\
	".\Model.h"\
	".\OGLView.h"\
	".\Quatern.h"\
	".\SetRot.h"\
	".\Skeleton.h"\
	".\Slash.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"GL\gl.h"\
	{$(INCLUDE)}"GL\glu.h"\


"$(INTDIR)\OGLView.obj" : $(SOURCE) $(DEP_CPP_OGLVI) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ENDIF

SOURCE=.\Quatern.cpp

!IF  "$(CFG)" == "Slash - Win32 Release"

DEP_CPP_QUATE=\
	".\Quatern.h"\
	".\Skeleton.h"\


"$(INTDIR)\Quatern.obj" : $(SOURCE) $(DEP_CPP_QUATE) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ELSEIF  "$(CFG)" == "Slash - Win32 Debug"

DEP_CPP_QUATE=\
	".\Quatern.h"\
	".\Skeleton.h"\
	".\StdAfx.h"\


"$(INTDIR)\Quatern.obj" : $(SOURCE) $(DEP_CPP_QUATE) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ENDIF

SOURCE=.\SetRot.cpp

!IF  "$(CFG)" == "Slash - Win32 Release"

DEP_CPP_SETRO=\
	".\SetRot.h"\
	".\Slash.h"\


"$(INTDIR)\SetRot.obj" : $(SOURCE) $(DEP_CPP_SETRO) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ELSEIF  "$(CFG)" == "Slash - Win32 Debug"

DEP_CPP_SETRO=\
	".\SetRot.h"\
	".\Slash.h"\
	".\StdAfx.h"\


"$(INTDIR)\SetRot.obj" : $(SOURCE) $(DEP_CPP_SETRO) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ENDIF

SOURCE=.\Skeleton.cpp

!IF  "$(CFG)" == "Slash - Win32 Release"

DEP_CPP_SKELE=\
	".\Quatern.h"\
	".\Skeleton.h"\


"$(INTDIR)\Skeleton.obj" : $(SOURCE) $(DEP_CPP_SKELE) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ELSEIF  "$(CFG)" == "Slash - Win32 Debug"

DEP_CPP_SKELE=\
	".\Quatern.h"\
	".\Skeleton.h"\
	".\StdAfx.h"\


"$(INTDIR)\Skeleton.obj" : $(SOURCE) $(DEP_CPP_SKELE) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ENDIF

SOURCE=.\Slash.cpp

!IF  "$(CFG)" == "Slash - Win32 Release"

DEP_CPP_SLASH=\
	".\MainFrm.h"\
	".\OGLView.h"\
	".\Quatern.h"\
	".\Skeleton.h"\
	".\Slash.h"\


"$(INTDIR)\Slash.obj" : $(SOURCE) $(DEP_CPP_SLASH) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ELSEIF  "$(CFG)" == "Slash - Win32 Debug"

DEP_CPP_SLASH=\
	".\MainFrm.h"\
	".\OGLView.h"\
	".\Quatern.h"\
	".\Skeleton.h"\
	".\Slash.h"\
	".\StdAfx.h"\
	{$(INCLUDE)}"GL\gl.h"\
	{$(INCLUDE)}"GL\glu.h"\


"$(INTDIR)\Slash.obj" : $(SOURCE) $(DEP_CPP_SLASH) "$(INTDIR)"\
 "$(INTDIR)\Slash.pch"


!ENDIF

SOURCE=.\Slash.rc
DEP_RSC_SLASH_=\
	".\res\Slash.ico"\
	".\res\Slash.rc2"\
	".\res\SlashDoc.ico"\


"$(INTDIR)\Slash.res" : $(SOURCE) $(DEP_RSC_SLASH_) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\


!IF  "$(CFG)" == "Slash - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\Slash.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c\


"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Slash.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Slash - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)\Slash.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Slash.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF


!ENDIF

