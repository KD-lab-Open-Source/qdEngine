#pragma once
#include "namedobjecttreenode.h"

/// ����� ����, ��������������� ������� �����
class FontObjectTreeNode : public NamedObjectTreeNode
{
public:
	FontObjectTreeNode(qdNamedObject* object);
	~FontObjectTreeNode(void);

	bool load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
		ITreeNode * parent, qdNamedObjectBase const* owner,
		ITreeNode* insertAfter);
	//! ���������� ������������� ������� ���� ��� ����
	DWORD getMenuResId() const;

	bool showProperties(TreeLogic& logic, int iActive);
	virtual bool deleteObject(TreeLogic& logic, bool ask, bool statDepends);

	virtual bool canRename(TreeLogic& logic);
	virtual bool rename(TreeLogic& logic, LPCTSTR newName);
protected:
	/// ���������� true, ���� newName ���������
	bool nameTest(TreeLogic& logic, LPCSTR newName);
};
