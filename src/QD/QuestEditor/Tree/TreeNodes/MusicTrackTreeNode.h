#pragma once
#include ".\conditionalobjecttreenode.h"

/// ���� ������������ �����
class MusicTrackTreeNode :
	public ConditionalObjectTreeNode
{
public:
	MusicTrackTreeNode(qdNamedObject* object);
	~MusicTrackTreeNode(void);
	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
						ITreeNode * parent, 
						qdNamedObjectBase const* owner,
						ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int iActive);
	bool onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);
	//! \brief ����������, ����� ����� ������� �������, ������������ � ������,
	// ��� ��� ��������(��������, ����� ��������� ������������� ������. 
	//��� �������, ����� ��������� �����)
	virtual bool deleteAsSubObject(TreeLogic& logic);

	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	void modifyMenu(TreeLogic& treeLogic, CMenu* pm) const;
	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCTSTR newName);
};
