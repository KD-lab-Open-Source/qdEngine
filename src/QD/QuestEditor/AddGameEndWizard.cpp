#include "stdafx.h"
#include "resource.h"
#include "addgameendwizard.h"
#include "qd_game_end.h"
#include "name_setter.h"
#include "SelectNameMWPage.h"
#include "SelectIScreenPage.h"
#include "AddIEleDonePage.h"
#include "name_op.h"
#include "ptree_api.h"
#include "AddGameEndWizData.h"

/**
	Фабрика, добавляющая последний экран
 */
class obj_fabrica : public AddIEleDonePage::obj_fabric
{
	/// Данные визрада
	AddGameEndWizData const* m_pData;
public:
	explicit obj_fabrica(AddGameEndWizData const* pdata):m_pData(pdata)
	{}
	bool create_obj(){
		return 
			(ptree_api::AddGameEnd(m_pData->parent(), 
					m_pData->name(), m_pData->scrName())
				!= NULL);
	}
};

const CString AddGameEndWizard::name_tester::get_unique_name(const CString& test) const{
	return ::get_unique_name(test, m_ptrDisp->game_end_list());
}

AddGameEndWizard::AddGameEndWizard(void)
{
}

AddGameEndWizard::~AddGameEndWizard(void)
{
}

bool AddGameEndWizard::Construct(qdGameDispatcher* ptrDisp, ITreeNode* parent)
{
	m_ptrData.reset(new AddGameEndWizData(parent));

	if (!m_ptrData)
		return false;
	m_ptrNameSetter.reset(new NameSetter(m_ptrData.get()));
	if (!m_ptrNameSetter)
		return false;

	m_ptrNameTest.reset(new name_tester(ptrDisp));
	if (!m_ptrNameTest)
		return false;

	m_ptrSelNamePage.reset(new SelectNameMWPage);
	if (!m_ptrSelNamePage)
		return false;
											 
	m_ptrAddElePage.reset(new AddIEleDonePage);
	if (!m_ptrAddElePage.get())
		return false;

	m_ptrFabric.reset(new obj_fabrica(m_ptrData.get()));
	if (!m_ptrFabric)
		return false;

	m_ptrISelScreenPage.reset(new SelectIScreenPage);
	if (!m_ptrISelScreenPage)
		return false;
	m_ptrISelScreenPage->set_wizard(this);

	m_ptrAddElePage->set_wizard(this);
	m_ptrAddElePage->set_obj_fabric(m_ptrFabric.get());
	m_ptrAddElePage->set_prev_page(m_ptrSelNamePage.get());

	m_ptrSelNamePage->set_wizard(this);
	m_ptrSelNamePage->SetNameSetter(m_ptrNameSetter.get());
	m_ptrSelNamePage->SetNameTester(m_ptrNameTest.get());
	m_ptrSelNamePage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_DEF_SEL_NAME));
	m_ptrSelNamePage->set_prev_page(m_ptrISelScreenPage.get());

	add_page(m_ptrISelScreenPage.get());
	add_page(m_ptrSelNamePage.get());
	add_page(m_ptrAddElePage.get());

	return true;
}

bool AddGameEndWizard::Construct(qdGameDispatcher* ptrDisp, HTREEITEM hParent)
{
	m_ptrData.reset(new AddGameEndWizData(hParent));

	if (!m_ptrData)
		return false;
	m_ptrNameSetter.reset(new NameSetter(m_ptrData.get()));
	if (!m_ptrNameSetter)
		return false;

	m_ptrNameTest.reset(new name_tester(ptrDisp));
	if (!m_ptrNameTest)
		return false;

	m_ptrSelNamePage.reset(new SelectNameMWPage);
	if (!m_ptrSelNamePage)
		return false;

	m_ptrAddElePage.reset(new AddIEleDonePage);
	if (!m_ptrAddElePage.get())
		return false;

	m_ptrFabric.reset(new obj_fabrica(m_ptrData.get()));
	if (!m_ptrFabric)
		return false;

	m_ptrISelScreenPage.reset(new SelectIScreenPage);
	if (!m_ptrISelScreenPage)
		return false;
	m_ptrISelScreenPage->set_wizard(this);

	m_ptrAddElePage->set_wizard(this);
	m_ptrAddElePage->set_obj_fabric(m_ptrFabric.get());
	m_ptrAddElePage->set_prev_page(m_ptrSelNamePage.get());

	m_ptrSelNamePage->set_wizard(this);
	m_ptrSelNamePage->SetNameSetter(m_ptrNameSetter.get());
	m_ptrSelNamePage->SetNameTester(m_ptrNameTest.get());
	m_ptrSelNamePage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_DEF_SEL_NAME));
	m_ptrSelNamePage->set_prev_page(m_ptrISelScreenPage.get());

	add_page(m_ptrISelScreenPage.get());
	add_page(m_ptrSelNamePage.get());
	add_page(m_ptrAddElePage.get());

	return true;
}