///////////////////////////////////////////////////////////////////////////////
//
// MainFrm.cpp : implementation of the CMainFrame class
//
// Purpose:	Implementation of Main Window of Deformation System
//
// Created:
//		JL 2/18/98
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1998 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Direct.h>
#include "SkinApp.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_VIEW_GEOMETRY, OnViewGeometry)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GEOMETRY, OnUpdateViewGeometry)
	ON_COMMAND(ID_VIEW_OUTLINE, OnViewOutline)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTLINE, OnUpdateViewOutline)
	ON_COMMAND(ID_HELP_WHICHOPENGL, OnHelpWhichopengl)
	ON_COMMAND(ID_VIEW_BONESYSTEM, OnViewBonesystem)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BONESYSTEM, OnUpdateViewBonesystem)
	ON_COMMAND(ID_CONTROL_LOWERARM, OnControlLowerarm)
	ON_UPDATE_COMMAND_UI(ID_CONTROL_LOWERARM, OnUpdateControlLowerarm)
	ON_COMMAND(ID_CONTROL_UPPERARM, OnControlUpperarm)
	ON_UPDATE_COMMAND_UI(ID_CONTROL_UPPERARM, OnUpdateControlUpperarm)
	ON_COMMAND(ID_MODIFYWEIGHTS, OnModifyweights)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_STATUS,	// MY ADDITION FOR PUTTING SETTINGS
	ID_INDICATOR_ROT,		// MY ADDITION FOR PUTTING SETTINGS
	ID_INDICATOR_ROTB,		// MY ADDITION FOR PUTTING SETTINGS
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_HArrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);

	InitializeSkeleton();
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
/// Local Variables ///////////////////////////////////////////////////////////
	RECT rect;
///////////////////////////////////////////////////////////////////////////////
	GetClientRect(&rect);

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_OGLView.m_ptrStatusBar = &m_wndStatusBar;
	m_OGLView.Create(NULL,"Render Window",WS_CHILD | WS_VISIBLE, CRect(1, 1,rect.right - 3,rect.bottom),this,104,&m_Skeleton); // - 60 bottom
	m_OGLView.ShowWindow(TRUE);

	m_OGLView.Invalidate(TRUE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
/// Local Variables ///////////////////////////////////////////////////////////
	HICON hicon;
///////////////////////////////////////////////////////////////////////////////

	hicon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_ClassName = AfxRegisterWndClass(NULL,
		(HCURSOR)m_HArrow, (HBRUSH)::GetStockObject(DKGRAY_BRUSH), hicon); //m_HArrow
	cs.lpszClass = m_ClassName;

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

///////////////////////////////////////////////////////////////////////////////
// Procedure:	OnHelpWhichopengl
// Purpose:		Create dialog to Show which version of OGL is running
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnHelpWhichopengl()
{
/// Local Variables ///////////////////////////////////////////////////////////
	char message[80];			// PLACE TO PUT THE MESSAGE
	char who[80],which[80],version[80];	// OPENGL STUFF
///////////////////////////////////////////////////////////////////////////////
	m_OGLView.GetGLInfo(who,which,version);
	sprintf(message,"Who:\t%s\nWhich:\t%s\nVersion:\t%s",who,which,version);
	MessageBox(message,"Which OpenGL Renderer?",MB_OK);
}
// OnWhichogl

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	m_OGLView.drawScene(FALSE);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	m_OGLView.SetWindowPos( &wndTopMost, 1, 1, cx - 3, cy - 20 , SWP_NOZORDER ); // -60 bottom
	CFrameWnd::OnSize(nType, cx, cy);
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_OGLView.HandleKeyDown(nChar);
	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
/// Local Variables ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

	switch (nChar)
	{
		case 'Q':
			break;
	}
	m_OGLView.HandleKeyUp(nChar);
	CFrameWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnControlLowerarm()
{
	m_OGLView.m_SelectedBone = &m_Skeleton.children[1];
	m_OGLView.drawScene(FALSE);
}

void CMainFrame::OnUpdateControlLowerarm(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_OGLView.m_SelectedBone == &m_Skeleton.children[1]);
}

void CMainFrame::OnControlUpperarm()
{
	m_OGLView.m_SelectedBone = &m_Skeleton.children[0];
	m_OGLView.drawScene(FALSE);
}

void CMainFrame::OnUpdateControlUpperarm(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_OGLView.m_SelectedBone == &m_Skeleton.children[0]);
}

// TOGGLE THE STATUS OF THE VIEW GEOMETRY FLAG
void CMainFrame::OnViewGeometry()
{
	m_OGLView.m_DrawGeometry = !m_OGLView.m_DrawGeometry;
	m_OGLView.drawScene(FALSE);
}

// SET THE CHECKED STATUS OF THE VIEW GEOMETRY MENU BASED ON STATUS
void CMainFrame::OnUpdateViewGeometry(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_OGLView.m_DrawGeometry );
}

// TOGGLE THE STATUS OF THE VIEW BONESYSTEM FLAG
void CMainFrame::OnViewBonesystem()
{
	m_OGLView.m_DrawBoneSystem = !m_OGLView.m_DrawBoneSystem;
	m_OGLView.drawScene(FALSE);
}

// SET THE CHECKED STATUS OF THE VIEW BONESYSTEM MENU BASED ON STATUS
void CMainFrame::OnUpdateViewBonesystem(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_OGLView.m_DrawBoneSystem );
}

