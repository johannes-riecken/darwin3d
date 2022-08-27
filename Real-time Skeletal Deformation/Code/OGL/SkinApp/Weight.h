#if !defined(AFX_WEIGHT_H__04F2D720_B669_11D1_83A0_004005308EB5__INCLUDED_)
#define AFX_WEIGHT_H__04F2D720_B669_11D1_83A0_004005308EB5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Weight.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWeight dialog

class CWeight : public CDialog
{
public:
	int m_pos;
// Construction
public:
	CWeight(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWeight)
	enum { IDD = IDD_SET_WEIGHTS };
	CSliderCtrl	m_Slider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWeight)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWeight)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEIGHT_H__04F2D720_B669_11D1_83A0_004005308EB5__INCLUDED_)
