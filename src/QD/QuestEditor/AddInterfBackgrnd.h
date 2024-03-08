#pragma once
#include "multiwizard\mwwizard.h"
#include <memory>

class AddBackData;
typedef std::auto_ptr<AddBackData> AddBackDataPtr;

class SelectNameMWPage;
typedef std::auto_ptr<SelectNameMWPage> SelectNameMWPagePtr;

class SelectAnimSoundMWPage;
typedef std::auto_ptr<SelectAnimSoundMWPage> SelectAnimSoundMWPagePtr;

class AddIEleDonePage;
typedef std::auto_ptr<AddIEleDonePage> AddIEleDonePagePtr;

class AnimTextSet;
typedef std::auto_ptr<AnimTextSet> AnimTextSetPtr;

class BackObjFabric;
typedef std::auto_ptr<BackObjFabric> BackObjFabricPtr;

class screen_ele_name_test;
typedef std::auto_ptr<screen_ele_name_test> ScreenEleNameTestPtr;

template <class _Data> class name_setter;

/// \brief Визард, добавляющий фон для интерйесного экрана
class AddInterfBackgrnd :
	public MWWizard
{
	typedef name_setter<AddBackData> NameSetter;
public:
	AddInterfBackgrnd(void);
	~AddInterfBackgrnd(void);

	AddBackData* data() const{
		return m_ptrData.get();
	}

	/// Создает объекты для работы и увязывает их между собой
	/**
		@return false - елси ну дулось что-нибудь создать
	 */
	bool Construct(qdInterfaceScreen* pscr);
private:
	/// Данные визарда
	AddBackDataPtr		m_ptrData;
	/// Страничка выбора имени
	SelectNameMWPagePtr m_ptrSelNamePage;
	/// Страничка выбора анимации и звука
	SelectAnimSoundMWPagePtr m_ptrSelAnimSoundPage;
	/// Объект, назначающий анимацию
	AnimTextSetPtr		m_ptrAnimTextSet;
	/// Последняя страничка
	AddIEleDonePagePtr	m_ptrAddIEleDonePage;
	/// Фабрика, создающая объект
	BackObjFabricPtr	m_ptrBackObjFabric;
	/// Объект, назначающий имя элементу
	std::auto_ptr<NameSetter> m_ptrNameSetter;
	/// Объект тестирующий уникальность имени
	ScreenEleNameTestPtr m_ptrScrEleNameTest;
};
