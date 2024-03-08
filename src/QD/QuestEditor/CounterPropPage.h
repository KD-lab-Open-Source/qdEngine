#pragma once


#include "BasePage.h"
#include "Main/QETreeListCtrl.h"
#include <TreeListCtrl/Null Listeners/NullItemDragdropNotifyListener.h>
#include "qd_counter.h"
#include <boost/shared_ptr.hpp>
// CounterPropPage dialog

class qdGameDispatcher;
class qdCounter;

/*!
	����� �������� ������� ��������
*/

class CounterPropPage : public CBasePage
					  , public NullItemDragdropNotifyListener
{
	DECLARE_DYNAMIC(CounterPropPage)

	typedef std::vector<boost::shared_ptr<qdCounterElement> > ElementsContainer;
public:
	CounterPropPage();
	virtual ~CounterPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_COUNTER };

	//! ��������� ��������� ���� � �������
	void set_data(qdGameDispatcher* ptrDisp, qdCounter* ptrCounter);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//! ���������� ������� "���������"
	virtual BOOL OnApply();
	//! ������������� �������
	virtual BOOL OnInitDialog();
	//! ���������� ������� "��������"
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
private:
	friend class eleCopy;
	
	/*!
	    �������. �������� qdCounterelement � ���������� ��� � ���������
	*/
	class eleCopy
	{
		ElementsContainer &m_cont;
	public:
		eleCopy(ElementsContainer& cont);
		void operator()(qdCounterElement const& pce);
	};

	/*!
	    �������. ��������� ����� � ����� � � ��������� � ������
	*/
	friend class InsertScene;
	class InsertScene
	{
		QETreeListCtrl& m_wndTree;
	public:
		InsertScene(QETreeListCtrl& tree);
		void operator()(class qdGameScene const* pscene);
	};
	/*!
	    �������. ��������� ��������� ������� � ������
	*/
	friend class InsertObject;
	class InsertObject
	{
		QETreeListCtrl& m_wndTree;
		CTreeListItem *m_ptrParent;
	public:
		InsertObject(QETreeListCtrl& tree, CTreeListItem * pParent);
		void operator()(class qdGameObject const* pobj);
	};

	/*!
	    �������. ��������� ��������� � ������
	*/
	friend class InsertState;
	class InsertState
	{
		QETreeListCtrl& m_wndTree;
		CTreeListItem *m_ptrParent;
		bool m_bLoadGlobal;
	public:
		InsertState(QETreeListCtrl& tree, 
			CTreeListItem * pParent, bool bLoadGlobal);
		void operator()(class qdGameObjectState const* pstate);
	};

	/*!
	    ��������� ��������� � ������ ���������, ������� ���������� ��� ���������
		�������
	*/
	friend class InsertIncDecState;
	class InsertIncDecState
	{
		CounterPropPage* m_pOwnerPage;
	private:
		//! ��������� ��������� �� ��������� � ��������� ������
		void AddItem2List(QETreeListCtrl& lst, qdCounterElement const* pele);
	public:
		InsertIncDecState(CounterPropPage* page);
		//! \brief ���������� ���� ���� ��������� ���������.
		//! ���������� �������� ��� ��������� � ����� ������
		void operator()(boost::shared_ptr<qdCounterElement>& pele);
	};
private:
	//! ���������� ����� ��� ������� �����������/������������ ���������
	void setTreeListCtrlListStyles(QETreeListCtrl& ctrl);
	//! ���������� ����� ��� ������ ������ ���������
	void setTreeListCtrlTreeStyles(QETreeListCtrl& ctrl);
	//! �������� ������� �������� � ����������� ������
	void real2back();
	//! �������� ������ �� ������������ ������ � ������.������������� ���������� ��������
	void back2real();
	//! ��������� � ������ ����� � ����������� ��������, ������������� �����
	void loadAllStates();
	//! ��������� ��������� � ������ ���������, �����������/������������� �������
	void loadIncDecStates();
	//! ���������� �� ��������� �������
	qdCounterElement* is_inc_state(qdGameObjectState const* pstate);
	//! ��������� �� ��������� �������
	qdCounterElement* is_dec_state(qdGameObjectState const* pstate);
	//! ����������� �� ��������� �� �������
	qdCounterElement* is_inc_or_dec_state(qdGameObjectState const* pstate);
	//! ����������� �� ��������� �� �������
	static ElementsContainer::iterator 
		is_inc_or_dec_state_itr(ElementsContainer & lst,
									qdGameObjectState const* pstate);

	enum eAddStates{AS_ADD_INC, AS_ADD_DEC};
	//! ��������� ��������� �� ������ � ��������������� ������
	void addStates(eAddStates eAct);
	//! ������� �� ������ � �������� �������� � ������
	void deleteStates(QETreeListCtrl& list);
	//! �������� �������� ��������� �� ������-���� ������
	void reflectRemoveInTree(qdGameObjectState const* pstate);
	//! ������� ���� ��������� � ������ ���������
	CTreeListItem* findStateItem(qdGameObjectState const* pstate);
	//! ������� ���� � ������ �� ������
	CTreeListItem* findTreeItemByText(QETreeListCtrl& tree, 
		CTreeListItem* pParent, LPCTSTR lpszText);
	//! ���� ���� �� �������� Item Data
	CTreeListItem* findTreeItemByData(QETreeListCtrl& tree, 
		CTreeListItem* pParent, DWORD_PTR lpszText);
	//! ����������� ������ ������� ������ �� ������ ��������
	void _1stcolumnWidth2ClientWidth(QETreeListCtrl& lst);
	//! ��������� ���������� ��������� � ������
	void loadGlobalStates();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	afx_msg void OnBnClickedCheckOnlyPositive();
	//! �������� �� ������ �������� ������������ ���������
	afx_msg void OnBnClickedButtonAddInc();
	//! �������� �� ������ ������� ������������ ���������
	afx_msg void OnBnClickedButtonDelInc();
	//! �������� �� ������ �������� ����������� ���������
	afx_msg void OnBnClickedButtonAddDec();
	//! �������� �� ������ ������� ����������� ���������
	afx_msg void OnBnClickedButtonDelDec();

	//! ���������� � ���, ��� ������ ����� � ������� ����
	virtual bool onDragEnter(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! ���������� � ���, ��� ����� ������ ��� �����
	virtual bool onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! ���������� � ���, ��� ������ ������ ����
	virtual bool onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);

	afx_msg void OnDeltaposSpinMaxValue(NMHDR *pNMHDR, LRESULT *pResult);
	void OnChangeLimitValue();

	QETreeListCtrl& getIncStates(){
		return m_wndIncStates;
	}
	QETreeListCtrl& getDecStates(){
		return m_wndDecStates;
	}
	QETreeListCtrl& getAllStates(){
		return m_wndAllStates;
	}
private:
	//! ��� ��������� �������
	QETreeListCtrl m_wndAllStates;
	//! ���������, ������� ���������� �������
	QETreeListCtrl m_wndIncStates;
	//! ���������, ������� ��������� �������
	QETreeListCtrl m_wndDecStates;

	//! ����������� ������ ���������
	ElementsContainer m_elementsBackup;

	//! ���������� �������
	qdCounter* m_ptrCounter;
	//! ��������� ����
	qdGameDispatcher* m_ptrGameDisp;

	CImageList m_treeImages;

	int m_iLimitValue;
	//! ������� ��������� ������ ������������� ��������
	BOOL m_bOnlyPositive;
};
