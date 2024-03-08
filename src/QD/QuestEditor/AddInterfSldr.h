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

///	������ ���������� ��������
class AddInterfSlider : public MWWizard
{
	typedef name_setter<AddSldrData> NameSetter;
public:
	AddInterfSlider(void);
	~AddInterfSlider(void);
	/// ������� ��� ����������� ��� ������ ������� � ��������� �� ����� �����
	bool Construct(qdInterfaceScreen* pscr);
private:
	/// ������ �������
	AddSldrDataPtr				m_ptrData;
	/// ������ ����������� ������������ �����
	ScreenEleNameTestPtr		m_ptrNameTest;
	/// �������, ��������� ������
	SldrObjFabricPtr			m_ptrSldrObjFabr;
	/// ������ ��������� �������� �������� ��� ��������
	BackAnimSetPtr				m_ptrBackAnimSet;
	/// ������ ��������� ���� �������
	ForegrSoundSetPtr			m_ptrForegrSoundSet;
	/// ������ ��������� �������� �������
	ForegrAnimSetPtr			m_ptrForegrAnimSet;
	/// ������ ����������� ��� ��������
	std::auto_ptr<NameSetter>	m_ptrNameSetter;
	/// ��������� ������ �����
	SelectNameMWPagePtr			m_ptrSelName;
	/// ��������� ������ �������� � ����� ��� ��������
	SelectAnimSoundMWPagePtr	m_ptrSelASForBack;
	/// ��������� ������ �������� � ����� ��� �������
	SelectAnimSoundMWPagePtr	m_ptrSelASForForeground;
	/// ��������� ���������
	AddIEleDonePagePtr			m_ptrDonePg;
};
