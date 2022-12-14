///////////////////////////////////////////////////////////////////////////////
//
// Kine.h : main header file for the Kine application
//
// Purpose:	header of Main Application of Inverse Kinematic System
//
// Created:
//		JL 7/1/98
//
///////////////////////////////////////////////////////////////////////////////
//
//	Copyright 1998 Jeff Lander, All Rights Reserved.
//  For educational purposes only.
//  Please do not republish in electronic or print form without permission
//  Thanks - jeffl@darwin3d.com
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_Kine_H__082DB1E4_6069_11D1_83A0_004005308EB5__INCLUDED_)
#define AFX_Kine_H__082DB1E4_6069_11D1_83A0_004005308EB5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
#ifdef _MSC_VER
	#error include 'stdafx.h' before including this file for PCH
#endif
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CKineApp:
// See Kine.cpp for the implementation of this class
//

class CKineApp : public CWinApp
{
public:
	CKineApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKineApp)
	public:
	virtual bool InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKineApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Kine_H__082DB1E4_6069_11D1_83A0_004005308EB5__INCLUDED_)
