#pragma once
//#include "gameobjecttreenode.h"
#include "animatedobjecttreenode.h"

/// ����� ����, �������������� ���������
class MovingObjectTreeNode : public AnimatedObjectTreeNode
{
public:
	MovingObjectTreeNode(qdNamedObject* object);
	~MovingObjectTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	bool reload(ITreeNodeFactory* factory, TreeLogic& logic, 
				ITreeNode const* parent, qdNamedObjectBase const* owner);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief ����������, ����� ����� ������� �������, ������������ � ������,
	// ��� ��� ��������(��������, ����� ��������� ������������� ������. 
	//��� �������, ����� ��������� �����)
	virtual bool deleteAsSubObject(TreeLogic& logic);

	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, ITreeNode* dropTarget, 
		DROPEFFECT dropEffect);
	virtual void updateItemImage(TreeLogic& treeLogic);
protected:
	class InsertState;
	/// \brief ������� ����� ��������� � �������� 
	/// loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic,
	/// ITreeNode* staticStates, ITreeNode* walkStates) ��� �������� ���������
	bool loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic) ;
	/// ��������� ���������, ������������ �� �� ��������������� ������
	bool loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic,
					ITreeNode* staticStates, ITreeNode* walkStates) ;
	/// ��������������� ������� ��� �������
	static void renameScale(qdNamedObject const* obj, LPCTSTR oldName, LPCTSTR newName);
	/// �������������� ����� � ��� �������
	static bool doRename(TreeLogic& logic, qdNamedObject* object, LPCTSTR newName);
};
