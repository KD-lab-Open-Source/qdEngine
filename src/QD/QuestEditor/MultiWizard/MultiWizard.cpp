// MultiWizard.cpp : implementation file
//

#include "stdafx.h"
#include "../QuestEditor.h"
#include "MultiWizard.h"
#include "mwbasepage.h"
#include <memory>
#include "mwwizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Отступ слева для три-контрола
const int TREE_LEFT_OFFSET = 10;
/// Ширина дерева
const int TREE_WIDTH = 200;
/// Высота дерева
const int TREE_HEIGHT = 300;
/// Растояние между деревом и страничками
const int PAGE_DIST_TO_TREE = 5;

/// Индексы кнопки "Следующий визард" 
const UINT IDC_NEXT_WIZARD_BTN = ID_WIZFINISH + 10;
/// Индексы кнопки "Предыдущий визард" 
const UINT IDC_BACK_WIZARD_BTN = ID_WIZFINISH + 11;
/// Индексы дерева
const UINT IDC_WIZARDTREE		= ID_WIZFINISH + 12;
/////////////////////////////////////////////////////////////////////////////
// MultiWizard



IMPLEMENT_DYNAMIC(MultiWizard, CPropertySheet)

MultiWizard::MultiWizard(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_cur_wizard = m_wizards.end();
	SetWizardMode();
	m_uiCapID = nIDCaption;
}

MultiWizard::MultiWizard(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_cur_wizard = m_wizards.end();
	SetWizardMode();
}

MultiWizard::~MultiWizard()
{
}

BEGIN_MESSAGE_MAP(MultiWizard, CPropertySheet)
	//{{AFX_MSG_MAP(MultiWizard)
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_SELCHANGEDW, IDC_WIZARDTREE, OnTreeItemChange)
	ON_NOTIFY(TVN_SELCHANGEDA, IDC_WIZARDTREE, OnTreeItemChange)

	ON_NOTIFY(TVN_SELCHANGINGW, IDC_WIZARDTREE, OnTreeItemChanging)
	ON_NOTIFY(TVN_SELCHANGINGA, IDC_WIZARDTREE, OnTreeItemChanging)

	ON_BN_CLICKED(IDC_NEXT_WIZARD_BTN, OnNextWizard)
	ON_BN_CLICKED(IDC_BACK_WIZARD_BTN, OnPrevWizard)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MultiWizard message handlers

/// Сдвигает страницу так, чтобы она не перекрывала дерево
VOID MultiWizard::AdjustPageLayout(MWBasePage* page){

	ASSERT(page);

	CRect r;
	page->GetWindowRect(r);
	ScreenToClient(r);
	
	page->SetWindowPos(NULL, TREE_LEFT_OFFSET + TREE_WIDTH + PAGE_DIST_TO_TREE, 
							r.top, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE|SWP_NOSIZE);
}


