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

class AddInterfSaveWizData;
typedef boost::scoped_ptr<AddInterfSaveWizData> AddInterfSaveWizDataPtr;


template<class _WizData> class AnimSet;
typedef AnimSet<AddInterfSaveWizData> SaveAnimSet;
typedef boost::scoped_ptr<SaveAnimSet> SaveAnimSetPtr;

template<class _WizData> class SoundSet;
typedef SoundSet<AddInterfSaveWizData> SaveSoundSet;
typedef boost::scoped_ptr<SaveSoundSet> SaveSoundSetPtr;

class SaveObjFabric;
typedef boost::scoped_ptr<SaveObjFabric> SaveObjFabricPtr;

///	Визард создающий сейв
class AddInterfSaveWiz : public MWWizard
{
	typedef name_setter<AddInterfSaveWizData> NameSetter;
public:
	AddInterfSaveWiz(void);
	~AddInterfSaveWiz(void);

	/// Создает объекты для работы и увязывает их между собой
	/**
		@return false - елси не удалось что-нибудь создать
	*/
	bool Construct(qdInterfaceScreen* pscr);
private:
	/// Данные визарда
	AddInterfSaveWizDataPtr			m_ptrData;
	/// Объект, назначающий имя элементу
	boost::scoped_ptr<NameSetter>	m_ptrNameSetter;
	/// Объект тестирующий уникальность имени
	ScreenEleNameTestPtr			m_ptrNameTest;
	/// Объект, назначающий анимацию элементу
	SaveAnimSetPtr					m_ptrAnimSet;
	/// Объект, назначающий звук элементу
	SaveSoundSetPtr					m_ptrSoundSet;
	/// Фабрика, создающая объект
	SaveObjFabricPtr				m_ptrFabric;

	/// Страничка выбора имени
	SelectNameMWPagePtr				m_ptrSelNamePage;
	/// Страничка выбора анимации и звука
	SelectAnimSoundMWPagePtr		m_ptrSelASPage;
	/// Последняя страничка
	AddIEleDonePagePtr				m_ptrAddElePage;
};
