// CounterPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "CounterPropPage.h"

#include "qd_game_scene.h"
#include "qd_game_object_animated.h"
#include "obj_type_detectors.h"

#include "ptree_hlprs.hpp"

#include <algorithm>
#include <boost/function_output_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>

// CounterPropPage dialog


CounterPropPage::eleCopy::eleCopy(ElementsContainer& cont)
: m_cont(cont)
{}

void CounterPropPage::eleCopy::operator ()(qdCounterElement const& pce)
{
	m_cont.push_back(boost::shared_ptr<qdCounterElement>
		(new qdCounterElement(pce.state(), pce.increment_value())));
}

CounterPropPage::InsertScene::InsertScene(QETreeListCtrl& tree)
:m_wndTree(tree)
{
}

void CounterPropPage::InsertScene::operator()(class qdGameScene const* pscene)
{
	CTreeListItem * pitem = m_wndTree.InsertItem(pscene->name());
	if (!pitem)
		return;

	m_wndTree.SetItemData(pitem, reinterpret_cast<DWORD_PTR>(pscene));
	qdGameObjectList const& lst = pscene->object_list();
	std::for_each(lst.begin(), lst.end(), InsertObject(m_wndTree, pitem));
}

CounterPropPage::InsertObject::InsertObject(QETreeListCtrl& tree, 
											CTreeListItem* pParent)
: m_wndTree(tree)
, m_ptrParent(pParent)
{
}
void CounterPropPage::InsertObject::operator()(class qdGameObject const* pobj)
{
	if (ptree_helpers::is_animated(pobj))
	{
		bool const bIsGlobal = (pobj->owner()->owner() == NULL);
		qdGameObjectAnimated const* const panim= 
			static_cast<qdGameObjectAnimated const*>(pobj);
		qdGameObjectStateVector const& v = panim->state_vector();
		std::for_each(v.begin(),v.end(),InsertState(m_wndTree, m_ptrParent, bIsGlobal));
	}
}

CounterPropPage::InsertState::InsertState(QETreeListCtrl& tree, 
										  CTreeListItem * pParent, bool bLoadGlobal)
: m_wndTree(tree)
, m_ptrParent(pParent)
, m_bLoadGlobal(bLoadGlobal)
{
}
void CounterPropPage::InsertState::operator()(qdGameObjectState const* pstate)
{
	//глобальные состояния загружаем отдельно
	if (!m_bLoadGlobal&&pstate->owner() != pstate->ref_owner())
		return;

	CString str;
	str.Format(IDS_FORMAT_STATE_PP_OBJ, pstate->name(), pstate->owner()->name());
	
	CTreeListItem * pitem = m_wndTree.InsertItem(str, m_ptrParent);
	if (!pitem)						 
		return;
	m_wndTree.SetItemData(pitem, reinterpret_cast<DWORD_PTR>(pstate));
}

CounterPropPage::InsertIncDecState::InsertIncDecState(CounterPropPage* pPage)
: 
m_pOwnerPage(pPage)
{}

//! Вставляет состояние из элемената в указанный список
void CounterPropPage::InsertIncDecState::AddItem2List(QETreeListCtrl& lst, 
													  qdCounterElement const* pele)
{
	CString str;
	qdGameObjectState const* const pstate = pele->state();
	if (!pstate)
	{
		str.LoadString(IDS_NOTHING);
		lst.InsertItem(str);
		return;
	}
	if (pstate->owner()->owner()
		&&!ptree_helpers::IsGlobalState4Obj(pstate,
				static_cast<qdGameObjectAnimated const*>(pstate->owner())))
		str.Format(IDS_FORMAT_STATE_PP_OBJ2, 
					pstate->name(), 
					pstate->owner()->name(),
					pstate->owner()->owner()->name());
	else
		str.Format(IDS_FORMAT_STATE_PP_OBJ, pstate->name(), pstate->owner()->name());

	CTreeListItem* pi = lst.InsertItem(str);
	if (pi)
		lst.SetItemData(pi, reinterpret_cast<DWORD_PTR>(pele));
}

