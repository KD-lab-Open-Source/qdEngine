#pragma once
#include "multiwizard\mwwizard.h"
#include <memory>

class SelectNameMWPage;
typedef std::auto_ptr<SelectNameMWPage> SelectNameMWPagePtr;

class SelectAnimSoundMWPage;
typedef std::auto_ptr<SelectAnimSoundMWPage> SelectAnimSoundMWPagePtr;

class AddIEleDonePage;
typedef std::auto_ptr<AddIEleDonePage> AddIEleDonePagePtr;
 
class SldrObjFabric;
typedef std::auto_ptr<SldrObjFabric> SldrObjFabricPtr;

class BackAnimSet;
typedef std::auto_ptr<BackAnimSet> BackAnimSetPtr;

class ForegrSoundSet;
typedef std::auto_ptr<ForegrSoundSet> ForegrSoundSetPtr;

class ForegrAnimSet;
typedef std::auto_ptr<ForegrAnimSet> ForegrAnimSetPtr;

template<class _Data> class name_setter;

class AddSldrData;
typedef std::auto_ptr<AddSldrData> AddSldrDataPtr;

class screen_ele_name_test;
typedef std::auto_ptr<screen_ele_name_test> ScreenEleNameTestPtr;

///	Визард добавления слайдера
class AddInterfSlider : public MWWizard
{
	typedef name_setter<AddSldrData> NameSetter;
public:
	AddInterfSlider(void);
	~AddInterfSlider(void);
	/// Создает все необходимые для работы объекты и увязывает их между собой
	bool Construct(qdInterfaceScreen* pscr);
private:
	/// Данные визарда
	AddSldrDataPtr				m_ptrData;
	/// Объект тестирующий уникальность имени
	ScreenEleNameTestPtr		m_ptrNameTest;
	/// Фабрика, создающая объект
	SldrObjFabricPtr			m_ptrSldrObjFabr;
	/// Объект назначает анимацию подложки для слайдера
	BackAnimSetPtr				m_ptrBackAnimSet;
	/// Объект назначает звук бегунка
	ForegrSoundSetPtr			m_ptrForegrSoundSet;
	/// Объект назначает анимацию бегунка
	ForegrAnimSetPtr			m_ptrForegrAnimSet;
	/// Объект назначающий имя слайдеру
	std::auto_ptr<NameSetter>	m_ptrNameSetter;
	/// Страничка выбора имени
	SelectNameMWPagePtr			m_ptrSelName;
	/// Страничка выбора анимации и звука для подложки
	SelectAnimSoundMWPagePtr	m_ptrSelASForBack;
	/// Страничка выбора анимации и звука для бегунка
	SelectAnimSoundMWPagePtr	m_ptrSelASForForeground;
	/// Последняя страничка
	AddIEleDonePagePtr			m_ptrDonePg;
};
