#pragma once

#include "multiwizard\mwwizard.h"
#include <boost/scoped_ptr.hpp>

class SelectNameMWPage;
typedef boost::scoped_ptr<SelectNameMWPage> SelectNameMWPagePtr;

class SelectAnimSoundMWPage;
typedef boost::scoped_ptr<SelectAnimSoundMWPage> SelectAnimSoundMWPagePtr;

class AddIEleDonePage;
typedef boost::scoped_ptr<AddIEleDonePage> AddIEleDonePagePtr;

class AddInterfBtnStateWizData;
typedef boost::scoped_ptr<AddInterfBtnStateWizData> AddInterfBtnStateWizDataPtr;

template<class _WizData> class AnimSet;
typedef AnimSet<AddInterfBtnStateWizData> StateAnimSet;
typedef boost::scoped_ptr<StateAnimSet> StateAnimSetPtr;

template<class _WizData> class SoundSet;
typedef SoundSet<AddInterfBtnStateWizData> StateSoundSet;
typedef boost::scoped_ptr<StateSoundSet> StateSoundSetPtr;

class interf_btn_state_name_test;
typedef boost::scoped_ptr<interf_btn_state_name_test> interf_btn_state_name_testPtr;

class BtnStateFabric;
typedef boost::scoped_ptr<BtnStateFabric> BtnStateFabricPtr;

template <class _Data> class name_setter;
class qdInterfaceButton;

///	Визард, добавляющий состояние кнопки
class AddInterfBtnStateWiz : public MWWizard
{
	typedef name_setter<AddInterfBtnStateWizData> NameSetter;
public:
	AddInterfBtnStateWiz(void);
	~AddInterfBtnStateWiz(void);
	/// Создает объекты для работы и увязывает их между собой
	/**
		@return false - елси ну дулось что-нибудь создать
	*/
	bool Construct(qdInterfaceButton* pbtn);
private:
	/// Данные визарда
	AddInterfBtnStateWizDataPtr m_ptrData;
	/// Объект, назначающий имя элементу
	boost::scoped_ptr<NameSetter> m_ptrNameSetter;

	/// Объект, назначающий анимацию элементу
	StateAnimSetPtr				m_ptrAnimSet;
	/// Объект, назначающий звук элементу
	StateSoundSetPtr			m_ptrSoundSet;

	/// Страничка выбора имени
	SelectNameMWPagePtr			m_ptrSelStateNamePage;
	/// Страничка выбора анимации и звука
	SelectAnimSoundMWPagePtr	m_ptrSelASPage;
	/// Последняя страничка
	AddIEleDonePagePtr			m_ptrAddElePage;
	/// Объект тестирующий уникальность имени
	interf_btn_state_name_testPtr m_ptrNameTester;
	/// Фабрика, создающая объект
	BtnStateFabricPtr			m_ptrFabric;
};
