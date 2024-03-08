#pragma once

#include <TreeListCtrl/Null Listeners/NullCustomDrawNotifyListener.h>

/// ������������ ���� ������ ������������ ������� � �������
class TreeCustomDrawer : public NullCustomDrawNotifyListener
{
public:
	TreeCustomDrawer(void);
	~TreeCustomDrawer(void);

	/// ���������� ���� � �����, ������� ����� ���������� ����
	DWORD onItemPrepaint(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi);
	/// ���������� � DC ���������� ����
	DWORD onItemPostpaint(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi);
protected:
	/// \brief ���������� ����� ������ ����� ��������� ������������ ��� ������� ����
	/// � ������ �����
	void selectItemBack(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi);
	/// \brief ���������� ����� ������ ����� ��������� ������������ ��� ������� ����
	/// ��� ����� ���������
	void selectStateItemBack(qdGameObjectState* pstate, 
									ITEM_CUSTOM_DRAW_INFO* pcdi);
private:
	/// ���� ��� ��������� ������
	CFont font_;
	/// ���������� ����, ������� ���� ������������
	HGDIOBJ oldFont_;
};
