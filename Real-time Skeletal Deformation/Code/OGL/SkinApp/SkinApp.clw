; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "skinapp.h"
LastPage=0

ClassCount=6
Class1=CMainFrame
Class2=COGLView
Class3=CSetRot
Class4=CSkinApp
Class5=CAboutDlg

ResourceCount=4
Resource1=IDD_SETROTATE
Resource2=IDD_ABOUTBOX
Resource3=IDD_SET_WEIGHTS
Class6=CWeight
Resource4=IDR_MAINFRAME

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
VirtualFilter=fWC
LastObject=ID_FILE_SAVE

[CLS:COGLView]
Type=0
BaseClass=CWnd
HeaderFile=OGLView.h
ImplementationFile=OGLView.cpp
Filter=W
VirtualFilter=WC
LastObject=COGLView

[CLS:CSetRot]
Type=0
BaseClass=CDialog
HeaderFile=SetRot.h
ImplementationFile=SetRot.cpp

[CLS:CSkinApp]
Type=0
BaseClass=CWinApp
HeaderFile=SkinApp.h
ImplementationFile=SkinApp.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=SkinApp.cpp
ImplementationFile=SkinApp.cpp
LastObject=ID_VIEW_BONESYSTEM

[DLG:IDD_SETROTATE]
Type=1
Class=CSetRot
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_XAXIS,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_YAXIS,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_ZAXIS,edit,1350631552

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=9
Control1=IDC_STATIC,static,1342308480
Control2=IDC_STATIC,static,1342308352
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_APP_EXIT
Command5=ID_CONTROL_UPPERARM
Command6=ID_CONTROL_LOWERARM
Command7=ID_MODIFYWEIGHTS
Command8=ID_VIEW_GEOMETRY
Command9=ID_VIEW_OUTLINE
Command10=ID_VIEW_BONESYSTEM
Command11=ID_APP_ABOUT
Command12=ID_HELP_WHICHOPENGL
CommandCount=12

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_MODIFYWEIGHTS
Command3=ID_FILE_OPEN
Command4=ID_FILE_NEW
Command5=ID_FILE_SAVE
Command6=ID_EDIT_PASTE
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_NEXT_PANE
Command10=ID_PREV_PANE
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_EDIT_CUT
Command14=ID_EDIT_UNDO
CommandCount=14

[DLG:IDD_SET_WEIGHTS]
Type=1
Class=CWeight
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SLIDER1,msctls_trackbar32,1342242840
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352

[CLS:CWeight]
Type=0
HeaderFile=Weight.h
ImplementationFile=Weight.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CWeight

