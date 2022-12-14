///////////////////////////////////////////////////////////////////////////////
//
// OGLView.h : class definition file
//
// Purpose:	Implementation of OpenGL Window of Particle System
//
// Created:
//		JL 11/1/97
// Revisions:
//		Integrated into Particle System Demo		2/18/98
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1998 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGLVIEW_H__2AB46761_27CD_11D1_83A0_004005308EB5__INCLUDED_)
#define AFX_OGLVIEW_H__2AB46761_27CD_11D1_83A0_004005308EB5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OGLView.h : header file
//

#include <GL/gl.h>
#include <GL/glut.h>

#include "particle.h"
/////////////////////////////////////////////////////////////////////////////
// COGLView window

class COGLView : public CWnd
{
// Construction
public:
	COGLView();

// Attributes
public:
	CStatusBar  *m_ptrStatusBar;
	HDC		m_hDC;
	HGLRC	m_hRC;
	CPoint	m_mousepos;
	float	m_Grab_Rot_X,m_Grab_Rot_Y,m_Grab_Rot_Z;
	float	m_Grab_Yaw,m_Grab_Pitch;
	float	m_Grab_Trans_X,m_Grab_Trans_Y,m_Grab_Trans_Z;
	int		m_ScreenWidth, m_ScreenHeight;
// Operations
public:
	bool	SetupPixelFormat(HDC hdc);
	GLvoid	drawScene(bool drawSelectRect);
	GLvoid	initializeGL(GLsizei width, GLsizei height);
	GLvoid	resize( GLsizei width, GLsizei height );
	void	GetGLInfo();
	void	UpdateStatus();
	void	IdleFunc();
	void	HandleKeyUp(UINT nChar);
	void	HandleKeyDown(UINT nChar);
	void	resetEmitter();
	void	editEmitter(tEmitter *emitter);
	bool	GetEmitter(const char* filename);
	bool	SaveEmitter(const char* filename);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COGLView)
	public:
	virtual bool Create(const char* lpszClassName, const char* lpszWindowName, unsigned int dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COGLView();
	bool	m_AntiAlias;
	bool	m_DrawAxis;
	tEmitter m_Emitter;

protected:
	bool	m_DrawSystem;
	tVector m_ViewRot;		// USED TO ALLOW VIEW TO ROTATE
	tEmitter *m_CurrentEmitter;
	// Generated message map functions
	//{{AFX_MSG(COGLView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OGLVIEW_H__2AB46761_27CD_11D1_83A0_004005308EB5__INCLUDED_)
