#pragma once

class CListCtrl;
/// ��������� ����� ���������� � CListCtrl.
/**
	����������� ��������� � 
	NamedObjectSelectorDlg. � ���� � NamedObjectSelectorDlg ��������� ����� ��������, 
	��� ������������ ���� �����. �������� � ProjectJoiner::selectSceneForJoin ��� ����� 
	������������
 */
struct INamedObjListReader
{
	virtual bool read(CListCtrl& lst) = 0;
};