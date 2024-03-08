#pragma once
#include "mwdatabase.h"

class qdInterfaceScreen;
/**
	Данные для визарда, добавляющего сейв в экран
 */
class AddInterfSaveWizData : public MWDataBase
{
	/// Экран, в который сейв будет добавлен
	qdInterfaceScreen* m_pScreen;
	/// Имя
	CString m_strName;
	/// Имя анимации
	CString m_strDefAnim;
	/// Имя звука
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