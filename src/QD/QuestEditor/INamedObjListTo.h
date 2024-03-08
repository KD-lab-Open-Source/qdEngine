#pragma once

class CListCtrl;
/**
	��������� ����� ���������� � CListCtrl. ����������� ��������� � 
	NamedObjectSelectorDlg. � ���� � NamedObjectSelectorDlg ��������� ����� ��������, 
	��� ������������ ���� �����. �������� � ProjectJoiner::selectSceneForJoin ��� ����� 
	������������
 */
interface INamedObjListToList
{
	virtual bool load(CListCtrl& lst) const = 0;
};