void CMainFrame::OnViewOutline()
{
	m_OGLView.m_DrawOutline = !m_OGLView.m_DrawOutline;
	if (m_OGLView.m_DrawOutline)
		glPolygonMode(GL_FRONT,GL_LINE);
	else
		glPolygonMode(GL_FRONT,GL_FILL);
	m_OGLView.drawScene(FALSE);
}

void CMainFrame::OnUpdateViewOutline(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_OGLView.m_DrawOutline );
}


void CMainFrame::OnModifyweights()
{
	m_OGLView.ModifyWeights();
}

/////////////////////////////////////////////////////////////////////////////
// Hierarchy Manipulation Functions

void CMainFrame::InitializeSkeleton()
{
/// Local Variables ///////////////////////////////////////////////////////////
	t_Bone *tempBones;
///////////////////////////////////////////////////////////////////////////////

	// INITIALIZE SOME OF THE SKELETON VARIABLES
	// I AM HARD CODING THE ARM BONE SYSTEM FOR NOW
	// SO I DON'T NEED TO DEAL WITH FILE LOADING
	ResetBone(&m_Skeleton, NULL);
	m_Skeleton.id = -1;
	strcpy(m_Skeleton.name,"Skeleton");
	m_Skeleton.p_trans.x = 2.0f;
	m_Skeleton.p_trans.y = 5.0f;
	m_Skeleton.p_trans.z = -20.0f;
	m_Skeleton.trans.x = 2.0f;
	m_Skeleton.trans.y = 5.0f;
	m_Skeleton.trans.z = -20.0f;

	// ALLOC ROOM FOR BONES
	tempBones = (t_Bone *)malloc(3 * sizeof(t_Bone));

	m_Skeleton.childCnt = 1;
	m_Skeleton.children = tempBones;

	ResetBone(&tempBones[0], &m_Skeleton);		// SETUP INITIAL BONE SETTINGS
	ResetBone(&tempBones[1], &m_Skeleton);		// SETUP INITIAL BONE SETTINGS
	ResetBone(&tempBones[2], &m_Skeleton);		// SETUP INITIAL BONE SETTINGS

	// THIS IS THE BONE FOR THE UPPER ARM
	strcpy(tempBones[0].name,"UpperArm");
	tempBones[0].id = 100;			// GIVE IT A UNIQUE ID
	tempBones[0].childCnt = 1;
	tempBones[0].children = &tempBones[1];
	tempBones[0].trans.x = 0.0;
	tempBones[0].trans.y = 0.0;
	tempBones[0].rot.x = 0.0;
	tempBones[0].rot.y = -90.0;
	tempBones[0].rot.z = 0.0;

	// THIS IS THE BONE FOR THE LOWER ARM
	strcpy(tempBones[1].name,"LowerArm");
	tempBones[1].id = 101;			// GIVE IT A UNIQUE ID
	tempBones[1].trans.y = -4.0;
	tempBones[1].childCnt = 1;
	tempBones[1].children = &tempBones[2];

	// THIS IS THE BONE FOR THE UPPER ARM
	strcpy(tempBones[2].name,"Hand");
	tempBones[2].id = 102;			// GIVE IT A UNIQUE ID
	tempBones[2].trans.y = -5.0;
	tempBones[2].childCnt = 0;

}


///////////////////////////////////////////////////////////////////////////////
// Function:	OnFileNew
// Purpose:		Clear the weight settings for the model
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileNew()
{
	m_OGLView.ResetWeights();
}

///////////////////////////////////////////////////////////////////////////////
// Function:	OnFileOpen
// Purpose:		Load the weight settings for the model
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileOpen()
{
/// Local Variables ///////////////////////////////////////////////////////////
	char BASED_CODE szFilter[] = "Weight Files (*.WGT)|*.WGT|All Files (*.*)|*.*||";
	char directory[80];
	CFileDialog	*dialog;
///////////////////////////////////////////////////////////////////////////////
	// HAD TO ADD DIRECTORY STUFF SINCE DIALOG CHANGES DIRECTORY
	_getcwd(directory,80);
	dialog = new CFileDialog(TRUE,"WGT",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if (dialog->DoModal() == IDOK)
	{
		if (!m_OGLView.GetWeights(dialog->GetPathName()))
		{
			MessageBox("Unable to load Weight File","Error",MB_OK);
		}
	}
	// RESET THE MAIN DIRECTORY
	_chdir(directory);
}

///////////////////////////////////////////////////////////////////////////////
// Function:	OnFileSave
// Purpose:		Save the weight settings for the model
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnFileSave()
{
/// Local Variables ///////////////////////////////////////////////////////////
	char BASED_CODE szFilter[] = "Weight Files (*.WGT)|*.WGT|All Files (*.*)|*.*||";
	char directory[80];
	CFileDialog	*dialog;
///////////////////////////////////////////////////////////////////////////////
	// HAD TO ADD DIRECTORY STUFF SINCE DIALOG CHANGES DIRECTORY
	_getcwd(directory,80);
	dialog = new CFileDialog(FALSE,"WGT",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if (dialog->DoModal() == IDOK)
	{
		if (!m_OGLView.SaveWeights(dialog->GetPathName()))
		{
			MessageBox("Unable to Save Weight File","Error",MB_OK);
		}
	}
	// RESET THE MAIN DIRECTORY
	_chdir(directory);
}
