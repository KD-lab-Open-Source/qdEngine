#pragma once
#include "mwdatabase.h"

/**
	Данные для визарда добавления слайдера
 */
class AddSldrData : public MWDataBase
{
	/// Экран, в который слайдер добавим
	class qdInterfaceScreen* m_pScreen;
	/// Имя слайдера
	CString m_strName;
	/// имя файла анимации бегунка
	CString m_strForgrAnimFile;
	/// Имя файла звука для бегунка
	CString m_strForgrSoundFile;
	/// Имя файла фона слайдера
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
