// Main\CondListEditor.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include ".\CondListEditor.h"


// CCondListEditor dialog

IMPLEMENT_DYNAMIC(CCondListEditor, CDialog)
CCondListEditor::CCondListEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CCondListEditor::IDD, pParent)
{
}

CCondListEditor::~CCondListEditor()
{
}

void CCondListEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONDITIONS, m_listBox);
	DDX_Control(pDX, IDC_BUTTON_UP, m_upButton);
	DDX_Control(pDX, IDC_BUTTON_DOWN, m_downButton);
	DDX_Control(pDX, IDC_BUTTON_FAST_UP, m_fastUpButton);
	DDX_Control(pDX, IDC_BUTTON_FAST_DOWN, m_fastDownButton);
}

void CCondListEditor::SwapLines(int i, int j)
{
	CString cur, pre;
	modifPermut_.swap(i,j);
	m_listBox.GetText(i, cur);
	m_listBox.GetText(j, pre);
	m_listBox.DeleteString(i);
	m_listBox.InsertString(i, pre);
	m_listBox.DeleteString(j);
	m_listBox.InsertString(j, cur);
}

void CCondListEditor::UpdateButtons()
{
	int cur_sel = m_listBox.GetCurSel();
	m_upButton.EnableWindow(false);
	m_fastUpButton.EnableWindow(false);
	m_downButton.EnableWindow(false);
	m_fastDownButton.EnableWindow(false);
	if (LB_ERR != cur_sel)
	{
		if (cur_sel > 0) 
		{
			m_upButton.EnableWindow(true);
			m_fastUpButton.EnableWindow(true);
		}
		if (cur_sel < m_listBox.GetListBoxInfo()-1) 
		{
			m_downButton.EnableWindow(true);
			m_fastDownButton.EnableWindow(true);
		}
	}
}

void CCondListEditor::TryLiftUp()
{
	int cur_sel = m_listBox.GetCurSel();
	if (LB_ERR != cur_sel && cur_sel > 0)
	{
		SwapLines(cur_sel-1, cur_sel);
		m_listBox.SetCurSel(cur_sel-1);
		UpdateButtons();
	}
}

void CCondListEditor::TryLiftDown()
{
	int cur_sel = m_listBox.GetCurSel();
	if (LB_ERR != cur_sel && cur_sel < m_listBox.GetListBoxInfo()-1)
	{
		SwapLines(cur_sel+1, cur_sel);
		m_listBox.SetCurSel(cur_sel+1);
		UpdateButtons();
	}	
}

BEGIN_MESSAGE_MAP(CCondListEditor, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_LBN_SELCHANGE(IDC_LIST_CONDITIONS, OnLbnSelchangeListConditions)
	ON_LBN_SELCANCEL(IDC_LIST_CONDITIONS, OnLbnSelcancelListConditions)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_FAST_UP, OnBnClickedButtonFastUp)
	ON_BN_CLICKED(IDC_BUTTON_FAST_DOWN, OnBnClickedButtonFastDown)
END_MESSAGE_MAP()

// CCondListEditor message handlers

BOOL CCondListEditor::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	modifPermut_ = g_permutation;
	fillConditionList(m_listBox);
	m_listBox.SetCurSel(-1);
	UpdateButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCondListEditor::OnBnClickedButtonUp()
{
	TryLiftUp();
}

void CCondListEditor::OnBnClickedButtonDown()
{
	TryLiftDown();
}

void CCondListEditor::OnLbnSelchangeListConditions()
{
	UpdateButtons();
}

void CCondListEditor::OnLbnSelcancelListConditions()
{
	UpdateButtons();
}

void CCondListEditor::OnBnClickedButtonFastUp()
{
	for (int i = 0; i < 10; i++)
		TryLiftUp();
}

void CCondListEditor::OnBnClickedButtonFastDown()
{
	for (int i = 0; i < 10; i++)
		TryLiftDown();
}

void CCondListEditor::OnOK()
{
	g_permutation = modifPermut_; // Заменяем глобальную перестановку условий созданной нами

	CDialog::OnOK();
}