//! \brief Определяет куда буде вставлено состояние.
//! Выставляет картинку для состояния в общем дереве
void CounterPropPage::InsertIncDecState::operator()
										(boost::shared_ptr<qdCounterElement>& pele)
{
	if (pele->increment_value())
		AddItem2List(m_pOwnerPage->getIncStates(), pele.get());
	else
		AddItem2List(m_pOwnerPage->getDecStates(), pele.get());
	if(pele->state()){
		CTreeListItem* pitem = m_pOwnerPage->findStateItem(pele->state());
		if (pitem)
		{
			int const nImage = static_cast<int>(!pele->increment_value());
			m_pOwnerPage->getAllStates().SetItemImage(pitem, nImage, nImage, nImage, nImage);
//			pitem->SetImage(nImage, nImage, nImage, nImage);
//			pitem->ShowTreeImage(TRUE);
		}
	}

}

IMPLEMENT_DYNAMIC(CounterPropPage, CBasePage)
CounterPropPage::CounterPropPage()
	: CBasePage(CounterPropPage::IDD)
	, m_ptrGameDisp(NULL)
	, m_ptrCounter(NULL)
	, m_iLimitValue(0)
	, m_bOnlyPositive(FALSE)
{
}

CounterPropPage::~CounterPropPage()
{
}

void CounterPropPage::DoDataExchange(CDataExchange* pDX)
{
	CBasePage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM1, m_wndAllStates);
	DDX_Control(pDX, IDC_CUSTOM2, m_wndIncStates);
	DDX_Control(pDX, IDC_CUSTOM3, m_wndDecStates);
	DDX_Text(pDX, IDC_EDIT_MAX_VALUE, m_iLimitValue);
	DDX_Check(pDX, IDC_CHECK_ONLY_POSITIVE, m_bOnlyPositive);
}

void CounterPropPage::set_data(qdGameDispatcher* ptrDisp, qdCounter* ptrCounter)
{
	m_ptrCounter = ptrCounter;
	m_ptrGameDisp = ptrDisp;
	real2back();
}

BEGIN_MESSAGE_MAP(CounterPropPage, CBasePage)
	ON_BN_CLICKED(IDC_BUTTON_ADD_INC, OnBnClickedButtonAddInc)
	ON_BN_CLICKED(IDC_BUTTON_DEL_INC, OnBnClickedButtonDelInc)
	ON_BN_CLICKED(IDC_BUTTON_ADD_DEC, OnBnClickedButtonAddDec)
	ON_BN_CLICKED(IDC_BUTTON_DEL_DEC, OnBnClickedButtonDelDec)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAX_VALUE, OnDeltaposSpinMaxValue)
	ON_BN_CLICKED(IDC_CHECK_ONLY_POSITIVE, OnBnClickedCheckOnlyPositive)
END_MESSAGE_MAP()


// CounterPropPage message handlers

//! Выставляет стили для списков уменьшающих/наращивающих состояний
void CounterPropPage::setTreeListCtrlListStyles(QETreeListCtrl& ctrl)
{
	ctrl.SetStyle
		( 0
//		| TLC_BKGNDIMAGE							// image background
//		| TLC_BKGNDCOLOR							// colored background ( not client area )
//		| TLC_DOUBLECOLOR							// double color background

		| TLC_MULTIPLESELECT						// single or multiple select
		| TLC_SHOWSELACTIVE							// show active column of selected item
		| TLC_SHOWSELALWAYS							// show selected item always
		| TLC_SHOWSELFULLROWS						// show selected item in fullrow mode

		| TLC_HEADER								// show header
		| TLC_HGRID									// show horizonal grid lines
		| TLC_VGRID									// show vertical grid lines
		| TLC_TGRID									// show tree horizonal grid lines ( when HGRID & VGRID )
		| TLC_HGRID_EXT								// show extention horizonal grid lines
		| TLC_VGRID_EXT								// show extention vertical grid lines
		| TLC_HGRID_FULL							// show full horizonal grid lines
		| TLC_READONLY								// read only

//		| TLC_IMAGE									// show image
		| TLC_HOTTRACK								// show hover text 

		| TLC_DROP									// drop support
		| TLC_DRAG									// drop support
//		| TLC_DROPTHIS								// drop self support
		| TLC_DROPROOT								// drop on root support

//		| TLC_HEADDRAGDROP							// head drag drop
		| TLC_NOAUTOCHECK							// do NOT auto set checkbox of parent & child
		| TLC_NOAUTOLOCK							// do NOT auto set lockbox of parent & child
		);

}

