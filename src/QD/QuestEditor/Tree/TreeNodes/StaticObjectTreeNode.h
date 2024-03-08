#pragma once
#include "gameobjecttreenode.h"

/// ���� ������������ �������
class StaticObjectTreeNode : public GameObjectTreeNode
{
public:
	StaticObjectTreeNode(qdNamedObject* object);
	~StaticObjectTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, ITreeNode * parent, 
		qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	bool showProperties(TreeLogic& logic, int iActive);
	virtual void updateItemImage(TreeLogic& treeLogic);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
protected:
	/// ������ �������� ��� ����� ������� � ������
	static int selectPicture(qdNamedObject const* object);

	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
};
