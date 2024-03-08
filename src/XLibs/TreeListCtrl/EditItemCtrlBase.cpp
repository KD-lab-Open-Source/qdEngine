#include "StdAfx.h"
#include "edititemctrlbase.h"
#include "TreeListDC.h"
#include "TreeListHeaderCtrl.h"
#include "TreeListTipCtrl.h"
#include "TreeListStaticCtrl.h"
#include "TreeListEditCtrl.h"
#include "TreeListComboCtrl.h"
#include "TreeListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


template<class Base>
EditItemCtrlBase<Base>::EditItemCtrlBase(void):
m_pTreeListCtrl( NULL )
{
}

template<class Base>
EditItemCtrlBase<Base>::~EditItemCtrlBase(void)
{
}

template<class Base>
void EditItemCtrlBase<Base>::setTreeListCtrl(CTreeListCtrl* p){
	m_pTreeListCtrl = p;
}

/////////////////////////////////////////////////////////////////////////////
// EditItemCtrlBase message handlers
template<class Base>
int EditItemCtrlBase<Base>::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (Base::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pTreeListCtrl = reinterpret_cast<CTreeListCtrl*>(GetParent());

	return 0;
}

template<class Base>
void EditItemCtrlBase<Base>::OnKillFocus(CWnd* pNewWnd) 
{
	Base::OnKillFocus(pNewWnd);

	m_pTreeListCtrl->SetCtrlFocus( pNewWnd, FALSE );
}