//! Выставляет стили для общего дерева состояний
void CounterPropPage::setTreeListCtrlTreeStyles(QETreeListCtrl& ctrl)
{
	ctrl.SetStyle
		( 0
		| TLC_TREELIST
//		| TLC_BKGNDIMAGE							// image background
//		| TLC_BKGNDCOLOR							// colored background ( not client area )
		| TLC_DOUBLECOLOR							// double color background

		| TLC_MULTIPLESELECT						// single or multiple select
		| TLC_SHOWSELACTIVE							// show active column of selected item
		| TLC_SHOWSELALWAYS							// show selected item always
		| TLC_SHOWSELFULLROWS						// show selected item in fullrow mode

		| TLC_HEADER								// show header
		| TLC_HGRID									// show horizonal grid lines
		| TLC_VGRID									// show vertical grid lines
//		| TLC_TGRID									// show tree horizonal grid lines ( when HGRID & VGRID )
		| TLC_HGRID_EXT								// show extention horizonal grid lines
		| TLC_VGRID_EXT								// show extention vertical grid lines
		| TLC_HGRID_FULL							// show full horizonal grid lines
		| TLC_READONLY								// read only

		| TLC_TREELINE								// show tree line
		| TLC_ROOTLINE								// show root line
		| TLC_BUTTON								// show expand/collapse button [+]
		| TLC_IMAGE									// show image
		| TLC_HOTTRACK								// show hover text 

		| TLC_DRAG									// drag support

		| TLC_NOAUTOCHECK							// do NOT auto set checkbox of parent & child
		| TLC_NOAUTOLOCK							// do NOT auto set lockbox of parent & child
		);
}

BOOL CounterPropPage::OnApply()
{
	if (!UpdateData())
		return FALSE;

	back2real();
	if (m_bOnlyPositive)
		m_ptrCounter->set_flag(qdCounter::POSITIVE_VALUE);
	else
		m_ptrCounter->drop_flag(qdCounter::POSITIVE_VALUE);
	return CBasePage::OnApply();
}

void CounterPropPage::_1stcolumnWidth2ClientWidth(QETreeListCtrl& lst)
{
	CRect r;
	lst.GetClientRect(&r);
	lst.SetColumnWidth( 0, r.Width()-2);
}

BOOL CounterPropPage::OnInitDialog()
{
	CBasePage::OnInitDialog();

	setTreeListCtrlTreeStyles(m_wndAllStates);
	setTreeListCtrlListStyles(m_wndIncStates);
	setTreeListCtrlListStyles(m_wndDecStates);

	m_treeImages.Create(IDB_COUNTER_TREE_BMP, 16, 4, 0x00FFFFFF);
	m_wndAllStates.SetImageList(&m_treeImages);
	CString strCap;
	strCap.LoadString(IDS_CLMN_INC_STATES);
	m_wndIncStates.InsertColumn(strCap);
	strCap.LoadString(IDS_CLMN_DEC_STATES);

	m_wndDecStates.InsertColumn(strCap);

	strCap.LoadString(IDS_CLMN_ALL_STATES);
	m_wndAllStates.InsertColumn(strCap);
	m_wndAllStates.SetImageList(&m_treeImages);
	_1stcolumnWidth2ClientWidth(m_wndAllStates);
	_1stcolumnWidth2ClientWidth(m_wndIncStates);
	_1stcolumnWidth2ClientWidth(m_wndDecStates);

	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN_MAX_VALUE))->SetRange32(0, INT_MAX);

	CWaitCursor wait;
	loadAllStates();
	loadIncDecStates();
	m_bOnlyPositive = m_ptrCounter->check_flag(qdCounter::POSITIVE_VALUE);
	m_wndIncStates.SetColumnWidth( 0, TLSCW_AUTOSIZE_USEHEADER );
	m_wndDecStates.SetColumnWidth( 0, TLSCW_AUTOSIZE_USEHEADER );
	UpdateData(FALSE);

	return TRUE;  
}

void CounterPropPage::OnCancel()
{
	CBasePage::OnCancel();
}

//! Копирует элеметы счетчика в собственный список
void CounterPropPage::real2back()
{
	m_elementsBackup.clear();
	std::copy(m_ptrCounter->elements().begin(), m_ptrCounter->elements().end(),
			boost::make_function_output_iterator(eleCopy(m_elementsBackup)));
	m_iLimitValue = m_ptrCounter->value_limit();
}

