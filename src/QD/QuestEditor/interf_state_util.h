#pragma	   once
#include <ShLwApi.h>

/**
	���������� SelectAnimSoundMWPage::text_set, ������� �������� � set_text 
	set_def_anim � ����������� �� ����, � � get_text anim_name. 
	���� ����� �������� �����, ����������� ������ �������. ������� set_def_anim 
	������ � ���(������) ��� ��������, anim_name - ���������� ��� ��������
 */
template<class _WizData>
class AnimSet : public SelectAnimSoundMWPage::text_set
{
	/// ��������� �� �����
	_WizData * pdata_;
	/// ����� �� �������� �� ������������� �����
	bool bStrikt_;
public:
	AnimSet(_WizData* pdata, bool bStrikt = true):pdata_(pdata), bStrikt_(bStrikt){}
	bool set_text(const CString& str){
		if ((!bStrikt_&&str.IsEmpty())||PathFileExists(str))
		{
			pdata_->set_def_anim(str);
			return true;
		}
		return false;
	}
	const CString& get_text()const {
		return pdata_->anim_name();
	}
};

/**
	���������� SelectAnimSoundMWPage::text_set, ������� �������� � set_text 
	set_def_sound � ����������� �� ����, � � get_text sound_name. 
	���� ����� �������� �����, ����������� ������ �������. ������� set_def_sound 
	������ � ���(������) ��� �����, sound_name - ���������� ��� ��������� �����
 */

template<class _WizData>
class SoundSet : public SelectAnimSoundMWPage::text_set
{
	_WizData * pdata_;
public:
	SoundSet(_WizData* pdata):pdata_(pdata){}
	bool set_text(const CString& str){
		if (str.IsEmpty()||PathFileExists(str)){
			pdata_->set_def_sound(str);
			return true;
		}
		return false;
	}
	const CString& get_text()const {
		return pdata_->sound_name();
	}
};

#if defined __QD_INTERFACE_BUTTON_H__
/**
	���������� INameTester ����������� ������������ ����� ��������� ������������ ������
 */
class interf_btn_state_name_test : public INameTester
{
	/// ���������� ������
	class qdInterfaceButton* m_pButton;
	/// ��������� �� ��������� �� ��������� ��� � ������ ������ �� ��������� ������
	bool is_uniq(LPCTSTR name) const{
		int const num_states = m_pButton->num_states();
		for(int i = 0; i < num_states; ++i)
			if (!_tcscmp(name, m_pButton->get_state(i)->name()))
				return false;
		return true;
	}
public:
	explicit interf_btn_state_name_test(qdInterfaceButton* pButton):m_pButton(pButton){}
	virtual ~interf_btn_state_name_test(){};
	/// ��������� ���������� ���
	virtual const CString get_unique_name(const CString& str) const
	{
		CString muster = str;
		int i = 0;
		//�������� �������� ���������� ���
		while(!is_uniq(muster))
			muster.Format("%s%d", (LPCTSTR)str, ++i);
		return muster;
	}
};
#endif //defined __QD_INTERFACE_BUTTON_H__