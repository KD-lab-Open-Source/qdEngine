#pragma once
#include "mwdatabase.h"

class qdInterfaceScreen;
/**
	������ ��� �������, ������������ ���� � �����
 */
class AddInterfSaveWizData : public MWDataBase
{
	/// �����, � ������� ���� ����� ��������
	qdInterfaceScreen* m_pScreen;
	/// ���
	CString m_strName;
	/// ��� ��������
	CString m_strDefAnim;
	/// ��� �����
	CString m_strDefSound;
public:
	explicit AddInterfSaveWizData(qdInterfaceScreen* pscr):m_pScreen(pscr){}

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
	qdInterfaceScreen* screen() const{
		return m_pScreen;
	}
};