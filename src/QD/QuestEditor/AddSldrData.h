#pragma once
#include "mwdatabase.h"

/**
	������ ��� ������� ���������� ��������
 */
class AddSldrData : public MWDataBase
{
	/// �����, � ������� ������� �������
	class qdInterfaceScreen* m_pScreen;
	/// ��� ��������
	CString m_strName;
	/// ��� ����� �������� �������
	CString m_strForgrAnimFile;
	/// ��� ����� ����� ��� �������
	CString m_strForgrSoundFile;
	/// ��� ����� ���� ��������
	CString m_strBackAnimFile;
public:
	explicit AddSldrData(qdInterfaceScreen* pscr):m_pScreen(pscr){}
	qdInterfaceScreen* screen() const{
		return m_pScreen;
	}

	const CString& name() const{
		return m_strName;
	}
	void set_name(const CString& str){
		m_strName = str;
		change();
	}
	const CString& foregr_anim_file() const{
		return m_strForgrAnimFile;
	}
	
	void set_foregr_anim_file(const CString& str){
		m_strForgrAnimFile = str;
		change();
	}

	const CString& foregr_sound_file() const{
		return m_strForgrSoundFile;
	}
	void set_foregr_sound_file(const CString& str){
		m_strForgrSoundFile = str;
		change();
	}

	const CString& back_anim_file() const{
		return m_strBackAnimFile;
	}
	void set_back_anim_file(const CString& str) {
		m_strBackAnimFile = str;
		change();
	}
};
