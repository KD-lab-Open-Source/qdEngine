// FloatBar.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "FloatBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFloatBar dialog


CFloatBar::CFloatBar(CWnd* pParent /*=NULL*/)
//	: CWnd(CFloatBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFloatBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFloatBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFloatBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFloatBar, CDialogBar)
	//{{AFX_MSG_MAP(CFloatBar)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFloatBar message handlers
