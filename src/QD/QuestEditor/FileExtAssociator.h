// FileExtAssociator.h: interface for the FileExtAssociator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEEXTASSOCIATOR_H__67DE8CC6_6134_40A2_AE61_02131A1BCACB__INCLUDED_)
#define AFX_FILEEXTASSOCIATOR_H__67DE8CC6_6134_40A2_AE61_02131A1BCACB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>
/// � ����� ������ �� ������������. ����� ���������� � �������...
/**
	� ����� ������ �� ������������

	����� ���������� � ������� ����� ���������� ����������� ������ ����������.
	������� �������� ��� ������ � �����-�� ������ �� MSDN

	���������� � ������� ����� ���������� ������ � ������ ����������������
	IDS_APPS_SUB_KEY - ��� exe-����� ���������� 
	IDS_PROG_ID_REG_KEY	- � ��� ���������� ������. ��� ������ ���������� QuestEditor.exe
	� ��������� � ��� "QuestEditor.1"
	IDS_PROG_ID_KEY_VALUE - ��� �������� ��� ����������� �����. ����� ��������� ��������
							����, ��� �� ����� ���������
*/
class FileExtAssociator  
{
	UINT m_uiExtStrID;
public:
	FileExtAssociator();
	~FileExtAssociator();
	/// \brief ������� �������. �������� �������������� 
	/// ������ � �����������, ������� ����� ��������������
	bool DO(UINT uiExtStrID);
private:
	bool DefineFileClass(ATL::CRegKey* pKeyExt, ATL::CRegKey* pKeyProgID);
	bool DefineExtEntry(ATL::CRegKey* pKeyExt);
	bool DefineProgIDEntre(ATL::CRegKey* pKeyProgID);
	bool DefiningAttributes4FileClass(ATL::CRegKey* pKey);

	bool DefineAppKeyEntry();
	bool DefineShellSubKeys(ATL::CRegKey* pKey);
};

#endif //!defined(AFX_FILEEXTASSOCIATOR_H__67DE8CC6_6134_40A2_AE61_02131A1BCACB__INCLUDED_)