BOOL MultiWizard::OnInitDialog(){

	BOOL bRes = CPropertySheet::OnInitDialog();
	ASSERT(GetPageCount()>1);
	
	CRect rectWnd;
	GetWindowRect(rectWnd);

	GetTabControl()->ShowWindow(SW_HIDE);

	CRect rcPage = GetFirstPageRect();
	
	CRect rectTree(TREE_LEFT_OFFSET, rcPage.top, 
					TREE_LEFT_OFFSET + TREE_WIDTH, rcPage.bottom);

	m_wndTree.Create(WS_VISIBLE | WS_TABSTOP | WS_CHILD /*| WS_BORDER*/
		| TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES |	TVS_SHOWSELALWAYS
		| TVS_DISABLEDRAGDROP, rectTree, this, IDC_WIZARDTREE);
	
	m_wndTree.ModifyStyleEx(0, WS_EX_STATICEDGE);
	
	//вычисляем насколько надо увеличить ширину окна
	//чтобы все без проблем влазило.
	int dx = rectTree.Width() + rcPage.Width() 
					+ PAGE_DIST_TO_TREE + 2*TREE_LEFT_OFFSET - rectWnd.Width();
	if (dx < 0)
		dx = 0;

	SetWindowPos(NULL, 0, 0, rectWnd.Width() + dx, rectWnd.Height(),
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	MoveStandartButtons();
	AddNavigationButtons();

	FillTree();

	m_cur_wizard = m_wizards.begin();
	ASSERT(!m_wizards.empty());
	ActivateWizard(*m_cur_wizard);
	CenterWindow();
	
	if(GetDlgItem(IDCANCEL))
		GetDlgItem(IDCANCEL)->
			SetWindowText(CString((LPTSTR)IDS_MULTIWIZARD_IDCANCEL_CAP));
	return bRes;
}

/// Определяет рект первой страницы в наборе
CRect MultiWizard::GetFirstPageRect(){
	CRect r(0, 0, 0, 0);
	CPropertyPage* page = GetPage(0);
	ASSERT(page);//pages must be added
	page->GetWindowRect(r);
	ScreenToClient(r);
	return r;
}

/// Сдвигает стандратные кнопки, так чтобы они оказались под страничками
VOID MultiWizard::MoveStandartButtons()
{
	CRect pbRect;
	CRect rcPage = GetFirstPageRect();

	CWnd * pb = GetDlgItem(ID_WIZBACK);

	pb->GetWindowRect(pbRect);
	ScreenToClient(pbRect);

	int dx = rcPage.left - pbRect.left;
	if (dx < 0)
		dx = 0;
	pb->SetWindowPos(NULL, pbRect.left + dx, pbRect.top, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	pb = GetDlgItem(ID_WIZNEXT);
	pb->GetWindowRect(pbRect);
	ScreenToClient(pbRect);

	pb->SetWindowPos(NULL, pbRect.left + dx, pbRect.top, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	pb = GetDlgItem(ID_WIZFINISH);
	if (pb){ 
		pb->GetWindowRect(pbRect);
		ScreenToClient(pbRect);

		pb->SetWindowPos(NULL, pbRect.left + dx, pbRect.top, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	pb = GetDlgItem(IDCANCEL);
	if (pb){ 
		pb->GetWindowRect(pbRect);
		ScreenToClient(pbRect);

		pb->SetWindowPos(NULL, pbRect.left + dx, pbRect.top, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	pb = GetDlgItem(IDHELP);
	if (pb){ 
		pb->GetWindowRect(pbRect);
		ScreenToClient(pbRect);
		pb->SetWindowPos(NULL, pbRect.left + dx, pbRect.top, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

/// Добавляет кнопки следующий/предыдущий визард
VOID MultiWizard::AddNavigationButtons(){

	CRect rcTree;
	m_wndTree.GetWindowRect(rcTree);
	ScreenToClient(rcTree);

	CWnd * pb = GetDlgItem(ID_WIZBACK);
	CRect rcBackBtn;
	pb->GetWindowRect(rcBackBtn);
	ScreenToClient(rcBackBtn);
	
	CRect rcBtn(rcTree.left, rcBackBtn.top,
				rcTree.left + (TREE_WIDTH>>1),rcBackBtn.bottom);

	m_wndPrevWizard.Create(_T("<<"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
		rcBtn, this, IDC_BACK_WIZARD_BTN);

	rcBtn.OffsetRect(rcBtn.right - rcBtn.left, 0);
	m_wndNextWizard.Create(_T(">>"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
		rcBtn, this, IDC_NEXT_WIZARD_BTN);
}

/**
	Добавляет визард в список визардов. 
	А все его страницы в список страниц
 */
bool MultiWizard::AddWizard(MWWizard* pw){
	WizardList_t::iterator i = 
		m_wizards.insert(m_wizards.end(),  pw);

	if (i == m_wizards.end())
		return false;
	if (m_cur_wizard == m_wizards.end())
		m_cur_wizard = i;
	add_pages(pw);
	return true;
}

/**
	Удаляет визард из списка. Если визрад был текущим, то выбор падает на предыдущий
 */
VOID MultiWizard::RemoveWizard(MWWizard* pw){
	WizardList_t::iterator i = 
		std::find(m_wizards.begin(), m_wizards.end(), pw);
	if (i != m_wizards.end())
	{
		if (i == m_cur_wizard){
			m_cur_wizard = m_wizards.erase(i);
			if (m_cur_wizard == m_wizards.end()&&!m_wizards.empty())
				--m_cur_wizard;
		}
		else
			m_wizards.erase(i);
	}
}

/**
	если визард только один, то дерева не делаем. Добавляем корневым 
	элементом только один этот визард.
	если много то, корневым делается узел с загловком m_uiCapID. 
	Все визарды добавляются под этот узел
 */
VOID MultiWizard::FillTree(){
	if (m_wizards.size() == 1)
	{
		MWWizard* pw = m_wizards.front();
		HTREEITEM hItem = m_wndTree.InsertItem(pw->caption(), TVI_ROOT);
		if (hItem) 
		{
			m_wndTree.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(pw));
			pw->set_item(hItem);
		}
	}
	else{
		CString strCap;
		if (!m_uiCapID) 
			GetWindowText(strCap);
		else
			strCap.LoadString(m_uiCapID);

		HTREEITEM hParent = m_wndTree.InsertItem(strCap);
		if (!hParent)
			return;
		WizardList_t::iterator i = m_wizards.begin(), e = m_wizards.end();
		for(; i != e; ++i){
			MWWizard* pw = *i;
			HTREEITEM hItem = m_wndTree.InsertItem(pw->caption(), hParent);
			if (hItem) 
			{
				m_wndTree.SetItemData(hItem, reinterpret_cast<DWORD_PTR>(pw));
				pw->set_item(hItem);
			}
		}
		m_wndTree.Expand(hParent, TVE_EXPAND);
	}
}

/**
	При смене узла в дереве текущий визард сюспендится и скрывается.
	Вместо него показывается визард соответсвующий выбранному узлу 
 */
VOID MultiWizard::OnTreeItemChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pTreeView = (NM_TREEVIEW*)pNMHDR;
	
	*pResult = 0;
	if (!pTreeView->itemNew.lParam) 
	{
		*pResult = TRUE;
	}
	if (pTreeView->itemNew.hItem&&pTreeView->itemNew.lParam) 
	{
		WizardList_t::iterator res = std::find(m_wizards.begin(), m_wizards.end(), 
			reinterpret_cast<MWWizard*>(pTreeView->itemNew.lParam));
		ASSERT(res != m_wizards.end());
		if(m_cur_wizard != res){
			CWaitCursor __wait__;
			if(!SuspendWizard(*m_cur_wizard))
			{
				*pResult = TRUE;
				return;
			}
			if(!ShowWizard(*res))
			{
				*pResult = TRUE;
				return;
			}
			m_cur_wizard = res;
		}
	}
	CheckAccessToNWB();
	CheckAccessToPWB();
}

VOID MultiWizard::OnTreeItemChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pTreeView = (NM_TREEVIEW*)pNMHDR;
}

/// Скрывает текущий визард и активизирует следующий по списку
VOID MultiWizard::OnNextWizard()
{
	CWaitCursor __wait__;
	ASSERT(m_cur_wizard != --m_wizards.end());

	if(!SuspendWizard(*m_cur_wizard))
		return;
	WizardList_t::iterator res = m_cur_wizard;
	++res;
	if(ActivateWizard(*res))
		m_cur_wizard = res;
}

/// Скрывает текущий визард и активизирует предыдущий по списку
VOID MultiWizard::OnPrevWizard()
{
	ASSERT(m_cur_wizard != m_wizards.begin());

	CWaitCursor __wait__;
	if(!SuspendWizard(*m_cur_wizard))
		return;
	WizardList_t::iterator res = m_cur_wizard;
	--res;
	if(ActivateWizard(*res))
		m_cur_wizard = res;
}

/// Управляет доступом к кнопкам следующий/предыдущий визард.
/**
	Если доступ разрешается, то делается еще проверка на возможность 
	дать доступ к каждой конкртной кнопке
 */
VOID MultiWizard::SuppresWizardChange(BOOL bSupp){
	m_wndTree.EnableWindow(!bSupp);
	if (bSupp) //т.е. запрещаем доступ
	{
		m_wndNextWizard.EnableWindow(!bSupp);
		m_wndPrevWizard.EnableWindow(!bSupp);
	}
	else{
		CheckAccessToNWB();
		CheckAccessToPWB();
	}
}

/// Управляет доступом кнопки "Следующий визард"
/** 
	Проверяет можно ли переключиться на следующий визард
*/
VOID MultiWizard::CheckAccessToNWB()
{
	m_wndNextWizard.EnableWindow(!m_wizards.empty()
		&&m_cur_wizard!=--m_wizards.end());
}

/// Управляет доступом кнопки "Предыдущий визард"
/** 
	Проверяет можно ли переключиться на Предыдущий визард
*/
VOID MultiWizard::CheckAccessToPWB(){
	m_wndPrevWizard.EnableWindow(m_cur_wizard!=m_wizards.begin());
}

/// Показывает визард
/**
	Резъюмит визард. И активизирует его текущую страницу
 */
bool MultiWizard::ShowWizard(MWWizard* pw)
{
	ASSERT(pw);
	bool bres = pw->ResumeWizard();
	if (bres) 
		SetActivePage(pw->cur_page());
	return bres;
}

/**
	Выбирает узел в дереве, соответствующий визарду и вызывает ShowWizard
 */
bool MultiWizard::ActivateWizard(MWWizard* pw)
{
	ASSERT(pw);
	m_wndTree.SelectItem(pw->item());
	return ShowWizard(pw);
}

/// Сюспендит визард. 
bool MultiWizard::SuspendWizard(MWWizard* pw){
	return pw->SuspendWizard();
}

/**
	Уведомляет о том, что визард закончил работу
 */
VOID MultiWizard::OnEndWizard(){
	if (m_cur_wizard != --m_wizards.end())
		OnNextWizard();
	else
		EndDialog(ID_WIZFINISH);
}

/// Добавляет страницы визарда в список страниц CPropertySheet
void MultiWizard::add_pages(MWWizard* pw)
{
	const MWPagesList_t& l = pw->pages_list();
	MWPagesList_t::const_iterator i = l.begin(), e = l.end();
	for(; i != e; ++i){
		AddPage(*i);
	}
}
