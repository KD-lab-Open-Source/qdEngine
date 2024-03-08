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

///	������ ��������� ����
class AddInterfSaveWiz : public MWWizard
{
	typedef name_setter<AddInterfSaveWizData> NameSetter;
public:
	AddInterfSaveWiz(void);
	~AddInterfSaveWiz(void);

	/// ������� ������� ��� ������ � ��������� �� ����� �����
	/**
		@return false - ���� �� ������� ���-������ �������
	*/
	bool Construct(qdInterfaceScreen* pscr);
private:
	/// ������ �������
	AddInterfSaveWizDataPtr			m_ptrData;
	/// ������, ����������� ��� ��������
	boost::scoped_ptr<NameSetter>	m_ptrNameSetter;
	/// ������ ����������� ������������ �����
	ScreenEleNameTestPtr			m_ptrNameTest;
	/// ������, ����������� �������� ��������
	SaveAnimSetPtr					m_ptrAnimSet;
	/// ������, ����������� ���� ��������
	SaveSoundSetPtr					m_ptrSoundSet;
	/// �������, ��������� ������
	SaveObjFabricPtr				m_ptrFabric;

	/// ��������� ������ �����
	SelectNameMWPagePtr				m_ptrSelNamePage;
	/// ��������� ������ �������� � �����
	SelectAnimSoundMWPagePtr		m_ptrSelASPage;
	/// ��������� ���������
	AddIEleDonePagePtr				m_ptrAddElePage;
};
