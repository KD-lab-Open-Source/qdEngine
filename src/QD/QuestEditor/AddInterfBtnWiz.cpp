#include "stdafx.h"
#include "resource.h"
#include "addinterfbtnwiz.h"

#include "AddIntefBtnWizData.h"
#include "SelectNameMWPage.h"
#include "SelectAnimSoundMWPage.h"
#include "AddIEleDonePage.h"
#include "name_setter.h"

#include "ConvertTGAtoQDA.h"

#include "ptree_api.h"

#include "qd_interface_button.h"
#include "screen_ele_name_test.h"
#include "interf_state_util.h"

#include "EdgeRemover.h"

#include "disk_op.h"

class StateNameSet :
	public INameSetter
{
	AddInterfBtnWizData* pdata_;
public:

	explicit StateNameSet(AddInterfBtnWizData* pdata):
	pdata_(pdata)
	{
	}

	virtual const CString& get_string() const{
		return pdata_->state_name();
	}
	virtual void set_string(const CString& str){
		pdata_->set_state_name(str);
	}
};

class BtnObjFabric : public AddIEleDonePage::obj_fabric
{
	AddInterfBtnWizData* pdata_;
	void set_anim_and_sound(qdInterfaceElementState& st, 
								qdInterfaceElementState::state_mode_t mode)
	{
		st.set_animation_file(pdata_->anim_name(), mode);
		if (pdata_->sound_name().IsEmpty())
			st.set_sound_file(NULL, mode);
		else
			st.set_sound_file(pdata_->sound_name(), mode);
	}
	CString get_state_name(){
		CString name(pdata_->state_name());
		if (name.IsEmpty())
			name = disk_op::GetFileName(pdata_->anim_name());
		return name;
	}
	bool convertToQDA()
	{
		ConvertTGAtoQDA convertor;
		CString res = convertor.convert(pdata_->anim_name());
		if (res.IsEmpty())
			return false;
		pdata_->set_def_anim(res);
		return true;
	}
public:
	explicit BtnObjFabric(AddInterfBtnWizData* pdata): pdata_(pdata){}
	bool create_obj(){
		qdInterfaceButton* pbtn = 
			ptree_api::AddInterfBtn(pdata_->name(), pdata_->screen());
		if (!pbtn)
			return false;
		int iStateNum = ptree_api::AddInterfBtnState(get_state_name(), pbtn);
		if (iStateNum == -1){
			ptree_api::DeleteObject(pbtn, false);
			return false;
		}
		convertToQDA();
		if (AfxMessageBox(IDS_ASK_REMOVE_EDGES, MB_YESNO) == IDYES)
		{
			EdgeRemover er;
			er.remove_edges(pdata_->anim_name());
		}

		qdInterfaceElementState* pst(pbtn->get_state(iStateNum));
		set_anim_and_sound(*pst, qdInterfaceElementState::DEFAULT_MODE);
		set_anim_and_sound(*pst, qdInterfaceElementState::EVENT_MODE);
		set_anim_and_sound(*pst, qdInterfaceElementState::MOUSE_HOVER_MODE);

		pbtn->activate_state(0);

		pbtn->init();
		pbtn->set_r(Vect2i(pbtn->size_x()>>1, pbtn->size_y()>>1));
		return true;
	}
};

AddInterfBtnWiz::AddInterfBtnWiz(void)
{
}

AddInterfBtnWiz::~AddInterfBtnWiz(void)
{
}

bool AddInterfBtnWiz::Construct(qdInterfaceScreen* pscr)
{
	m_ptrData.reset(new AddInterfBtnWizData(pscr));
	if (!m_ptrData)
		return false;

	m_ptrNameSetter.reset(new NameSetter(m_ptrData.get()));
	if (!m_ptrNameSetter)
		return false;

	m_ptrNameTest.reset(new screen_ele_name_test(pscr));
	if (!m_ptrNameTest)
		return false;

	m_ptrAnimSet.reset(new BtnAnimSet(m_ptrData.get()));
	if (!m_ptrAnimSet)
		return false;

	m_ptrSoundSet.reset(new BtnSoundSet(m_ptrData.get()));
	if (!m_ptrSoundSet)
		return false;

	m_ptrObjFab.reset(new BtnObjFabric(m_ptrData.get()));
	if (!m_ptrObjFab)
		return false;

	m_ptrStateNameSet.reset(new StateNameSet(m_ptrData.get()));
	if (!m_ptrStateNameSet)
		return false;

	m_ptrSelNamePage.reset(new SelectNameMWPage);
	if (!m_ptrSelNamePage)
		return false;
	m_ptrSelNamePage->set_wizard(this);
	m_ptrSelNamePage->SetNameSetter(m_ptrNameSetter.get());
	m_ptrSelNamePage->SetNameTester(m_ptrNameTest.get());
	m_ptrSelNamePage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_DEF_SEL_NAME));

//	m_ptrSelStateNamePage.reset(new SelectNameMWPage);
//	if (!m_ptrSelStateNamePage.get())
//		return false;
//	m_ptrSelStateNamePage->set_wizard(this);
//	m_ptrSelStateNamePage->set_prev_page(m_ptrSelNamePage.get());
//	m_ptrSelStateNamePage->SetNameSetter(m_ptrStateNameSet.get());
//	m_ptrSelStateNamePage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_SELECT_STATE_NAME));

	m_ptrSelASPage.reset(new SelectAnimSoundMWPage);
	if (!m_ptrSelASPage.get())
		return false;
	m_ptrSelASPage->set_wizard(this);
//	m_ptrSelASPage->set_prev_page(m_ptrSelStateNamePage.get());
	m_ptrSelASPage->set_anim_setter(m_ptrAnimSet.get());
	m_ptrSelASPage->set_sound_setter(m_ptrSoundSet.get());
	m_ptrSelASPage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_BTN_ANIM_SOUND));

	m_ptrAddElePage.reset(new AddIEleDonePage);
	if (!m_ptrAddElePage.get())
		return false;
	m_ptrAddElePage->set_wizard(this);
	m_ptrAddElePage->set_obj_fabric(m_ptrObjFab.get());
//	m_ptrAddElePage->set_prev_page(m_ptrSelASPage.get());

	add_page(m_ptrSelNamePage.get());
//	add_page(m_ptrSelStateNamePage.get());
	add_page(m_ptrSelASPage.get());
	add_page(m_ptrAddElePage.get());

	return true;
}