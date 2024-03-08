#pragma once
#include "gameobjecttreenode.h"

class AnimatedObjectTreeNode : public GameObjectTreeNode
{
public:
	AnimatedObjectTreeNode(qdNamedObject* object);
	~AnimatedObjectTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
				ITreeNode* parent, qdNamedObjectBase const* owner,
				ITreeNode* insertAfter);
	bool reload(ITreeNodeFactory* factory, TreeLogic& logic, 
				ITreeNode const* parent, qdNamedObjectBase const* owner);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int iActive);
	virtual void updateItemImage(TreeLogic& treeLogic);
	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);

	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief ����������, ����� ����� ������� �������, ������������ � ������,
	// ��� ��� ��������(��������, ����� ��������� ������������� ������. 
	//��� �������, ����� ��������� �����)
	virtual bool deleteAsSubObject(TreeLogic& logic);
	virtual bool canDrag(TreeLogic& logic);
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, 
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect);
	//! ���������� true, ���� ������� ������������, false � ��������� ������
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	bool onSelectInSceneView(TreeLogic& logic, bool bSelect);
	bool onSelectInTriggerView(TreeLogic& logic, bool bSelect);
	class InsertState;

	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;

	bool loadStates(ITreeNodeFactory* factory, TreeLogic& treeLogic);

	static int selectPicture(qdNamedObject const* object);
	static void updateTriggerElementTitles(qdGameDispatcher const* gameDisp, 
		qdGameObjectAnimated const* object);

	//! ������� ��� ��������� �������
	void deleteStates(TreeLogic& logic);
};
