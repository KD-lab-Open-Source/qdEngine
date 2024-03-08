#include "stdafx.h"
#include "animbycadr.h"
#include "qd_animation_maker.h"
#include "qd_animation.h"

AnimByCadr::AnimByCadr(void)
{
}

AnimByCadr::~AnimByCadr(void)
{
}

boost::shared_ptr<qdAnimation> AnimByCadr::make(LPCTSTR lpszFileName, float fCadrTime)
{
	boost::shared_ptr<qdAnimation> ptrAnim(new qdAnimation);
	if (!ptrAnim.get())
		return ptrAnim;

	ptrAnim->set_flag(QD_ANIMATION_FLAG_CROP|QD_ANIMATION_FLAG_COMPRESS);

	qdAnimationMaker anim_maker;
	anim_maker.set_default_frame_length(fCadrTime);
	int pos = -1;
	if (!anim_maker.insert_frame(ptrAnim.get(),lpszFileName, pos, true))
		ptrAnim.reset();
	return ptrAnim;
}
