#pragma once
#include "mwdatabase.h"
#include "Tree/ITreeNode.h"

/**
	Данные для визарда, добавляющего "послений экран"
 */
class AddGameEndWizData : public MWDataBase
{
	/// Имя экрана
	/**
		По умолчанию имя "последнего экрана" такое же как и у интерфесного экрана
	 */
	CString m_strName;
	/// Имя интерефесного экрана
	CString m_strScrName;
	/// Родительский узел в новом дереве
	ITreeNode* parent_;
	/// Родительский узел в старом дереве. Сохранено, чтобы можно было скомпелировать
	HTREEITEM m_hParent;
public:
	explicit AddGameEndWizData(ITreeNode* parent):parent_(parent){};
	explicit AddGameEndWizData(HTREEITEM hParent):m_hParent(hParent){};
	
	CString const& name() const{
		return m_strName;
	}
	void set_name(CString const& s){
		m_strName = s;
	}
	CString const& scrName() const{
		return m_strScrName;
	}
	void setScrName(CString const& s){
		m_strScrName = s;
	}
#ifdef USE_NEW_PROJ
	ITreeNode* parent() const{
		return parent_;
	}
#else
	HTREEITEM parent() const{
		return m_hParent;
	}
#endif // USE_NEW_PROJ
};