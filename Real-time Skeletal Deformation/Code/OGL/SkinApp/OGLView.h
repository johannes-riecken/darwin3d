///////////////////////////////////////////////////////////////////////////////
//
// OGLView.h : class definition file
//
// Purpose:	Implementation of OpenGL Window of Deformation System
//
// Created:
//		JL 11/1/97
// Revisions:
//		Integrated into Skinning Demo		2/18/98
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

#include "Skeleton.h"
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
	float	m_Grab_Trans_X,m_Grab_Trans_Y,m_Grab_Trans_Z;
	bool	m_DrawGeometry, m_DrawBoneSystem, m_DrawOutline;
	bool	m_Defmode_TwoBone;
	float	m_AnimBlend;
	t_Bone	*m_SelectedBone;	// THE CURRENTLY PICKED BONE
	t_Bone	m_Mesh;				// THE MESH TO DEFORM
	tColoredVertex *m_DeformedMesh;	// HOLDS THE PREDEFORMED VERTICES
	int		*m_SelectFlags;
	CRect   m_SelectRect;
	int		m_ScreenWidth, m_ScreenHeight;
// Operations
public:
	bool	SetupPixelFormat(HDC hdc);
	GLvoid	drawScene(bool drawSelectRect);
	GLvoid  drawSkeleton(t_Bone *rootBone);
	GLvoid	drawModel(t_Bone *baseBone, tColoredVertex *deformedMesh);
	GLvoid  DeformBone(t_Bone *rootBone, tColoredVertex *meshdata, tColoredVertex *defdata);
	GLvoid	DeformModel(t_Bone *rootBone);
	void	ModifyWeights();
	void	LoadObjectFile(char *filename,t_Bone *bonePtr);
	void	ResetWeights();
	bool	GetWeights(CString filename);
	bool	SaveWeights(CString filename);
	void	CreateBoneDLists(t_Bone *bone);
	GLvoid	initializeGL(GLsizei width, GLsizei height);
	GLvoid	resize( GLsizei width, GLsizei height );
	void	GetGLInfo(char *who,char *which, char *version);
	void	SelectVertices(bool select);
	void	CompareBuffer(GLint size, GLfloat *buffer,bool select);
	void	UpdateStatus();
	void	HandleKeyUp(UINT nChar);
	void	HandleKeyDown(UINT nChar);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COGLView)
	public:
	virtual bool Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, unsigned int dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, t_Bone *skeleton, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COGLView();

	// Generated message map functions
protected:
	t_Bone *m_Skeleton;
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
