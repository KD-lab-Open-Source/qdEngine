// TriggerDbgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "TriggerDbgDlg.h"
#include "qd_trigger_profiler.h"
#include "ListCtrl_insert_iterator.h"
#include "ITriggerView.h"
#include <algorithm>
#include "TrgProfLegend.h"
#include "te_consts.h"
#include "ProfilerDataToFile.h"
#include ".\triggerdbgdlg.h"

#include "curdirsafer.h"
#include <ShLwApi.h>
#include "custom_ddx.h"
#include "file_tests.h"
#include ".\minigameproppage.h"
#include "disk_op.h"

namespace{
CurDirSafer& getCurDirSafer()
{
	static CurDirSafer safer;
	return safer;
}
}

namespace listCtrlInserter
{
	template<>
	struct data_traits<qdTriggerProfilerRecord> {
		static int insert(CListCtrl* ctrl,int pos, const qdTriggerProfilerRecord& data)
		{
			return ctrl->InsertItem(pos,qdTriggerProfiler::record_text(data));
		}
	};
}

// TriggerDbgDlg dialog

IMPLEMENT_DYNAMIC(TriggerDbgDlg, CDialog)
TriggerDbgDlg::TriggerDbgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TriggerDbgDlg::IDD, pParent)
	, m_ptrITView(NULL)
{
}

TriggerDbgDlg::~TriggerDbgDlg()
{
}

bool TriggerDbgDlg::SetTriggerView(interface ITriggerView* ptrITView)
{
	m_ptrITView = ptrITView;
	return true;
}
void TriggerDbgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_SCROLLBAR, m_scrlr);
}


BEGIN_MESSAGE_MAP(TriggerDbgDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnLvnItemchangedList)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnNMRclickList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, OnLvnKeydownList)
	ON_COMMAND(IDC_LOAD_DBG, OnLoadDbg)
	ON_COMMAND(IDC_LOAD_DBG_FROM_FILE, OnLoadDbgFromFile)
	ON_COMMAND(IDC_CLEAR_DBG_LIST, OnClearDbgList)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_SHOW_LEGEND, OnBnClickedBtnShowLegend)
	ON_COMMAND(ID_SAVETOFILE_PROFILER_DATA, OnSavetofileProfilerData)
END_MESSAGE_MAP()


BOOL TriggerDbgDlg::OnInitDialog()
{
	BOOL bRes = CDialog::OnInitDialog();
	ASSERT(m_ptrITView);

	m_list.InsertColumn(0, _T(""), LVCFMT_LEFT, 80);
	SetupScroller();
	return bRes;
}
// TriggerDbgDlg message handlers

bool TriggerDbgDlg::Load(const char* fname /*=NULL*/)
{
	CWaitCursor _cursor_;
	qdTriggerProfiler* pProf = &qdTriggerProfiler::instance();

	std::string pre_str = pProf->work_file();
	if (NULL != fname) pProf->set_work_file(fname);
	bool load_result = pProf->load_from_work_file();
	pProf->set_work_file(pre_str.c_str());

	if (!load_result)
		return false;

	m_list.DeleteAllItems();
	std::copy(pProf->records_begin(), pProf->records_end(), 
		listCtrlInserter::listCtrl_inserter(m_list, 0));
	m_list.SetColumnWidth(0, LVSCW_AUTOSIZE);
	SelectItem(0);
	SetupScroller();
	return true;
}

bool TriggerDbgDlg::LoadFromFile()
{
	CString title;
	title.LoadString(IDS_OPEN_PROFILER_FILE_TITLE);
	CString filter;
	filter.LoadString(IDS_OPEN_PROFILER_FILE_FILTER);
	CString ext;
	ext.LoadString(IDS_OPEN_PROFILER_FILE_EXT);
	CString fileName = 
			disk_op::Get1DiskFileName(getCurDirSafer, filter, ext, title, true);

	return Load(fileName);
}

bool TriggerDbgDlg::SelectItem(int iItem) const{
	return m_list.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
}
bool TriggerDbgDlg::MoveSelection(int count) const
{
	int const items = m_list.GetItemCount();
	if (!items)
		return false;

	POSITION pos = m_list.GetFirstSelectedItemPosition();
	int iSel = m_list.GetNextSelectedItem(pos);

	if ((count > 0 &&iSel == items - 1)
		||(count < 0 &&!iSel))
		return false;

	iSel += count;
	if (iSel < 0)
		iSel = 0;
	if (iSel > items - 1)
		iSel = items -1;

	SelectItem(iSel);
	m_list.EnsureVisible(iSel, FALSE);
	return true;
}

bool TriggerDbgDlg::Prev() const
{
	return MoveSelection(-1);
}

bool TriggerDbgDlg::Next() const
{
	return MoveSelection(1);
}


void TriggerDbgDlg::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->uNewState&LVIS_SELECTED)
	{
		qdTriggerProfiler* pProf = &qdTriggerProfiler::instance();
		if (pProf->evolve(pNMLV->iItem))
		{
			m_scrlr.SetScrollPos(pNMLV->iItem);
			m_ptrITView->Redraw();
		}
	}
	*pResult = 0;
}


void TriggerDbgDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CRect r;
	GetClientRect(&r);
	r.DeflateRect(2, 1);
	if (IsWindow(m_list.m_hWnd))
	{	
		CWnd * pbtn = GetDlgItem(IDC_BTN_SHOW_LEGEND);
		ASSERT(pbtn);
		CRect btnRect;
		pbtn->GetWindowRect(btnRect);
		ScreenToClient(btnRect);

		r.top += 2;
		r.top += btnRect.Height() + 2;

		m_list.MoveWindow(r);

		r.bottom = r.top - 2;
		r.top = r.bottom - btnRect.Height();
		r.right -= btnRect.Width() + 1;
		m_scrlr.MoveWindow(r);

		r.left = r.right + 1;
		r.right -= btnRect.Width() + 1;
		pbtn->SetWindowPos(NULL, r.left, r.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	}
}

void TriggerDbgDlg::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU_PROF_DLG);
	CMenu* pm = menu.GetSubMenu(0);
	CPoint p;
	::GetCursorPos(&p);
	pm->TrackPopupMenu(TPM_LEFTBUTTON, p.x, p.y, this);
	*pResult = 0;
}

void TriggerDbgDlg::OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDow->wVKey == VK_F5)
		Load();
	*pResult = 0;
}

void TriggerDbgDlg::OnLoadDbg()
{
	Load();
}

void TriggerDbgDlg::OnLoadDbgFromFile()
{
	LoadFromFile();
}

void TriggerDbgDlg::OnClearDbgList()
{
	m_list.DeleteAllItems();
	m_scrlr.EnableScrollBar(ESB_DISABLE_BOTH);
}

void TriggerDbgDlg::SetupScroller()
{
	if (const int nmax = m_list.GetItemCount())
	{
		SCROLLINFO si;
		memset(&si, 0, sizeof(si));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMin = 0;
		si.nMax = nmax;
		si.nTrackPos = si.nPos = 0;
		si.nPage = 10;
		m_scrlr.SetScrollInfo(&si);
	}
	else
		m_scrlr.EnableScrollBar(ESB_DISABLE_BOTH);
}

void TriggerDbgDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch(nSBCode) {
	case SB_LEFT:
		SelectItem(0);
		break;
	case SB_RIGHT:
		SelectItem(m_list.GetItemCount()-1);
		break;
	case SB_LINELEFT:
		Prev();
		break;
	case SB_LINERIGHT:
		Next();
		break;
	case SB_PAGELEFT:    // Scroll one page left.
		{
			// Get the page size. 
			SCROLLINFO   info;
			info.cbSize = sizeof(SCROLLINFO);
			pScrollBar->GetScrollInfo(&info, SIF_ALL);
		   
			MoveSelection(-static_cast<int>(info.nPage));
			break;
		}
	case SB_PAGERIGHT:      // Scroll one page right.
		{
			const int maxpos = m_list.GetItemCount();
			// Get the page size. 
			SCROLLINFO   info;
			info.cbSize = sizeof(SCROLLINFO);
			pScrollBar->GetScrollInfo(&info, SIF_ALL);
			MoveSelection(info.nPage);
			break;
		}
	case SB_THUMBTRACK: 
		{// Drag scroll box to specified position. nPos is the
			SCROLLINFO si;
			memset(&si, 0, sizeof(SCROLLINFO));
			si.cbSize = sizeof(si);
			pScrollBar->GetScrollInfo(&si, SIF_ALL);

			SelectItem(si.nTrackPos);     // position that the scroll box has been dragged to.
			m_list.EnsureVisible(si.nTrackPos, TRUE);
		}
		break;
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

bool TriggerDbgDlg::Activate() const
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	int iSel = m_list.GetNextSelectedItem(pos);
	if (iSel == -1)
		return true;
	qdTriggerProfiler::instance().evolve(iSel);
	return true;
}

bool TriggerDbgDlg::Deactivate() const
{
	return true;
}

void TriggerDbgDlg::OnBnClickedBtnShowLegend()
{
	if (!m_ptrTrgProfLegend)
	{
		m_ptrTrgProfLegend.reset(new TrgProfLegend);
		if (!m_ptrTrgProfLegend)
		{
			Beep(1000, 100);
			return;
		}
		m_ptrTrgProfLegend->initColors(DONED_LINK_CLR, ACTIVE_LINK_CLR, 
										INACTIVE_LINK_CLR, DONED_ELE_CLR, 
										WORKING_ELE_CLR, INACTIVE_ELE_CLR, 
										WAITING_ELE_CLR);
		m_ptrTrgProfLegend->Create(TrgProfLegend::IDD, this);
	}
	if (!m_ptrTrgProfLegend->IsWindowVisible())
	{
		CRect r;
		GetWindowRect(r);
		CRect r2;
		m_ptrTrgProfLegend->GetWindowRect(r2);
		r.left = r.right - r2.Width();
		r.top = r.bottom - r2.Height();
		m_ptrTrgProfLegend->SetWindowPos(NULL, r.left, r.top, 
			0, 0, SWP_NOSIZE|SWP_NOZORDER);
		m_ptrTrgProfLegend->ShowWindow(SW_SHOW);
	}
}

void TriggerDbgDlg::OnShow()
{
}
void TriggerDbgDlg::OnHide()
{
	if (m_ptrTrgProfLegend)
		m_ptrTrgProfLegend->ShowWindow(SW_HIDE);
}

void TriggerDbgDlg::OnSavetofileProfilerData()
{
	ProfilerDataToFile::run(qdTriggerProfiler::instance());
}
