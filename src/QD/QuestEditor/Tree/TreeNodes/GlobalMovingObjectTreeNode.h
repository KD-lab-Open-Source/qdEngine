#pragma once
#include "movingobjecttreenode.h"

/// ����� ����, �������������� ����������� ���������
class GlobalMovingObjectTreeNode :
	public MovingObjectTreeNode
{
public:
	GlobalMovingObjectTreeNode(qdNamedObject* object);
	~GlobalMovingObjectTreeNode(void);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
	bool showProperties(TreeLogic& logic, int iActive);

	virtual bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	virtual bool canDrag(TreeLogic& logic);
	virtual DROPEFFECT canDrop(TreeLogic& logic, ITreeNode* overNode);
	virtual DROPEFFECT onDropData(TreeLogic& logic, 
							ITreeNode* dropTarget, 
							DROPEFFECT dropEffect);
	//! ���������� true, ���� ������� ������������, false � ��������� ������
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// ������� ����� ����������� �������, � ������� ��� ���������
	/// ���������� true, ���� newName ���������
	bool removeEmptyCopyOfGlobal(TreeLogic& logic, qdGameObject* pobj);
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
	//! ��������������� ����� ����������� ����� � ������
	void renameCopies(TreeLogic& logic, LPCTSTR oldName, LPCTSTR newName);
};
