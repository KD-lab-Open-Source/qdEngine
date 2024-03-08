#pragma once


#include "conddlgmgr.h"
#include "mytree.h"
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "CondsMgr.h"

class qdConditionGroup;

/*!
 ������ ������� ��� qdConditionalObject �� ����������� qdGameObjectState
*/
// CO_ConditionDlg dialog
class CO_ConditionDlg : 
	private boost::base_from_member<CMyTree>
	, public CDialog
	, public MakeCondDlgBase
{
	DECLARE_DYNAMIC(CO_ConditionDlg);

public:
	CO_ConditionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CO_ConditionDlg();

	void set_data(class qdGameDispatcher* ptrDisp, class qdConditionalObject* ptrObj);
// Dialog Data
	enum { IDD = IDD_DLG_CO_CONDS };

	void OnCondDataComplete(bool bComplete);
	class qdGameScene* get_scene() const;
	class qdGameDispatcher* get_disp() const{
		return m_ptrGameDisp;
	};

	bool changed() const{
		return m_bChanged||m_CondsMgr.was_changed();
	}
private:
	VOID ShowCondDlg(qdCondition::ConditionType type);
	VOID LoadCondMode();
	void LoadConditions();
	void SetButtonsState();	

	void mg2wnd();
	void wnd2mg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCbnSelchangeComboCondition();
	afx_msg void OnBnClickedButtonAddCond();
	afx_msg void OnLbnSelchangeList();
	afx_msg void OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCreateOr();
	afx_msg void OnCreateAnd();
	afx_msg void OnChangeType();
	afx_msg void OnDelete();
	afx_msg void OnDeleteAll();
	afx_msg void OnRemoveFromGroup();
	afx_msg void OnCreateAndForAll();
	afx_msg void OnCreateOrForAll();

	//! Drag & drop � ������: ����� �� �������
	void OnCanDropReq(NMHDR* phdr, LRESULT* pResult);
	//! Drag & drop � ������: ����� �� ������
	void OnCanDragReq(NMHDR* phdr, LRESULT* pResult);
	//! Drag & drop � ������: �������
	void OnDropReq(NMHDR* phdr, LRESULT* pResult);

	//! ��������� ������ �������
	bool LoadGroups();
	//! ������ ������� � ������
	void ConditionToTree(const qdCondition* pcond, HTREEITEM hParent);
	//! ������ ������� ������� � ������
	void ObjConditionsToTree();
	HTREEITEM FindCondGroup(int iCondNum);
	void DeleteGroup(HTREEITEM hItem);
	void DeleteCondition(HTREEITEM hItem);
	//! �������� �� ���� �������
	bool is_group(HTREEITEM hItem);

	void TreeToCO();

	int GetCondIndex(const qdCondition* p);

	//! ����� �� �������� �������
	bool CanAdd();

	//! getter for m_bAddConditionOnClose
	bool getAddConditionOnClose() const{
		return m_bAddConditionOnClose;
	}
	//! setter for m_bAddConditionOnClose
	void setAddConditionOnClose(bool b){
		m_bAddConditionOnClose = b;
	}
protected:
	//! ��������� ��������� �������
	CCJFlatComboBox m_wndConds;
	//! ������ ����������� ��� ������� ����� ��� ������ ���� �� ���������
	BOOL m_iCondLog;
	//! ��������� ������� �������� ��� �������
	CondDlgMgr m_CDMgr;

	qdGameDispatcher* m_ptrGameDisp;
	qdConditionalObject* m_ptrSourceCO;

	bool m_bChanged;

	typedef std::map<HTREEITEM, boost::shared_ptr<qdConditionGroup> > GroupItems_t;
	GroupItems_t m_group_items;

	typedef std::vector<boost::shared_ptr<qdCondition> > Conditions;
	Conditions m_conditions;

	//! ������������ ������� � ������ � �������
	CondsMgr m_CondsMgr;

	//! ����� �� �������� ��������� ������� ��� �������� �������
	bool m_bAddConditionOnClose;
};
