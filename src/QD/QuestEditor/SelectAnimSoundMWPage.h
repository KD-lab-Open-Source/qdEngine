#pragma once

#include "wizardbasepage.h"
// SelectAnimSoundMWPage dialog

/**
	��������� ������ �������� � ����� ��� �������� �����������
 */
class SelectAnimSoundMWPage : public WizardBasePage
{
	DECLARE_DYNAMIC(SelectAnimSoundMWPage)

public:
	/// @brief ��������� ���������� ������. ���� ����� �������� ����� ���������� 
	/// ���������� ���������� 
	class text_set
	{
	public:
		/// ��������� �����
		/**
			@return true - ���� ���������� ����� ��������
		 */
		virtual bool set_text(const CString& str) = 0;
		/// ���������� �����
		virtual const CString& get_text() const = 0;
	};
	SelectAnimSoundMWPage();
	virtual ~SelectAnimSoundMWPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_SEL_ANIM_AND_SOUND };


	void set_anim_setter(text_set* pas);
	void set_sound_setter(text_set* pss);
	/// ��������� ��������� ��������
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
	/// ������ ������, ����������� ��� ����� ��������
	text_set* m_pAnimSetter;
	/// ������ ������, ����������� ��� ����� �����
	text_set* m_pSoundSetter;
	/// ��� ����� ��������
	CString m_strAnimFile;
	/// ��� ����� �����
	CString m_strSoundFile;
	/// ��������� ���������
	CString m_strCaption;
};