//! Копирует данные из собственного списка в объект.Устанавливает предельное значение
void CounterPropPage::back2real()
{
	m_ptrCounter->remove_all_elements();
	ElementsContainer::const_iterator 
		itr = m_elementsBackup.begin(), end = m_elementsBackup.end();
	for(; itr != end; ++itr)
	{
		boost::shared_ptr<qdCounterElement>const p = *itr;
		m_ptrCounter->add_element(p->state(), p->increment_value());
	}
	m_ptrCounter->set_value_limit(m_iLimitValue);
}

//! Загружает глобальные состояния в дерево
void CounterPropPage::loadGlobalStates()
{
	CString str((LPTSTR)IDS_GLOBAL_STATES_NODE);
	CTreeListItem* pitem = m_wndAllStates.InsertItem(str);
	if (pitem)
	{
		qdGameObjectList const& lst = m_ptrGameDisp->global_object_list();
		std::for_each(lst.begin(), lst.end(), InsertObject(m_wndAllStates, pitem));
	}
}

//! Загружает в дерево сцены с состояниями объектов, принадлежащих сцене
void CounterPropPage::loadAllStates()
{
	loadGlobalStates();
	qdGameSceneList const& lst = m_ptrGameDisp->scene_list();
	std::for_each(lst.begin(), lst.end(), InsertScene(m_wndAllStates));
}

//! Загружает состояния в списки состояний, уменьшающих/увеличивающих счетчик
void CounterPropPage::loadIncDecStates()
{
	std::for_each(m_elementsBackup.begin(), 
		m_elementsBackup.end(), InsertIncDecState(this));
}

//! Наращивает ли состояние счетчик
qdCounterElement * CounterPropPage::is_inc_state(qdGameObjectState const* pstate)
{
	qdCounterElement* pele = is_inc_or_dec_state(pstate);
	if (pele&&pele->increment_value())
		return pele;
	return NULL;
}
//! Уменьшает ли состояние счетчик
qdCounterElement * CounterPropPage::is_dec_state(qdGameObjectState const* pstate)
{
	qdCounterElement* pele = is_inc_or_dec_state(pstate);
	if (pele&&!pele->increment_value())
		return pele;
	return NULL;
}

//! Воздействет ли состояние на счетчик
CounterPropPage::ElementsContainer::iterator
	CounterPropPage::is_inc_or_dec_state_itr(ElementsContainer & lst,
			qdGameObjectState const* pstate)
{
	typedef boost::indirect_iterator<ElementsContainer::iterator,qdCounterElement> Itr_t;

	return std::find(Itr_t(lst.begin()), Itr_t(lst.end()), pstate).base();
}

//! Воздействет ли состояние на счетчик
qdCounterElement *
	CounterPropPage::is_inc_or_dec_state(qdGameObjectState const* pstate)
{

	ElementsContainer::iterator res = is_inc_or_dec_state_itr(m_elementsBackup, pstate);
	if (res != m_elementsBackup.end())
		return (*res).get();
	return NULL;
}

void CounterPropPage::addStates(eAddStates eAct)
{
	InsertIncDecState inserter(this);
	CTreeListItem *pitem = m_wndAllStates.GetSelectedItem();
	while (pitem&&m_wndAllStates.GetParentItem(pitem)) {
		qdGameObjectState* pstate = 
			reinterpret_cast<qdGameObjectState*>(m_wndAllStates.GetItemData(pitem));
		if (pstate&&!is_inc_or_dec_state(pstate))
		{
			boost::shared_ptr<qdCounterElement> 
				ele(new qdCounterElement(pstate, (eAct == AS_ADD_INC)));
			m_elementsBackup.push_back(ele);
			inserter(ele);
			if (eAct == AS_ADD_INC)
				m_wndAllStates.SetItemImage(pitem, 0, 0, 0, 0);
			else
				m_wndAllStates.SetItemImage(pitem, 1, 1, 1, 1);
		}
		pitem = m_wndAllStates.GetNextSelectedItem(pitem);
	}
}

void CounterPropPage::deleteStates(QETreeListCtrl& list)
{
	CTreeListItem *pitem = list.GetSelectedItem();
	while (pitem) {
		qdCounterElement* pele = 
			reinterpret_cast<qdCounterElement*>(list.GetItemData(pitem));

		ASSERT(is_inc_or_dec_state_itr(m_elementsBackup, pele->state()) 
			!= m_elementsBackup.end());
		
		reflectRemoveInTree(pele->state());
		m_elementsBackup.erase(is_inc_or_dec_state_itr(m_elementsBackup, pele->state()));

		list.DeleteItem(pitem);
		pitem = list.GetNextSelectedItem(pitem);
	}	
}

