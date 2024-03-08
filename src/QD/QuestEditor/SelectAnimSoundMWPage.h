#pragma once

#include "wizardbasepage.h"
// SelectAnimSoundMWPage dialog

/**
	Страничка выбора анимации и звука для элемента инетерфейса
 */
class SelectAnimSoundMWPage : public WizardBasePage
{
	DECLARE_DYNAMIC(SelectAnimSoundMWPage)

public:
	/// @brief Интерфейс назначения текста. Куда будет назначет текст определяет 
	/// конкретная реализация 
	class text_set
	{
	public:
		/// Назначает текст
		/**
			@return true - если переданный текст подходит
		 */
		virtual bool set_text(const CString& str) = 0;
		/// Возвращает текст
		virtual const CString& get_text() const = 0;
	};
	SelectAnimSoundMWPage();
	virtual ~SelectAnimSoundMWPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_SEL_ANIM_AND_SOUND };


	void set_anim_setter(text_set* pas);
	void set_sound_setter(text_set* pss);
	/// назначает заголовок станички
	void SetCaption(const CString& str);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	afx_msg void OnBnClickedBtnSelAnim();
	afx_msg void OnBnClickedBtnSelSound();
	afx_msg void OnEnChangeEditSound();
	afx_msg void OnEnChangeEditAnim();
private:
	/// Сеттер текста, назначающий имя файла анимации
	text_set* m_pAnimSetter;
	/// Сеттер текста, назначающий имя файла звука
	text_set* m_pSoundSetter;
	/// Имя файла анимации
	CString m_strAnimFile;
	/// Имя файла звука
	CString m_strSoundFile;
	/// Заголовок странички
	CString m_strCaption;
};
