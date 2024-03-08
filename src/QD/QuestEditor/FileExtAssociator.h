// FileExtAssociator.h: interface for the FileExtAssociator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEEXTASSOCIATOR_H__67DE8CC6_6134_40A2_AE61_02131A1BCACB__INCLUDED_)
#define AFX_FILEEXTASSOCIATOR_H__67DE8CC6_6134_40A2_AE61_02131A1BCACB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>
/// В новой версии НЕ ИСПОЛЬЗУЕТСЯ. Класс прописывал в реестре...
/**
	В новой версии НЕ ИСПОЛЬЗУЕТСЯ

	Класс прописывал в реестре какие расширения принадлежат нашему приложению.
	Порядок действий был найден в какой-то статье из MSDN

	необходимо в таблице строк определить строки с такими идентификаторами
	IDS_APPS_SUB_KEY - имя exe-файла приложения 
	IDS_PROG_ID_REG_KEY	- а это загадочная строка. для своего приложения QuestEditor.exe
	я определил её как "QuestEditor.1"
	IDS_PROG_ID_KEY_VALUE - это значения для предыдущего ключа. Здесь находится описание
							того, что мы будем открывать
*/
class FileExtAssociator  
{
	UINT m_uiExtStrID;
public:
	FileExtAssociator();
	~FileExtAssociator();
	/// \brief Входная функция. Приниает индентификатор 
	/// строки с расширением, которое будем регистрировать
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