CTreeListItem* CounterPropPage::findTreeItemByText(QETreeListCtrl& tree, 
												   CTreeListItem* pParent,
												   LPCTSTR lpszText)
{
	CTreeListItem* pChild = tree.GetChildItem(pParent);
	while (pChild) {
		if (!_tcscmp(pChild->GetText(), lpszText))
			return pChild;
		pChild = tree.GetNextSiblingItem(pChild);
	}
	return NULL;
}

CTreeListItem* CounterPropPage::findTreeItemByData(QETreeListCtrl& tree, 
												   CTreeListItem* pParent,
												   DWORD_PTR data)
{
	CTreeListItem* pChild = tree.GetChildItem(pParent);
	while (pChild) {
		if (pChild->GetData() == data)
			return pChild;
		pChild = tree.GetNextSiblingItem(pChild);
	}
	return NULL;
}

CTreeListItem* CounterPropPage::findStateItem(qdGameObjectState const* pstate)
{
	if(!pstate)
		return 0;

	CTreeListItem* pGlobalStatesNode = m_wndAllStates.GetChildItem(TLI_ROOT);
#ifdef _DEBUG
	ASSERT(pGlobalStatesNode);
	CString str((LPTSTR)IDS_GLOBAL_STATES_NODE);
	ASSERT(!str.Compare(pGlobalStatesNode->GetText()));
#endif
	CTreeListItem* pStateItem = findTreeItemByData(m_wndAllStates, pGlobalStatesNode, 
				reinterpret_cast<DWORD_PTR>(pstate));
	if (pStateItem)
		return pStateItem;

	LPCTSTR const pSceneName = pstate->owner()->owner()->name();
	CTreeListItem* const pSceneItem = 
		findTreeItemByText(m_wndAllStates, TLI_ROOT, pSceneName);
	ASSERT(pSceneItem);
	pStateItem = findTreeItemByData(m_wndAllStates, pSceneItem, 
			reinterpret_cast<DWORD_PTR>(pstate));
	ASSERT(pStateItem);

	return pStateItem;
}

void CounterPropPage::reflectRemoveInTree(qdGameObjectState const* pstate)
{
	CTreeListItem* const pStateItem = findStateItem(pstate);
//	pStateItem->ShowTreeImage(FALSE);
	m_wndAllStates.ShowItemImage(pStateItem, false);
}

void CounterPropPage::OnBnClickedButtonAddInc()
{
	addStates(AS_ADD_INC);
	SetModified();
	m_wndIncStates.SetColumnWidth( 0, TLSCW_AUTOSIZE_USEHEADER );
	m_wndAllStates.Invalidate();
}

void CounterPropPage::OnBnClickedButtonDelInc()
{
	deleteStates(m_wndIncStates);
	SetModified();
	m_wndAllStates.Invalidate();
}

void CounterPropPage::OnBnClickedButtonAddDec()
{
	addStates(AS_ADD_DEC);
	SetModified();
	m_wndDecStates.SetColumnWidth( 0, TLSCW_AUTOSIZE_USEHEADER );
	m_wndAllStates.Invalidate();
}

void CounterPropPage::OnBnClickedButtonDelDec()
{
	deleteStates(m_wndDecStates);
	SetModified();
	m_wndAllStates.Invalidate();
}

BOOL CounterPropPage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == ::GetDlgItem(m_hWnd, IDC_EDIT_MAX_VALUE))
		{
			OnChangeLimitValue();
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}

void CounterPropPage::OnChangeLimitValue()
{
	if (!UpdateData())
		return;
	SetModified();
}
void CounterPropPage::OnDeltaposSpinMaxValue(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	SetModified();
	m_iLimitValue = pNMUpDown->iPos;
	*pResult = 0;
}

void CounterPropPage::OnBnClickedCheckOnlyPositive()
{
	SetModified();
}

//! Уведомляет о том, что курсор вошел в область окна
bool CounterPropPage::onDragEnter(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd)
{
//	if (source.m_hWnd == m_wndDecStates.m_hWnd)
	return false;
}
//! Уведомляет о том, что тянем курсор над окном
bool CounterPropPage::onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd)
{
	return false;
}
//! Уведомляет о том, что отжали кнопку мыши
bool CounterPropPage::onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd)
{
	return false;
}
