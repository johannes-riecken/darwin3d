///////////////////////////////////////////////////////////////////////////////
//
// MainFrm.cpp : implementation of the CMainFrame class
//
// Purpose:	Implementation of Main Window of Quaternion Animation System
//
// Created:
//		JL 9/1/97
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1997 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Slash.h"

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
	ON_COMMAND(ID_VIEW_USEQUATERNIONS, OnViewUsequaternions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_USEQUATERNIONS, OnUpdateViewUsequaternions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GEOMETRY, OnUpdateViewGeometry)
	ON_COMMAND(ID_HELP_WHICHOPENGL, OnHelpWhichopengl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_STATUS,	// MY ADDITION FOR PUTTING SETTINGS
	ID_INDICATOR_ROT,		// MY ADDITION FOR PUTTING SETTINGS
	ID_INDICATOR_QUAT,		// MY ADDITION FOR PUTTING SETTINGS
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
	free(m_Skeleton.children);
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
// Notes:		Pretty Handy info for debugging
///////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnHelpWhichopengl()
{
	m_OGLView.GetGLInfo();
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

	m_OGLView.drawScene();
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

/////////////////////////////////////////////////////////////////////////////
// Hierarchy Manipulation Functions

void CMainFrame::InitializeSkeleton()
{
/// Local Variables ///////////////////////////////////////////////////////////
	t_Bone *tempBones;
///////////////////////////////////////////////////////////////////////////////

	// INITIALIZE SOME OF THE SKELETON VARIABLES
	ResetBone(&m_Skeleton, NULL);
	m_Skeleton.id = -1;
	strcpy(m_Skeleton.name,"Skeleton");
	m_Skeleton.b_trans.y = -4.0f;		// was -4
	m_Skeleton.b_trans.z = -100.0f;
	m_Skeleton.trans.y = 0.0f;		// -4
	m_Skeleton.trans.z = -100.0f;

	// ALLOC ROOM FOR BONES
	tempBones = (t_Bone *)malloc(2 * sizeof(t_Bone));

	m_Skeleton.childCnt = 1;
	m_Skeleton.children = tempBones;

	ResetBone(&tempBones[0], &m_Skeleton);		// SETUP INITIAL BONE SETTINGS
	ResetBone(&tempBones[1], &m_Skeleton);		// SETUP INITIAL BONE SETTINGS

	tempBones[0].childCnt = 1;
	tempBones[0].children = &tempBones[1];
	tempBones[0].trans.x = 0;
	tempBones[0].rot.x = 0.0;
	tempBones[0].rot.y = 0.0;
	tempBones[0].rot.z = 0.0;

	tempBones[1].trans.y = 4;

}


// TOGGLE THE STATUS OF THE VIEW GEOMETRY FLAG
void CMainFrame::OnViewGeometry()
{
	m_OGLView.m_DrawGeometry = !m_OGLView.m_DrawGeometry;
	m_OGLView.drawScene();
}

// SET THE CHECKED STATUS OF THE VIEW GEOMETRY MENU BASED ON STATUS
void CMainFrame::OnUpdateViewGeometry(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_OGLView.m_DrawGeometry );
}

// TOGGLE THE STATUS OF THE USE QUATERNIONS FLAG
void CMainFrame::OnViewUsequaternions()
{
	m_OGLView.m_UseQuat = !m_OGLView.m_UseQuat;
	m_OGLView.drawScene();
}

// SET THE CHECKED STATUS OF THE USE QUATERNIONS MENU BASED ON STATUS
void CMainFrame::OnUpdateViewUsequaternions(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck( m_OGLView.m_UseQuat );
}

