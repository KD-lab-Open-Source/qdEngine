#pragma once
#include "mwdatabase.h"
#include "Tree/ITreeNode.h"

/**
	������ ��� �������, ������������ "�������� �����"
 */
class AddGameEndWizData : public MWDataBase
{
	/// ��� ������
	/**
		�� ��������� ��� "���������� ������" ����� �� ��� � � ������������ ������
	 */
	CString m_strName;
	/// ��� ������������� ������
	CString m_strScrName;
	/// ������������ ���� � ����� ������
	ITreeNode* parent_;
	/// ������������ ���� � ������ ������. ���������, ����� ����� ���� ��������������
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