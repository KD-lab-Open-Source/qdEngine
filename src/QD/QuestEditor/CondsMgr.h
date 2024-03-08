#pragma once
/********************************************************************
	created:	2003/04/24
	created:	24:4:2003   14:54
	filename: 	d:\Projects\Quest\QuestEditor\CondsMgr.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	CondsMgr
	file ext:	h
	author:		�����
	
	purpose:	������������ ������� � ������ � �� ������ � ������.
*********************************************************************/

#include "mytree.h"
#include <map>
#include <boost/shared_ptr.hpp>

class qdConditionalObject;
class qdConditionGroup;
class qdCondition;

/**
	������������ ������� � ������ � �� ������ � ������.
	��� ��� ��� ���������������� ����� ��� StateCondDlg � CO_ConditionDlg 
	��� ���� �������� � ��������� �����
 */
class CondsMgr
{
public:
	CondsMgr(CMyTree& tree);
	~CondsMgr();

	/// ���� �� ���������
	bool was_changed() const{
		return m_bChanged;
	}
	/// ��������� ������������� ������
	void set_obj(qdConditionalObject* pobj);

	/// ������� ������
	HTREEITEM CreateGroup(qdConditionGroup::conditions_mode_t mode);
	/// \brief ��� ������� ��������� ������� ������� ������ � ��������� ������� 
	/// � ���������� ������� � ��� ������
	void CreateGroups(qdConditionGroup::conditions_mode_t mode);

	/// ��������� ������� � ������ ������� � � ������
	/**
		���� ������� ������, �� ��������� � ������, ����� ������ � ������ 
	 */
	bool AddCondition(qdCondition const* pcond);
	/// ������� ���������� ������� �� ������
	void RemoveFromGroup();

	/// ������ ��� � � �� ��� � �������� � ���������� �����
	void InvertType();
	/// ������ ��� � � �� ��� � �������� ��� ����
	void InvertType(HTREEITEM hItem);

	/// ��������� �������� �� ���� ����� ������
	bool IsGroup(HTREEITEM hItem)const;

	//! ������ � ������� qdCoditionObject
	void TreeToCO();
	//! ������� � ������
	void CondsToTree();
	//! �������� �����
	bool LoadGroups();

	/// ������� ��� ��������� �������
	void DeleteSelected();
	/// ������� ���
	void DeleteAll();


	// Drag&Drop

	LRESULT OnCanDropReq(NM_MT_CAN_DROP* pcd);
	LRESULT OnCanDragReq(NM_MT_CAN_DRAG* pcd);
	LRESULT OnDropReq(NM_MT_DROP* pmd);

	LRESULT OnNMRclickTree(CWnd* from);
protected:
	/// ������ ��� � � �� ��� � �������� ��� ������
	void InvertType(qdConditionGroup& cg);

	/// ��������� �� ������ ���������� �����
	void ReadGroupsFromTree();
	/// ��������� �� ������ ���������� ���������� ������(hItem - ���� ������)
	void ReadGroupFromTree(HTREEITEM hItem);

	/// ������ ������� � ������ �������
	int GetCondIndex(const qdCondition* p);

	/// ������� ������ �� ������ � �� �������.
	/**
		��� ������� ����������� � ������ ������
	 */
	void DeleteGroup(HTREEITEM hItem);
	/// ������� ������� �� ������ � �� ������
	void DeleteCondition(HTREEITEM hItem);
private:
	/// ��������� ������� � ������ � � ������ ������� m_conditions
	void ConditionToTree(const qdCondition* pcond, 
						HTREEITEM hParent,
						HTREEITEM hInsertAfter = TVI_LAST);
	/// ���������� ���� ������ ������ ���������� �������
	HTREEITEM FindCondGroup(int iCondNum);
	/// �� ������ ���������� ��������� ������
	CString GetGroupCaption(qdConditionGroup::conditions_mode_t mode);
private:
	/// ������� ����������� ������
	CMyTree& m_tree;
	/// ������������� ������
	qdConditionalObject* m_ptrObj;
	/// ��������� �� ���������
	bool m_bChanged;

	typedef std::map<HTREEITEM, boost::shared_ptr<qdConditionGroup> > GroupItems_t;
	/// ������ �����
	GroupItems_t m_group_items;

	typedef std::vector<boost::shared_ptr<qdCondition> > Conditions;
	/// ������ �������
	Conditions m_conditions;
};
