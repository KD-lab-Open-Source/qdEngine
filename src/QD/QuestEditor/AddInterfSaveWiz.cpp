#include "stdafx.h"
#include "resource.h"
#include "addinterfsavewiz.h"

#include "AddInterfSaveWizData.h"

#include "SelectNameMWPage.h"
#include "SelectAnimSoundMWPage.h"
#include "AddIEleDonePage.h"
#include "EdgeRemover.h"

#include "name_setter.h"

#include "screen_ele_name_test.h"
#include "qd_interface_save.h"
#include "interf_state_util.h"

#include "ptree_api.h"

/**
	Фабрика, создающая сейв 
 */
class SaveObjFabric	: public AddIEleDonePage::obj_fabric
{
	/// Данные визарда
	AddInterfSaveWizData* pdata_;
public:
	explicit SaveObjFabric(AddInterfSaveWizData* data):pdata_(data){}

	bool create_obj(){
		qdInterfaceSave* psave = 
			ptree_api::AddInterfSave(pdata_->name(), pdata_->screen());
		if (!psave)
			return false;

		if (AfxMessageBox(IDS_ASK_REMOVE_EDGES, MB_YESNO) == IDYES)
		{
			EdgeRemover er;
			er.remove_edges(pdata_->anim_name());
		}

		if (pdata_->anim_name().IsEmpty())
			psave->set_frame_animation_file(NULL);
		else
			psave->set_frame_animation_file(pdata_->anim_name());

		if (pdata_->sound_name().IsEmpty())
			psave->set_frame_sound_file(NULL);
		else
			psave->set_frame_sound_file(pdata_->sound_name());

		psave->init();

		psave->set_r(Vect2i(psave->size_x()>>1, psave->size_y()>>1));
		int sz = psave->size_x();
		if (sz)
			psave->set_thumbnail_size_x(sz);
		sz = psave->size_y();
		if (sz)
			psave->set_thumbnail_size_y(sz);
		return true;
	}
};


AddInterfSaveWiz::AddInterfSaveWiz(void)
{
}

AddInterfSaveWiz::~AddInterfSaveWiz(void)
{
}

bool AddInterfSaveWiz::Construct(qdInterfaceScreen* pscr)
{
	m_ptrData.reset(new AddInterfSaveWizData(pscr));
	if (!m_ptrData)
		return false;

	m_ptrNameSetter.reset(new NameSetter(m_ptrData.get()));
	if (!m_ptrNameSetter)
		return false;

	m_ptrNameTest.reset(new screen_ele_name_test(pscr));
	if (!m_ptrNameTest)
		return false;

	m_ptrAnimSet.reset(new SaveAnimSet(m_ptrData.get(), false));
	if (!m_ptrAnimSet)
		return false;

	m_ptrSoundSet.reset(new SaveSoundSet(m_ptrData.get()));
	if (!m_ptrSoundSet)
		return false;

	m_ptrFabric.reset(new SaveObjFabric(m_ptrData.get()));
	if (!m_ptrFabric)
		return false;

	m_ptrSelNamePage.reset(new SelectNameMWPage);
	if (!m_ptrSelNamePage)
		return false;

	m_ptrSelNamePage->set_wizard(this);
	m_ptrSelNamePage->SetNameSetter(m_ptrNameSetter.get());
	m_ptrSelNamePage->SetNameTester(m_ptrNameTest.get());
	m_ptrSelNamePage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_DEF_SEL_NAME));

	m_ptrSelASPage.reset(new SelectAnimSoundMWPage);
	if (!m_ptrSelASPage.get())
		return false;

	m_ptrSelASPage->set_wizard(this);
	m_ptrSelASPage->set_prev_page(m_ptrSelNamePage.get());
	m_ptrSelASPage->set_anim_setter(m_ptrAnimSet.get());
	m_ptrSelASPage->set_sound_setter(m_ptrSoundSet.get());
	m_ptrSelASPage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_BTN_ANIM_SOUND));

	m_ptrAddElePage.reset(new AddIEleDonePage);
	if (!m_ptrAddElePage.get())
		return false;

	m_ptrAddElePage->set_wizard(this);
	m_ptrAddElePage->set_obj_fabric(m_ptrFabric.get());
	m_ptrAddElePage->set_prev_page(m_ptrSelASPage.get());

	add_page(m_ptrSelNamePage.get());
	add_page(m_ptrSelASPage.get());
	add_page(m_ptrAddElePage.get());

	return true;
}