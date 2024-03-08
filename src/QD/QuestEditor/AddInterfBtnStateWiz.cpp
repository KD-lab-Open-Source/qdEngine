#include "stdafx.h"
#include "resource.h"
#include "addinterfbtnstatewiz.h"

#include "AddInterfBtnStateWizData.h"
#include "SelectNameMWPage.h"
#include "SelectAnimSoundMWPage.h"
#include "AddIEleDonePage.h"
#include "name_setter.h"
#include "ptree_api.h"
#include "qd_interface_button.h"

#include "interf_state_util.h"
#include "EdgeRemover.h"
#include "ptree_api.h"
#include "ConvertTGAtoQDA.h"

/**
	Фабрика, создающая кнопочное состояние и добавляющая его кнопке
 */
class BtnStateFabric : public AddIEleDonePage::obj_fabric
{
	/// Данные визарда. Хранит все необъодимые для создания данные
	AddInterfBtnStateWizData* pdata_;
	/// Назначает анимацию и звук режиму состояния
	void set_anim_and_sound(qdInterfaceElementState& st, 
		qdInterfaceElementState::state_mode_t mode)
	{
		st.set_animation_file(pdata_->anim_name(), mode);
		if (pdata_->sound_name().IsEmpty())
			st.set_sound_file(NULL, mode);
		else
			st.set_sound_file(pdata_->sound_name(), mode);
	}
	/// Конвертирует файл анимации в формат QDA
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
	explicit BtnStateFabric(AddInterfBtnStateWizData* pdata): pdata_(pdata){}
	bool create_obj(){
		qdInterfaceButton* const pbtn = pdata_->button();
		if ( !convertToQDA())
			return false;
		EdgeRemover::remove_edges(pdata_->anim_name());
		int iStateNum = 
			ptree_api::AddInterfBtnState(pdata_->name(), pbtn);


		qdInterfaceElementState* pst(pbtn->get_state(iStateNum));
		set_anim_and_sound(*pst, qdInterfaceElementState::DEFAULT_MODE);
		set_anim_and_sound(*pst, qdInterfaceElementState::EVENT_MODE);
		set_anim_and_sound(*pst, qdInterfaceElementState::MOUSE_HOVER_MODE);

		pbtn->activate_state(0);

		pbtn->init();
		return true;
	}
};


AddInterfBtnStateWiz::AddInterfBtnStateWiz(void)
{
}

AddInterfBtnStateWiz::~AddInterfBtnStateWiz(void)
{
}

bool AddInterfBtnStateWiz::Construct(qdInterfaceButton* pbtn)
{
	m_ptrData.reset(new AddInterfBtnStateWizData(pbtn));
	if (!m_ptrData)
		return false;
	m_ptrNameSetter.reset(new NameSetter(m_ptrData.get()));
	if (!m_ptrNameSetter)
		return false;

	m_ptrAnimSet.reset(new StateAnimSet(m_ptrData.get()));
	if (!m_ptrAnimSet)
		return false;

	m_ptrSoundSet.reset(new StateSoundSet(m_ptrData.get()));
	if (!m_ptrSoundSet)
		return false;

	m_ptrNameTester.reset(new interf_btn_state_name_test(pbtn));
	if (!m_ptrNameTester)
		return false;

	m_ptrSelStateNamePage.reset(new SelectNameMWPage);
	if (!m_ptrSelStateNamePage)
		return false;

	m_ptrFabric.reset(new BtnStateFabric(m_ptrData.get()));
	if (!m_ptrFabric)
		return false;

	m_ptrSelStateNamePage->set_wizard(this);
	m_ptrSelStateNamePage->SetNameSetter(m_ptrNameSetter.get());
	m_ptrSelStateNamePage->SetNameTester(m_ptrNameTester.get());
	m_ptrSelStateNamePage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_SELECT_STATE_NAME));

	m_ptrSelASPage.reset(new SelectAnimSoundMWPage);
	if (!m_ptrSelASPage.get())
		return false;
	m_ptrSelASPage->set_wizard(this);
	m_ptrSelASPage->set_prev_page(m_ptrSelStateNamePage.get());
	m_ptrSelASPage->set_anim_setter(m_ptrAnimSet.get());
	m_ptrSelASPage->set_sound_setter(m_ptrSoundSet.get());
	m_ptrSelASPage->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_BTN_ANIM_SOUND));

	m_ptrAddElePage.reset(new AddIEleDonePage);
	if (!m_ptrAddElePage.get())
		return false;
	m_ptrAddElePage->set_wizard(this);
	m_ptrAddElePage->set_obj_fabric(m_ptrFabric.get());
	m_ptrAddElePage->set_prev_page(m_ptrSelASPage.get());

	add_page(m_ptrSelStateNamePage.get());
	add_page(m_ptrSelASPage.get());
	add_page(m_ptrAddElePage.get());

	return true;
}
