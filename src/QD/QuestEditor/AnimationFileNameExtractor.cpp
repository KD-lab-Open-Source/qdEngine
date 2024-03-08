#include "stdafx.h"
#include "animationfilenameextractor.h"
#include "qd_interface_background.h"
#include "qd_interface_slider.h"
#include "qd_interface_save.h"
#include "qd_interface_element_state.h"
#include "qd_game_object_state.h"

AnimationFileNameExtractor::AnimationFileNameExtractor(void)
{
}

AnimationFileNameExtractor::~AnimationFileNameExtractor(void)
{
}

LPCTSTR AnimationFileNameExtractor::getFileName(qdNamedObjectBase const *pob)
{
	if (qdGameObjectStateStatic const* const po = 
		dynamic_cast<qdGameObjectStateStatic const*>(pob))
	{
		return extractFromStaticState(po);
	}
	else if (qdInterfaceElementState const * const pes = 
		dynamic_cast<qdInterfaceElementState const*>(pob))
	{
		return extractFromInterfaceElementState(pes);
	}
	else if (qdInterfaceSave const* const psave =
		dynamic_cast<qdInterfaceSave const*>(pob))
	{
		return extractFromInterfaceSave(psave);
	}
	else if (qdInterfaceSlider const * const pslder = 
		dynamic_cast<qdInterfaceSlider const*>(pob))
	{
		return extractFromInterfaceSlider(pslder);
	}
	else if (qdInterfaceBackground const * const pback = 
		dynamic_cast<qdInterfaceBackground const*>(pob))
	{
		return extractFromInterfaceBackground(pback);
	}
	return NULL;
}

LPCTSTR AnimationFileNameExtractor::extractFromInterfaceBackground(
								class qdInterfaceBackground const* p)
{
	return p->animation_file();
}

LPCTSTR AnimationFileNameExtractor::extractFromInterfaceSlider(
								class qdInterfaceSlider const* p)
{
	return p->slider()->animation_file();
}

LPCTSTR AnimationFileNameExtractor::extractFromInterfaceSave(
								class qdInterfaceSave const* p)
{
	return p->frame_animation_file();
}

LPCTSTR AnimationFileNameExtractor::extractFromInterfaceElementState(
					class qdInterfaceElementState const* p)
{
	return p->animation_file();
}

LPCTSTR AnimationFileNameExtractor::extractFromStaticState(
							class qdGameObjectStateStatic const* p)
{
	ASSERT(p->state_type() == qdGameObjectState::STATE_STATIC);
	return p->animation()->qda_file();
};
