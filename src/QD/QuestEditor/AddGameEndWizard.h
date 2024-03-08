#pragma once
#include "multiwizard\mwwizard.h"
#include <boost/scoped_ptr.hpp>
#include "name_interf.h"
#include "name_setter.h"

interface ITreeNode;

class SelectNameMWPage;
typedef boost::scoped_ptr<SelectNameMWPage> SelectNameMWPagePtr;

class AddIEleDonePage;
typedef boost::scoped_ptr<AddIEleDonePage> AddIEleDonePagePtr;

class AddGameEndWizData;
typedef boost::scoped_ptr<AddGameEndWizData> AddGameEndWizDataPtr;

class SelectIScreenPage;
typedef boost::scoped_ptr<SelectIScreenPage> SelectIScreenPagePtr;

class qdGameDispatcher;
class obj_fabrica;

///	������ ����������� "��������� �����" � �����
class AddGameEndWizard : public MWWizard
{
	/// ��������� ���������� ��� ��� "���������� ������"
	class name_tester : public INameTester
	{
	public:
		name_tester(qdGameDispatcher* ptrDisp):m_ptrDisp(ptrDisp){}
		const CString get_unique_name(const CString& test) const;
	private:
		/// ��������� �������
		qdGameDispatcher* m_ptrDisp;
	};

public:
	AddGameEndWizard(void);
	~AddGameEndWizard(void);

	/// ������� ������� ��� ������ � ��������� �� ����� �����. ������ ��� ������ ������
	/**
		@param parent - ����, ��� ������� ����� ������ �����
		@return false - ���� �� ������� ���-������ �������
	*/
	bool Construct(qdGameDispatcher* ptrDisp, ITreeNode* parent);
	/// ������� ������� ��� ������ � ��������� �� ����� �����. 
	/// ������ ��� ������� ������ ������. �� ������ ��� ��������������
	/**
		@param hParent - ����, ��� ������� ����� ������ �����
		@return false - ���� �� ������� ���-������ �������
	*/
	bool Construct(qdGameDispatcher* ptrDisp, HTREEITEM hParent);

	AddGameEndWizData* data(){
		return m_ptrData.get();
	}
private:
	typedef name_setter<AddGameEndWizData>	NameSetter;
	/// ������, ����������� ��� ��������
	boost::scoped_ptr<NameSetter>			m_ptrNameSetter;
	/// ������ ����������� ������������ �����
	boost::scoped_ptr<name_tester>			m_ptrNameTest;
	/// ������ �������
	boost::scoped_ptr<AddGameEndWizData>	m_ptrData;
	/// ��������� ������ �����
	SelectNameMWPagePtr						m_ptrSelNamePage;
	/// ��������� ���������
	AddIEleDonePagePtr						m_ptrAddElePage;
	/// ��������� ������ ������������� ������
	SelectIScreenPagePtr					m_ptrISelScreenPage;
	/// �������, ��������� ������
	boost::scoped_ptr<obj_fabrica>			m_ptrFabric;
};
