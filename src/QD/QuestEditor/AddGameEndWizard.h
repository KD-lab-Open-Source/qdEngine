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

///	Визард добавляющий "последний экран" в прект
class AddGameEndWizard : public MWWizard
{
	/// Возвращет уникальное имя для "последнего экрана"
	class name_tester : public INameTester
	{
	public:
		name_tester(qdGameDispatcher* ptrDisp):m_ptrDisp(ptrDisp){}
		const CString get_unique_name(const CString& test) const;
	private:
		/// Диспетчер проекта
		qdGameDispatcher* m_ptrDisp;
	};

public:
	AddGameEndWizard(void);
	~AddGameEndWizard(void);

	/// Создает объекты для работы и увязывает их между собой. Версия для нового дерева
	/**
		@param parent - узел, под которым будет создан экран
		@return false - елси не удалось что-нибудь создать
	*/
	bool Construct(qdGameDispatcher* ptrDisp, ITreeNode* parent);
	/// Создает объекты для работы и увязывает их между собой. 
	/// Версия для старого дерева дерева. Не должна уже использоваться
	/**
		@param hParent - узел, под которым будет создан экран
		@return false - елси не удалось что-нибудь создать
	*/
	bool Construct(qdGameDispatcher* ptrDisp, HTREEITEM hParent);

	AddGameEndWizData* data(){
		return m_ptrData.get();
	}
private:
	typedef name_setter<AddGameEndWizData>	NameSetter;
	/// Объект, назначающий имя элементу
	boost::scoped_ptr<NameSetter>			m_ptrNameSetter;
	/// Объект тестирующий уникальность имени
	boost::scoped_ptr<name_tester>			m_ptrNameTest;
	/// Данные визарда
	boost::scoped_ptr<AddGameEndWizData>	m_ptrData;
	/// Страничка выбора имени
	SelectNameMWPagePtr						m_ptrSelNamePage;
	/// Последняя страничка
	AddIEleDonePagePtr						m_ptrAddElePage;
	/// Страничка выбора интерфейсного экрана
	SelectIScreenPagePtr					m_ptrISelScreenPage;
	/// Фабрика, создающая объект
	boost::scoped_ptr<obj_fabrica>			m_ptrFabric;
};
