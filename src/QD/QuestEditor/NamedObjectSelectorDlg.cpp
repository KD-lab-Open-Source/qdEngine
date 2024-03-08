// NamedObjectSelectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "NamedObjectSelectorDlg.h"


// NamedObjectSelectorDlg dialog

IMPLEMENT_DYNCREATE(NamedObjectSelectorDlg, CDialog)

NamedObjectSelectorDlg::NamedObjectSelectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NamedObjectSelectorDlg::IDD, pParent)
	, m_ptrListLoader(NULL)
	, m_uiCapId(static_cast<UINT_PTR>(-1))
{
}

NamedObjectSelectorDlg::~NamedObjectSelectorDlg()
{
}

void NamedObjectSelectorDlg::setListLoader(INamedObjListToList * loader)
{
	m_ptrListLoader = loader;
}

void NamedObjectSelectorDlg::setListReader(INamedObjListReader *reader)
{
	m_ptrListReader = reader;
}

void NamedObjectSelectorDlg::setCapId(UINT_PTR uiCapID)
{
	m_uiCapId = uiCapID;
}

void NamedObjectSelectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
}

BOOL NamedObjectSelectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_ptrListLoader)
		return FALSE;

	m_list.SetExtendedStyle(LVS_EX_CHECKBOXES|m_list.GetExtendedStyle());
	InsertColumns();
	if (!m_ptrListLoader->load(m_list))
		return FALSE;  // return TRUE  unless you set the focus to a control
	SetWindowText(CString(reinterpret_cast<LPTSTR>(m_uiCapId)));
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	return TRUE;
}

BEGIN_MESSAGE_MAP(NamedObjectSelectorDlg, CDialog)
END_MESSAGE_MAP()

void NamedObjectSelectorDlg::OnOK()
{
	ASSERT(m_ptrListReader);
	if (m_ptrListReader)
		m_ptrListReader->read(m_list);

	__super::OnOK();
}

void NamedObjectSelectorDlg::InsertColumns()
{
	m_list.InsertColumn(0, _T("Èìÿ"));
}
