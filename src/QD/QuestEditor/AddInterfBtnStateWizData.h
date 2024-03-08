#pragma once
#include "mwdatabase.h"

/**
	������ ��� �������, ������������ ��������� ������
 */
class AddInterfBtnStateWizData :
	public MWDataBase
{
	/// ��� ���������
	CString m_strName;
	/// ��� ����� �������� ��� ���� ������� ���������
	CString m_strDefAnim;
	/// ��� ����� ����� ��� ���� ������� ���������
	CString m_strDefSound;
	/// ������, ������� ��������� ����� ���������
	qdInterfaceButton* pbtn_;
public:

	explicit AddInterfBtnStateWizData(qdInterfaceButton* pbtn):pbtn_(pbtn)
	{
	}

	const CString& name() const {
		return m_strName;
	}
	void set_name(const CString& name){
		m_strName = name;
	}
	const CString& anim_name() const{
		return m_strDefAnim;
	}
	void set_def_anim(const CString& str){
		m_strDefAnim = str;
	}

	const CString& sound_name() const{
		return m_strDefSound;
	}
	void set_def_sound(const CString& str){
		m_strDefSound = str;
	}
	qdInterfaceButton* button() const{
		return pbtn_;
	}
};
