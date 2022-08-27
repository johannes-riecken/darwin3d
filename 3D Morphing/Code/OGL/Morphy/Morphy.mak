# Microsoft Developer Studio Generated NMAKE File, Based on Morphy.dsp
!IF "$(CFG)" == ""
CFG=Morphy - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Morphy - Win32 Debug.
!ENDIF

!IF "$(CFG)" != "Morphy - Win32 Release" && "$(CFG)" != "Morphy - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "Morphy.mak" CFG="Morphy - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Morphy - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Morphy - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE
!ERROR An invalid configuration is specified.
!ENDIF

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

!IF  "$(CFG)" == "Morphy - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Morphy.exe"


CLEAN :
	-@erase "$(INTDIR)\Bitmap.obj"
	-@erase "$(INTDIR)\LoadDlg.obj"
	-@erase "$(INTDIR)\LoadOBJ.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\Morphy.obj"
	-@erase "$(INTDIR)\Morphy.pch"
	-@erase "$(INTDIR)\Morphy.res"
	-@erase "$(INTDIR)\OGLView.obj"
	-@erase "$(INTDIR)\Skeleton.obj"
	-@erase "$(INTDIR)\Slider.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Morphy.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Morphy.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Morphy.res" /d "NDEBUG"
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Morphy.bsc"
BSC32_SBRS= \

LINK32=link.exe
LINK32_FLAGS=dsound.lib winmm.lib opengl32.lib glu32.lib glaux.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Morphy.pdb" /machine:I386 /out:"$(OUTDIR)\Morphy.exe"
LINK32_OBJS= \
	"$(INTDIR)\Bitmap.obj" \
	"$(INTDIR)\LoadDlg.obj" \
	"$(INTDIR)\LoadOBJ.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\Morphy.obj" \
	"$(INTDIR)\OGLView.obj" \
	"$(INTDIR)\Skeleton.obj" \
	"$(INTDIR)\Slider.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Morphy.res"

"$(OUTDIR)\Morphy.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Morphy - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Morphy.exe" "$(OUTDIR)\Morphy.bsc"


CLEAN :
	-@erase "$(INTDIR)\Bitmap.obj"
	-@erase "$(INTDIR)\Bitmap.sbr"
	-@erase "$(INTDIR)\LoadDlg.obj"
	-@erase "$(INTDIR)\LoadDlg.sbr"
	-@erase "$(INTDIR)\LoadOBJ.obj"
	-@erase "$(INTDIR)\LoadOBJ.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\Morphy.obj"
	-@erase "$(INTDIR)\Morphy.pch"
	-@erase "$(INTDIR)\Morphy.res"
	-@erase "$(INTDIR)\Morphy.sbr"
	-@erase "$(INTDIR)\OGLView.obj"
	-@erase "$(INTDIR)\OGLView.sbr"
	-@erase "$(INTDIR)\Skeleton.obj"
	-@erase "$(INTDIR)\Skeleton.sbr"
	-@erase "$(INTDIR)\Slider.obj"
	-@erase "$(INTDIR)\Slider.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Morphy.bsc"
	-@erase "$(OUTDIR)\Morphy.exe"
	-@erase "$(OUTDIR)\Morphy.ilk"
	-@erase "$(OUTDIR)\Morphy.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Morphy.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $<
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Morphy.res" /d "_DEBUG"
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Morphy.bsc"
BSC32_SBRS= \
	"$(INTDIR)\Bitmap.sbr" \
	"$(INTDIR)\LoadDlg.sbr" \
	"$(INTDIR)\LoadOBJ.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\Morphy.sbr" \
	"$(INTDIR)\OGLView.sbr" \
	"$(INTDIR)\Skeleton.sbr" \
	"$(INTDIR)\Slider.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Morphy.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=dsound.lib winmm.lib opengl32.lib glu32.lib glaux.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Morphy.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Morphy.exe" /pdbtype:sept
LINK32_OBJS= \
	"$(INTDIR)\Bitmap.obj" \
	"$(INTDIR)\LoadDlg.obj" \
	"$(INTDIR)\LoadOBJ.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\Morphy.obj" \
	"$(INTDIR)\OGLView.obj" \
	"$(INTDIR)\Skeleton.obj" \
	"$(INTDIR)\Slider.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Morphy.res"

"$(OUTDIR)\Morphy.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Morphy.dep")
!INCLUDE "Morphy.dep"
!ELSE
!MESSAGE Warning: cannot find "Morphy.dep"
!ENDIF
!ENDIF


!IF "$(CFG)" == "Morphy - Win32 Release" || "$(CFG)" == "Morphy - Win32 Debug"
SOURCE=.\Bitmap.cpp

!IF  "$(CFG)" == "Morphy - Win32 Release"


"$(INTDIR)\Bitmap.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ELSEIF  "$(CFG)" == "Morphy - Win32 Debug"


"$(INTDIR)\Bitmap.obj"	"$(INTDIR)\Bitmap.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ENDIF

SOURCE=.\LoadDlg.cpp

!IF  "$(CFG)" == "Morphy - Win32 Release"


"$(INTDIR)\LoadDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ELSEIF  "$(CFG)" == "Morphy - Win32 Debug"


"$(INTDIR)\LoadDlg.obj"	"$(INTDIR)\LoadDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ENDIF

SOURCE=.\LoadOBJ.cpp

!IF  "$(CFG)" == "Morphy - Win32 Release"


"$(INTDIR)\LoadOBJ.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ELSEIF  "$(CFG)" == "Morphy - Win32 Debug"


"$(INTDIR)\LoadOBJ.obj"	"$(INTDIR)\LoadOBJ.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ENDIF

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "Morphy - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ELSEIF  "$(CFG)" == "Morphy - Win32 Debug"


"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ENDIF

SOURCE=.\Morphy.cpp

!IF  "$(CFG)" == "Morphy - Win32 Release"


"$(INTDIR)\Morphy.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ELSEIF  "$(CFG)" == "Morphy - Win32 Debug"


"$(INTDIR)\Morphy.obj"	"$(INTDIR)\Morphy.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ENDIF

SOURCE=.\Morphy.rc

"$(INTDIR)\Morphy.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\OGLView.cpp

!IF  "$(CFG)" == "Morphy - Win32 Release"


"$(INTDIR)\OGLView.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ELSEIF  "$(CFG)" == "Morphy - Win32 Debug"


"$(INTDIR)\OGLView.obj"	"$(INTDIR)\OGLView.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ENDIF

SOURCE=.\Skeleton.cpp

!IF  "$(CFG)" == "Morphy - Win32 Release"


"$(INTDIR)\Skeleton.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ELSEIF  "$(CFG)" == "Morphy - Win32 Debug"


"$(INTDIR)\Skeleton.obj"	"$(INTDIR)\Skeleton.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ENDIF

SOURCE=.\Slider.cpp

!IF  "$(CFG)" == "Morphy - Win32 Release"


"$(INTDIR)\Slider.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ELSEIF  "$(CFG)" == "Morphy - Win32 Debug"


"$(INTDIR)\Slider.obj"	"$(INTDIR)\Slider.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Morphy.pch"


!ENDIF

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Morphy - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\Morphy.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Morphy.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Morphy - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Morphy.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Morphy.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF


!ENDIF

