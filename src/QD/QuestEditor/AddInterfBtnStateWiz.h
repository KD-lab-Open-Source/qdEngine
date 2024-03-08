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

///	������, ����������� ��������� ������
class AddInterfBtnStateWiz : public MWWizard
{
	typedef name_setter<AddInterfBtnStateWizData> NameSetter;
public:
	AddInterfBtnStateWiz(void);
	~AddInterfBtnStateWiz(void);
	/// ������� ������� ��� ������ � ��������� �� ����� �����
	/**
		@return false - ���� �� ������ ���-������ �������
	*/
	bool Construct(qdInterfaceButton* pbtn);
private:
	/// ������ �������
	AddInterfBtnStateWizDataPtr m_ptrData;
	/// ������, ����������� ��� ��������
	boost::scoped_ptr<NameSetter> m_ptrNameSetter;

	/// ������, ����������� �������� ��������
	StateAnimSetPtr				m_ptrAnimSet;
	/// ������, ����������� ���� ��������
	StateSoundSetPtr			m_ptrSoundSet;

	/// ��������� ������ �����
	SelectNameMWPagePtr			m_ptrSelStateNamePage;
	/// ��������� ������ �������� � �����
	SelectAnimSoundMWPagePtr	m_ptrSelASPage;
	/// ��������� ���������
	AddIEleDonePagePtr			m_ptrAddElePage;
	/// ������ ����������� ������������ �����
	interf_btn_state_name_testPtr m_ptrNameTester;
	/// �������, ��������� ������
	BtnStateFabricPtr			m_ptrFabric;
};
