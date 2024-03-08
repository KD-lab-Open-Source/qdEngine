#pragma once
#include "treenodestube.h"

/// ������� ����� ���� ����� ��������
/**
	�������� ���� ����� ������������� ����, ������� �� ������������� �������
	������. ��� �� ����, ������� ���� ���������� �����������. ��� �� ��������
	���������� �������� �������� ������.
	����� ����� ����� ����������� ���������������. ������ ��� ��������������
	������� ������������ ����.
 */
class FolderTreeNode : public TreeNodeStube
{
public:
	FolderTreeNode(DWORD folderId);
	~FolderTreeNode();
	/// ���������� id �����.
	DWORD getFolderId() const;
	ContentClassificator getContentClassificator() const;
	virtual void onDeleteItem(TreeLogic& logic);
	NodeContent getContent() const;
protected:
private:
	/// ������������� �����
	/**
		������ ������������� ����� id ��� ��������, ������� ���������� � ��������.
		��� ����� ��������� ��������� ������������� ��������� ��������������
	 */
	DWORD folderId_;
};
