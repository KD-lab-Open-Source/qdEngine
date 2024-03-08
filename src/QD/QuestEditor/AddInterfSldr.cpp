#include "stdafx.h"
#include "QuestEditor.h"
#include "addinterfsldr.h"

#include "ptree_api.h"
#include "EdgeRemover.h"
#include <ShlwApi.h>

#include "AddSldrData.h"
#include "SelectNameMWPage.h"
#include "SelectAnimSoundMWPage.h"
#include "AddIEleDonePage.h"
#include "name_setter.h"
#include "screen_ele_name_test.h"

#include "qd_interface_slider.h"
#include "ConvertTGAtoQDA.h"

/**
	Реализация SelectAnimSoundMWPage::text_set, которая назначает 
	слайдеру анимацию для бегунка
 */
class ForegrAnimSet : public SelectAnimSoundMWPage::text_set
{
	AddSldrData * pdata_;
public:
	ForegrAnimSet(AddSldrData* pdata):pdata_(pdata){}
	bool set_text(const CString& str){
		if (PathFileExists(str))
		{
			pdata_->set_foregr_anim_file(str);
			return true;
		}
		return false;
	}
	const CString& get_text()const {
		return pdata_->foregr_anim_file();
	}
};

/**
	Реализация SelectAnimSoundMWPage::text_set, которая назначает 
	слайдеру звук для бегунка
 */

class ForegrSoundSet : public SelectAnimSoundMWPage::text_set
{
	AddSldrData * pdata_;
public:
	ForegrSoundSet(AddSldrData* pdata):pdata_(pdata){}
	bool set_text(const CString& str){
		if (str.IsEmpty()||PathFileExists(str))
		{
			pdata_->set_foregr_sound_file(str);
			return true;
		}
		return false;
	}
	const CString& get_text()const {
		return pdata_->foregr_sound_file();
	}
};

/**
	Реализация SelectAnimSoundMWPage::text_set, которая назначает 
	слайдеру анимацию для фона слайдера
 */
class BackAnimSet : public SelectAnimSoundMWPage::text_set
{
	AddSldrData * pdata_;
public:
	BackAnimSet(AddSldrData* pdata):pdata_(pdata){}
	bool set_text(const CString& str){
		if (str.IsEmpty()||PathFileExists(str))
		{
			pdata_->set_back_anim_file(str);
			return true;
		}
		return false;
	}
	const CString& get_text()const {
		return pdata_->back_anim_file();
	}
};

/**
	Реализация фабрики создающая слайдер
 */
class SldrObjFabric : public AddIEleDonePage::obj_fabric
{
	/// Данные визарда
	AddSldrData* pdata_;
	/// Конвртирует файлы анимации в QDA форамат
	bool convertToQDA()
	{
		ConvertTGAtoQDA convertor;
		CString res = convertor.convert(pdata_->foregr_anim_file());
		if (res.IsEmpty())
			return false;
		pdata_->set_foregr_anim_file(res);

		res = convertor.convert(pdata_->back_anim_file());
		if (res.IsEmpty())
			return false;
		pdata_->set_back_anim_file(res);
		return true;
	}
public:
	explicit SldrObjFabric (AddSldrData* pdata): pdata_(pdata){}
	bool create_obj(){
		/// Добавляет слайдер
		qdInterfaceSlider* psldr = 
			ptree_api::AddInterfSldr(pdata_->name(), pdata_->screen());
		if (!psldr)
			return false;

		convertToQDA();
		if (AfxMessageBox(IDS_ASK_REMOVE_EDGES, MB_YESNO) == IDYES)
		{
			EdgeRemover er;
			er.remove_edges(pdata_->foregr_anim_file());
			er.remove_edges(pdata_->back_anim_file());
		}
		qdInterfaceElementState st(*psldr->slider());
		st.set_animation_file(pdata_->foregr_anim_file());

		if (pdata_->foregr_sound_file().IsEmpty())
			st.set_sound_file(NULL);
		else
			st.set_sound_file(pdata_->foregr_sound_file());
		psldr->update_slider(st);
		
		st = *psldr->background();
		st.set_sound_file(NULL);
		if (pdata_->back_anim_file().IsEmpty())
			st.set_animation_file(NULL);
		else
			st.set_animation_file(pdata_->back_anim_file());

		psldr->update_background(st);
		psldr->init();
		/// Выставляем на центр экрана
		psldr->set_r(Vect2i(psldr->size_x()>>1, psldr->size_y()>>1));
		return true;
	}
};

