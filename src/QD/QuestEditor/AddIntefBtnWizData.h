#pragma once
#include "mwdatabase.h"

class qdInterfaceScreen;

/**
	Данные визарда добавляющего кнопки
 */
class AddInterfBtnWizData :
	public MWDataBase
{
	/// Экран, в который будет добавлена кнопка
	qdInterfaceScreen* m_pScreen;
	/// Имя кнопки
	CString m_strName;
	/// Имя состояния
	CString m_strStateName;
	/// Имя анимации для всех резимов
	CString m_strDefAnim;
	/// Имя звука для всех резимов
	CString m_strDefSound;
public:

	explicit AddInterfBtnWizData(qdInterfaceScreen* pscr):m_pScreen(pscr)
	{
	}

	~AddInterfBtnWizData(void)
	{
	}

	const CString& name() const {
		return m_strName;
	}
	void set_name(const CString& name){
		m_strName = name;
	}

	const CString& state_name() const {
		return m_strStateName;
	}
	void set_state_name(const CString& str) {
		m_strStateName = str;
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
