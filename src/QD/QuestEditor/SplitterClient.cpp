// SplitterClient.cpp : implementation file
//

#include "stdafx.h"
#include "questeditor.h"
#include "SplitterClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplitterClient
IMPLEMENT_DYNCREATE(CSplitterClient,CWnd);

CSplitterClient::CSplitterClient()
{
	m_pResizedChild = NULL;
}

CSplitterClient::~CSplitterClient()
{
}


BEGIN_MESSAGE_MAP(CSplitterClient, CWnd)
//{{AFX_MSG_MAP(CSplitterClient)
ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplitterClient message handlers
void CSplitterClient::SetResizedChild(CWnd* ptrChild)
{
	m_pResizedChild = ptrChild;
	ResizeChild();
}

void CSplitterClient::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	ResizeChild();
}

void CSplitterClient::ResizeChild()
{
	if (m_pResizedChild == NULL) {
		return;
	}
	CRect r;
	GetClientRect(&r);
	
	m_pResizedChild->MoveWindow(r);
}
void CSplitterClient::PostNcDestroy()
{
	CWnd::PostNcDestroy();
	delete this;
}
