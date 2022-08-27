// Weight.cpp : implementation file
//

#include "stdafx.h"
#include "skinapp.h"
#include "Weight.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWeight dialog


CWeight::CWeight(CWnd* pParent /*=NULL*/)
	: CDialog(CWeight::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWeight)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWeight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWeight)
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWeight, CDialog)
	//{{AFX_MSG_MAP(CWeight)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWeight message handlers

int CWeight::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

BOOL CWeight::DestroyWindow() 
{
	m_pos = m_Slider.GetPos();
	return CDialog::DestroyWindow();
}

BOOL CWeight::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Slider.SetRange( 0, 256, TRUE);
//	m_Slider.SetSelection(  0, 256 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
