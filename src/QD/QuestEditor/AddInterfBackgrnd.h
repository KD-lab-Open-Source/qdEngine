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

/// \brief ������, ����������� ��� ��� ������������ ������
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

	/// ������� ������� ��� ������ � ��������� �� ����� �����
	/**
		@return false - ���� �� ������ ���-������ �������
	 */
	bool Construct(qdInterfaceScreen* pscr);
private:
	/// ������ �������
	AddBackDataPtr		m_ptrData;
	/// ��������� ������ �����
	SelectNameMWPagePtr m_ptrSelNamePage;
	/// ��������� ������ �������� � �����
	SelectAnimSoundMWPagePtr m_ptrSelAnimSoundPage;
	/// ������, ����������� ��������
	AnimTextSetPtr		m_ptrAnimTextSet;
	/// ��������� ���������
	AddIEleDonePagePtr	m_ptrAddIEleDonePage;
	/// �������, ��������� ������
	BackObjFabricPtr	m_ptrBackObjFabric;
	/// ������, ����������� ��� ��������
	std::auto_ptr<NameSetter> m_ptrNameSetter;
	/// ������ ����������� ������������ �����
	ScreenEleNameTestPtr m_ptrScrEleNameTest;
};
