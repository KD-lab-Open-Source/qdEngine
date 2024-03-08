#pragma once
#include "multiwizard\mwwizard.h"
#include <boost/scoped_ptr.hpp>

class SelectNameMWPage;
typedef boost::scoped_ptr<SelectNameMWPage> SelectNameMWPagePtr;

class SelectAnimSoundMWPage;
typedef boost::scoped_ptr<SelectAnimSoundMWPage> SelectAnimSoundMWPagePtr;

class AddIEleDonePage;
typedef boost::scoped_ptr<AddIEleDonePage> AddIEleDonePagePtr;

class screen_ele_name_test;
typedef boost::scoped_ptr<screen_ele_name_test> ScreenEleNameTestPtr;

template <class _Data> class name_setter;

class AddInterfBtnWizData;
typedef boost::scoped_ptr<AddInterfBtnWizData> AddInterfBtnWizDataPtr;


template<class _WizData> class AnimSet;
typedef AnimSet<AddInterfBtnWizData> BtnAnimSet;
typedef boost::scoped_ptr<BtnAnimSet> BtnAnimSetPtr;

template<class _WizData> class SoundSet;
typedef SoundSet<AddInterfBtnWizData> BtnSoundSet;
typedef boost::scoped_ptr<BtnSoundSet> BtnSoundSetPtr;


class BtnObjFabric;
typedef boost::scoped_ptr<BtnObjFabric> BtnObjFabricPtr;

class StateNameSet;
typedef boost::scoped_ptr<StateNameSet> StateNameSetPtr;


/// \brief Визард добавляющий кнопку в экран
class AddInterfBtnWiz :	public MWWizard
{
	typedef name_setter<AddInterfBtnWizData> NameSetter;
public:
	AddInterfBtnWiz(void);
	virtual ~AddInterfBtnWiz(void);
	/// Создает объекты для работы и увязывает их между собой
	/**
		@return false - елси ну дулось что-нибудь создать
	*/
	bool Construct(qdInterfaceScreen* pscr);
private:
	/// Данные визарда
	AddInterfBtnWizDataPtr		m_ptrData;
	/// Объект, назначающий имя элементу
    boost::scoped_ptr<NameSetter>	m_ptrNameSetter;
	/// Объект тестирующий уникальность имени
	ScreenEleNameTestPtr		m_ptrNameTest;
	/// Объект назначает анимацию кнопке
	BtnAnimSetPtr				m_ptrAnimSet;
	/// Объект назначает звук
	BtnSoundSetPtr				m_ptrSoundSet;
	/// Фабрика, создающая объект
	BtnObjFabricPtr				m_ptrObjFab;
	/// Объект, назначающий имя состоянию кнопки
	StateNameSetPtr				m_ptrStateNameSet;

	/// Страничка выбора имени
	SelectNameMWPagePtr			m_ptrSelNamePage;
	/// Страничка выбора имени для состояния
	SelectNameMWPagePtr			m_ptrSelStateNamePage;
	/// Страничка выбора анимации и звука
	SelectAnimSoundMWPagePtr	m_ptrSelASPage;
	/// Последняя страничка
	AddIEleDonePagePtr			m_ptrAddElePage;
};
