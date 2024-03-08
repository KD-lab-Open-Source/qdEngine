#pragma once
#include "afxwin.h"

class CTreeListCtrl;

//������ �� �������� ������ ���������� ����� ����� GetWindowText
template<class Base>
class EditItemCtrlBase :public Base
{
public:
	EditItemCtrlBase(void):
	  m_pTreeListCtrl( NULL )
	  {}
	  virtual ~EditItemCtrlBase(void){}
protected:
	//! ������ ���� ������� �� ������������ ������
	/*!
		������������� ��������� �� TreeListCtrl
	*/
	void OnKillFocus(CWnd* pNewWnd)
	{
		Base::OnKillFocus(pNewWnd);

		m_pTreeListCtrl->SetCtrlFocus( pNewWnd, FALSE );
	}

protected:
	void setTreeListCtrl(CTreeListCtrl* p){
		m_pTreeListCtrl = p;
	}
	CTreeListCtrl* getTreeListCtrl(){
		return m_pTreeListCtrl;
	}
private:
	CTreeListCtrl*	m_pTreeListCtrl;
};
