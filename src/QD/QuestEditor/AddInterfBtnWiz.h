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


/// \brief ������ ����������� ������ � �����
class AddInterfBtnWiz :	public MWWizard
{
	typedef name_setter<AddInterfBtnWizData> NameSetter;
public:
	AddInterfBtnWiz(void);
	virtual ~AddInterfBtnWiz(void);
	/// ������� ������� ��� ������ � ��������� �� ����� �����
	/**
		@return false - ���� �� ������ ���-������ �������
	*/
	bool Construct(qdInterfaceScreen* pscr);
private:
	/// ������ �������
	AddInterfBtnWizDataPtr		m_ptrData;
	/// ������, ����������� ��� ��������
    boost::scoped_ptr<NameSetter>	m_ptrNameSetter;
	/// ������ ����������� ������������ �����
	ScreenEleNameTestPtr		m_ptrNameTest;
	/// ������ ��������� �������� ������
	BtnAnimSetPtr				m_ptrAnimSet;
	/// ������ ��������� ����
	BtnSoundSetPtr				m_ptrSoundSet;
	/// �������, ��������� ������
	BtnObjFabricPtr				m_ptrObjFab;
	/// ������, ����������� ��� ��������� ������
	StateNameSetPtr				m_ptrStateNameSet;

	/// ��������� ������ �����
	SelectNameMWPagePtr			m_ptrSelNamePage;
	/// ��������� ������ ����� ��� ���������
	SelectNameMWPagePtr			m_ptrSelStateNamePage;
	/// ��������� ������ �������� � �����
	SelectAnimSoundMWPagePtr	m_ptrSelASPage;
	/// ��������� ���������
	AddIEleDonePagePtr			m_ptrAddElePage;
};
