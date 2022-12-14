///////////////////////////////////////////////////////////////////////////////
//
// OGLView.h : class definition file
//
// Purpose:	Implementation of OpenGL Window of Hierarchical Animation System
//
// Created:
//		JL 9/12/99
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1999 Jeff Lander, All Rights Reserved.
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
	HDC m_hDC;
	HGLRC m_hRC;
	CPoint m_mousepos;
	float m_Grab_Rot_X,m_Grab_Rot_Y,m_Grab_Rot_Z;
	float m_Grab_Trans_X,m_Grab_Trans_Y,m_Grab_Trans_Z;
	CStatusBar  *m_StatusBar;
	CRect   m_SelectRect;
	int		m_ScreenWidth, m_ScreenHeight;
	bool	m_DrawSkeleton;
	bool	m_DrawDeformed;
	int		m_PickX, m_PickY;
	int		m_BoneCnt;
	t_Bone		m_Camera,*m_SelectedBone;	// For the Camera and Pointer to Current Bone
	t_Bone		m_Skeleton;					// Storage for the Skeletal system
	t_Visual	m_Model;					// Actual Model to be deformed
// Operations
public:
	bool	SetupPixelFormat(HDC hdc);
	void	LoadActor(const char* filename);
	GLvoid	drawModel(t_Visual *model);
	GLvoid  drawSkeleton(t_Bone *rootBone);
	GLvoid	drawScene(bool drawSelectRect);
	GLvoid	initializeGL(GLsizei width, GLsizei height);
	GLvoid	resize( GLsizei width, GLsizei height );
	void	CompareBuffer(GLint size, GLfloat *buffer,bool select);
	void	SelectVertices(bool select);
	void	GetGLInfo();
	void	HandleKeyUp(UINT nChar);
	void	HandleKeyDown(UINT nChar);
	void	OnViewResetskeleton();
	void	SetSkeletonList(t_Bone *skeleton);
	GLvoid	GetSkeletonMat(t_Bone *rootBone);
	GLvoid  GetBaseSkeletonMat(t_Bone *rootBone);
	GLvoid	WeightBones();
	GLvoid	DeformVertices(t_Bone *rootBone,t_Visual *visual);
	void	IterateBoneWeights(t_Bone *skeleton, bool read, FILE *fp) ;
	void	ClearBoneWeights(t_Bone *skeleton);
	bool	LoadWeights(const char* name);
	bool	SaveWeights(const char* name);
	bool	LoadOBJModel(const char* name);
	bool	LoadSkeletonFile(const char* name);
	void	SetBasePose();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COGLView)
	public:
	virtual bool Create(const char* lpszClassName, const char* lpszWindowName, unsigned int dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COGLView();

	// Generated message map functions
protected:
	void UpdateStatusBar(int mode);
	void UpdateStatusBarFrameInfo();
	//{{AFX_MSG(COGLView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OGLVIEW_H__2AB46761_27CD_11D1_83A0_004005308EB5__INCLUDED_)
