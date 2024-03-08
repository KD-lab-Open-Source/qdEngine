#include "stdafx.h"
#include "converttgatoqdaininterface.h"
#include "ConvertTGAtoQDA.h"

#include <algorithm>
#include <functional>
#include <typeinfo>

#include "qd_interface_screen.h"
#include "qd_interface_slider.h"
#include "qd_interface_save.h"
#include "qd_interface_element_state.h"
#include "qd_interface_button.h"

#include "disk_op.h"


ConvertTGAtoQDAInInterface::ConvertTGAtoQDAInInterface(void)
{
}

ConvertTGAtoQDAInInterface::~ConvertTGAtoQDAInInterface(void)
{
}

void ConvertTGAtoQDAInInterface::convert(qdNamedObjectBase* pobj) const
{
	type_info const& ti = typeid(*pobj);
	if (ti == typeid(qdInterfaceScreen))
		convertTGAtoQDAInInterfaceScreen(static_cast<qdInterfaceScreen*>(pobj));
	else if (ti == typeid(qdInterfaceButton))
		convertTGAtoQDAInInterfaceButton(static_cast<qdInterfaceButton*>(pobj));
	else if (ti == typeid(qdInterfaceSlider))
		convertTGAtoQDAInInterfaceSlider(static_cast<qdInterfaceSlider*>(pobj));
	else if (ti == typeid(qdInterfaceSave))
		convertTGAtoQDAInInterfaceSave(static_cast<qdInterfaceSave*>(pobj));
	else if (ti == typeid(qdInterfaceElementState))
		convertTGAtoQDAInInterfaceElementState(
								static_cast<qdInterfaceElementState*>(pobj));
}

void ConvertTGAtoQDAInInterface::convertTGAtoQDAInInterfaceScreen(
											qdInterfaceScreen* pobj) const
{
	qdInterfaceScreen::element_list_t const& lst = pobj->element_list();
	std::for_each(lst.begin(), lst.end(), 
		std::bind1st(std::mem_fun(ConvertTGAtoQDAInInterface::convert), this));
	pobj->init();
}

void ConvertTGAtoQDAInInterface::convertTGAtoQDAInInterfaceButton(
											qdInterfaceButton* pobj) const
{
	int const num = pobj->num_states();
	for(int i = 0; i < num; ++i)
		convertTGAtoQDAInInterfaceElementState(pobj->get_state(i));
	pobj->init();
}

void ConvertTGAtoQDAInInterface::convertTGAtoQDAInInterfaceSlider(
											qdInterfaceSlider* pobj) const
{
	if (disk_op::GetImageFileType(pobj->
				background()->animation_file()) == disk_op::FT_TGA)
	{
		ConvertTGAtoQDA convertor;
		CString res = convertor.convert(pobj->background()->animation_file());
		qdInterfaceElementState state(*pobj->background());
		state.set_animation_file(res);
		pobj->update_background(state);
	}

	if (disk_op::GetImageFileType(pobj->slider()->animation_file()) == disk_op::FT_TGA)
	{
		ConvertTGAtoQDA convertor;
		CString res = convertor.convert(pobj->slider()->animation_file());
		qdInterfaceElementState state(*pobj->slider());
		state.set_animation_file(res);
		pobj->update_slider(state);
	}
	pobj->init();
}

void ConvertTGAtoQDAInInterface::convertTGAtoQDAInInterfaceSave(
											qdInterfaceSave* pobj) const
{
	if (disk_op::GetImageFileType(pobj->frame_animation_file()) == disk_op::FT_TGA)
	{
		ConvertTGAtoQDA convertor;
		CString res = convertor.convert(
			pobj->frame_animation_file());
		pobj->set_frame_animation_file(res);
		pobj->init();
	}
}

void ConvertTGAtoQDAInInterface::convertTGAtoQDAInInterfaceElementState(
											qdInterfaceElementState* pobj) const
{
	ConvertTGAtoQDA convertor;
	if (disk_op::GetImageFileType(pobj->animation_file(
			qdInterfaceElementState::DEFAULT_MODE)) == disk_op::FT_TGA)
	{
		CString const res = convertor.convert(
			pobj->animation_file(qdInterfaceElementState::DEFAULT_MODE));
		pobj->set_animation_file(res, qdInterfaceElementState::DEFAULT_MODE);
	}
	if (disk_op::GetImageFileType(pobj->animation_file(
		qdInterfaceElementState::EVENT_MODE)) == disk_op::FT_TGA)
	{
		CString const res = convertor.convert(
			pobj->animation_file(qdInterfaceElementState::EVENT_MODE));
		pobj->set_animation_file(res, qdInterfaceElementState::EVENT_MODE);
	}
	if (disk_op::GetImageFileType(pobj->animation_file(
		qdInterfaceElementState::MOUSE_HOVER_MODE)) == disk_op::FT_TGA)
	{
		CString const res = convertor.convert(
			pobj->animation_file(qdInterfaceElementState::MOUSE_HOVER_MODE));
		pobj->set_animation_file(res, qdInterfaceElementState::MOUSE_HOVER_MODE);
	}
}
