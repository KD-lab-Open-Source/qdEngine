#pragma once

interface ITreeNode;
interface ITreeNodeFactory;

class qdNamedObject;
class qdInterfaceObjectBase;

class TreeLogic;

#include "Main/QETreeListCtrl.h"

enum ContentClassificator{
	CC_FOLDER,///< ����, �� ���������� �� ����� ������
	CC_NAMED_OBJECT, ///< ����, ��������� �� ������ ������� qdNamedObject
	CC_INTERFACE_OBJECT///< ����, ��������� �� ������ ������� qdInterfaceObjectBase
};

enum eNodeState{
	NS_SELECTED = 0x01,
	NS_FOCUSED	= 0x02,
};
/// ��� �������� ���� � ����� ������. 
/**
	���� ������������ � ����������� �� �������������� ContentClassificator
 */
typedef union tagNodeContent{
	DWORD folderId;
	qdNamedObject* gameObject;
	qdInterfaceObjectBase* interfaceObject;
}NodeContent;

//! ��������� ���� ������
ITreeNode* const TN_INSERT_FIRST = (ITreeNode*)0xFFFFFFFF;
//! ��������� ���� ���������
ITreeNode* const TN_INSERT_LAST	= (ITreeNode*)0xFFFFFFFE;
//! ��������� ���� ������������
ITreeNode* const TN_INSERT_SORT	= (ITreeNode*)0xFFFFFFFC;
//! �������� ����
ITreeNode* const TN_ROOT = (ITreeNode*)0xFFFFFFFB;

/// ������� ��������� ��� ����� ������
struct ITreeNode
{
	virtual ~ITreeNode() = 0;
	virtual CTreeListItem* getTreeListItem() const = 0;

	//! ��� ��������� � ����
	virtual ContentClassificator getContentClassificator() const = 0;
	//! ���������� ��, ��� ��������� � ����
	virtual NodeContent getContent() const = 0;
	//! �������� ���� � ������
	/*!
		������ ���������� \a false,���� �� ������� �������� � ������ ����. 
		���� �� ������� ��������� ��� ����, ������������ true
	*/
	virtual bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
					ITreeNode* parent, qdNamedObjectBase const* owner, 
					ITreeNode* insertAfter = TN_INSERT_LAST) = 0;
	//! ��������� ����������� �����
	/*!
		parent � owner ����������� ������� NULL, ���� node ��� ��� ��������.
	*/
	virtual bool reload(ITreeNodeFactory* factory, TreeLogic& logic,
							ITreeNode const* parent, 
							qdNamedObjectBase const* owner) = 0;

	//! ���������� �� �������� ���� �� ������
	virtual void onDeleteItem(TreeLogic& treeLogic) = 0;

	/// ������������ ������� "���� ������ �������� ����"
	virtual void onRClick(TreeLogic& treeLogic) = 0;

	virtual bool showProperties(TreeLogic& treeLogic, int activePage) = 0;
	virtual bool showConditions(TreeLogic& logic) = 0;

	//! ��������� ����������� ����� � ����� � ������
	virtual void updateItemImage(TreeLogic& treeLogic) = 0;

	//! �� ��������� ��������� ���� � ������
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState) = 0;
	//! �� ��������� ��������� �������� � ������ �� ������ ����
	virtual bool onCheckNode(TreeLogic& logic, bool bCheck) = 0;
	//! ������� ������ ������, � �������� �������� ����. ask ���������� �� 
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends) = 0;
	//! \brief ����������, ����� ����� ������� �������, ������������ � ������,
	/// ��� ��� ��������(��������, ����� ��������� ������������� ������. 
	/// ��� �������, ����� ��������� �����)
	virtual bool deleteAsSubObject(TreeLogic& logic) = 0;

	//! ����� �� ������������ ����
	virtual bool canDrag(TreeLogic& logic) = 0;
	//! ����� �� ������ ���� �� ���, ��� ������� �����
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode) = 0;
	//! ���� ������� �� dropTarget 
	virtual DROPEFFECT onDropData(TreeLogic& logic, 
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect) = 0;
	//! ���������� true, ���� ������� ������������, false � ��������� ������
	virtual bool rename(TreeLogic& logic, LPCTSTR newName) = 0;
	//! ����� �� ������� ��� ����
	virtual bool canRename(TreeLogic& logic) = 0;
};


