#include "stdafx.h"
#include "QuestEditor.h"
#include "addinterfbackgrnd.h"

#include "AddBackData.h"
#include "SelectNameMWPage.h"
#include "SelectAnimSoundMWPage.h"
#include "AddIEleDonePage.h"
#include "name_setter.h"

#include "ptree_api.h"
#include "EdgeRemover.h"

#include "qd_interface_background.h"
#include "screen_ele_name_test.h"


#include "ShlwApi.h"

/**
	Реализация SelectAnimSoundMWPage::text_set назначающая файл анимации
	в AddBackData.
 */
class AnimTextSet : public SelectAnimSoundMWPage::text_set
{
	/// Объект, которому будет назначен текст
	AddBackData * pdata_;
public:
	AnimTextSet(AddBackData* pdata):pdata_(pdata){}
	bool set_text(const CString& str){
		if (PathFileExists(str))
		{
			pdata_->set_anim_file(str);
			return true;
		}
		return false;
	}
	const CString& get_text()const {
		return pdata_->anim_file();
	}
};

/**
	Фабрика создания фона
 */
class BackObjFabric : public AddIEleDonePage::obj_fabric
{
	AddBackData* pdata_;
public:
	explicit BackObjFabric(AddBackData* pdata): pdata_(pdata){}
	/// Создает фон
	bool create_obj(){
		qdInterfaceBackground* pback = 
			ptree_api::AddInterfBackgrnd(pdata_->name(), pdata_->screen());
		if (!pback)
			return false;
		pback->set_animation_file(pdata_->anim_file());
		if (AfxMessageBox(IDS_ASK_REMOVE_EDGES, MB_YESNO) == IDYES)
		{
			EdgeRemover er;
			er.remove_edges(pdata_->anim_file());
		}

		pback->init();
		pback->set_r(Vect2i(pback->size_x()>>1, pback->size_y()>>1));
		return true;
	}
};

AddInterfBackgrnd::AddInterfBackgrnd(void)
{

}

AddInterfBackgrnd::~AddInterfBackgrnd(void)
{
}

bool AddInterfBackgrnd::Construct(qdInterfaceScreen* pscr)
{
	m_ptrData.reset(new AddBackData(pscr));
	if (!m_ptrData.get())
		return false;
	m_ptrAnimTextSet.reset(new AnimTextSet(m_ptrData.get()));
	if (!m_ptrAnimTextSet.get())
		return false;

	m_ptrBackObjFabric.reset(new BackObjFabric(m_ptrData.get()));
	if (!m_ptrBackObjFabric.get())
		return false;

	m_ptrNameSetter.reset(new NameSetter(m_ptrData.get()));
	if (!m_ptrNameSetter.get())
		return false;

	m_ptrScrEleNameTest.reset(new screen_ele_name_test(pscr));
	if (!m_ptrScrEleNameTest.get())
		return false;

	m_ptrSelNamePage.reset(new SelectNameMWPage);
	if (!m_ptrSelNamePage.get())
		return false;

	m_ptrSelNamePage->set_wizard(this);
	m_ptrSelNamePage->SetNameSetter(m_ptrNameSetter.get());
	m_ptrSelNamePage->SetNameTester(m_ptrScrEleNameTest.get());
	m_ptrSelNamePage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_DEF_SEL_NAME));

	m_ptrSelAnimSoundPage.reset(new SelectAnimSoundMWPage);
	if (!m_ptrSelAnimSoundPage.get())
		return false;
	m_ptrSelAnimSoundPage->set_wizard(this);
	m_ptrSelAnimSoundPage->set_anim_setter(m_ptrAnimTextSet.get());
	m_ptrSelAnimSoundPage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_IMAGE));

	m_ptrAddIEleDonePage.reset(new AddIEleDonePage);
	if (!m_ptrAddIEleDonePage.get())
		return false;

	m_ptrAddIEleDonePage->set_prev_page(m_ptrSelNamePage.get());
	m_ptrAddIEleDonePage->set_obj_fabric(m_ptrBackObjFabric.get());
	m_ptrAddIEleDonePage->set_wizard(this);

	add_page(m_ptrSelNamePage.get());
	add_page(m_ptrSelAnimSoundPage.get());
	add_page(m_ptrAddIEleDonePage.get());

	return true;
}
