#pragma	   once
#include <ShLwApi.h>

/**
	Реализация SelectAnimSoundMWPage::text_set, которая вызывает в set_text 
	set_def_anim у переданного ей типа, а в get_text anim_name. 
	Этим типом является класс, описывающий данные визарда. Функция set_def_anim 
	задает в нем(классе) имя анимации, anim_name - возвращает имя анимации
 */
template<class _WizData>
class AnimSet : public SelectAnimSoundMWPage::text_set
{
	/// Указатель на класс
	_WizData * pdata_;
	/// Нужна ли проверка на существование файла
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
	Реализация SelectAnimSoundMWPage::text_set, которая вызывает в set_text 
	set_def_sound у переданного ей типа, а в get_text sound_name. 
	Этим типом является класс, описывающий данные визарда. Функция set_def_sound 
	задает в нем(классе) имя звука, sound_name - возвращает имя звукового файла
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
	Реализация INameTester проверяющая уникальность имени состояния интерфейсной кнопки
 */
class interf_btn_state_name_test : public INameTester
{
	/// Подопытная кнопка
	class qdInterfaceButton* m_pButton;
	/// Проверяет не совпадает ли указанное имя с именем одного из состояний кнопки
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
	/// Возврщает уникальное имя
	virtual const CString get_unique_name(const CString& str) const
	{
		CString muster = str;
		int i = 0;
		//пытаемся получить уникальное имя
		while(!is_uniq(muster))
			muster.Format("%s%d", (LPCTSTR)str, ++i);
		return muster;
	}
};
#endif //defined __QD_INTERFACE_BUTTON_H__