AddInterfSlider::AddInterfSlider(void)
{
}

AddInterfSlider::~AddInterfSlider(void)
{
}

bool AddInterfSlider::Construct(qdInterfaceScreen* pscr)
{
	/// все создаем
	m_ptrData.reset(new AddSldrData(pscr));
	if (!m_ptrData.get())
		return false;

	m_ptrSldrObjFabr.reset(new SldrObjFabric(m_ptrData.get()));
	if (!m_ptrSldrObjFabr.get())
		return false;

	m_ptrBackAnimSet.reset(new BackAnimSet(m_ptrData.get()));
	if (!m_ptrBackAnimSet.get())
		return false;

	m_ptrForegrSoundSet.reset(new ForegrSoundSet(m_ptrData.get()));
	if (!m_ptrForegrSoundSet.get())
		return false;

	m_ptrForegrAnimSet.reset(new ForegrAnimSet(m_ptrData.get()));
	if (!m_ptrForegrAnimSet.get())
		return false;

	m_ptrNameSetter.reset(new NameSetter(m_ptrData.get()));
	if (!m_ptrNameSetter.get())
		return false;

	m_ptrSelName.reset(new SelectNameMWPage);
	if (!m_ptrSelName.get())
		return false;

	m_ptrNameTest.reset(new screen_ele_name_test(pscr));
	if (!m_ptrNameTest.get())
		return false;

	/// создаем и увязыаем
	m_ptrSelName->set_wizard(this);
	m_ptrSelName->SetNameSetter(m_ptrNameSetter.get());
	m_ptrSelName->SetNameTester(m_ptrNameTest.get());
	m_ptrSelName->SetCaption(CString((LPTSTR)IDS_PAGE_CAP_DEF_SEL_NAME));

	m_ptrSelASForForeground.reset(new SelectAnimSoundMWPage);
	if (!m_ptrSelASForForeground.get())
		return false;
	m_ptrSelASForForeground->set_wizard(this);
	m_ptrSelASForForeground->set_prev_page(m_ptrSelName.get());
	m_ptrSelASForForeground->set_anim_setter(m_ptrForegrAnimSet.get());
	m_ptrSelASForForeground->set_sound_setter(m_ptrForegrSoundSet.get());
	m_ptrSelASForForeground->
		SetCaption(CString((LPTSTR)IDS_PAGE_CAP_SLDR_FOREGR_ANIM_AND_SOUND));

	m_ptrSelASForBack.reset(new SelectAnimSoundMWPage);
	if (!m_ptrSelASForBack.get())
		return false;
	m_ptrSelASForBack->set_wizard(this);
	m_ptrSelASForBack->set_prev_page(m_ptrSelASForForeground.get());
	m_ptrSelASForBack->set_anim_setter(m_ptrBackAnimSet.get());
	m_ptrSelASForBack->
		SetCaption(CString((LPTSTR)IDS_PAGE_CAP_SLDR_BACK_ANIM));

	m_ptrDonePg.reset(new AddIEleDonePage);
	if (!m_ptrDonePg.get())
		return false;
	m_ptrDonePg->set_wizard(this);
	m_ptrDonePg->set_prev_page(m_ptrSelASForBack.get());
	m_ptrDonePg->set_obj_fabric(m_ptrSldrObjFabr.get());

	add_page(m_ptrSelName.get());
	add_page(m_ptrSelASForForeground.get());
	add_page(m_ptrSelASForBack.get());
	add_page(m_ptrDonePg.get());
	return true;
}
