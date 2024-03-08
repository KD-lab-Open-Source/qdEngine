#include "stdafx.h"
#include "moveocntr.h"
#include "qd_camera.h"
#include "qd_game_object_state.h"
#include "qd_game_object_moving.h"
#include "ptree_hlprs.hpp"

namespace{
	void apply_offset(qdCoordsAnimation* pca, const qdCamera* pcam, const Vect2s& off)
	{
		int const cnt = pca->size();
		for(int i = 0; i < cnt; ++i)
		{
			const Vect3f pos(pcam->global2camera_coord(pca->get_point(i)->dest_pos()));
			Vect2s scr_pos(pcam->camera_coord2scr(pos));
			scr_pos.x += off.x;
			scr_pos.y += off.y;
			scr_pos = pcam->scr2rscr(scr_pos);
			pca->get_point(i)->set_dest_pos(pcam->rscr2global(scr_pos, pos.z));
		}
	}

	void adjust_ca_z(qdCoordsAnimation* const pca, float z){
		size_t const cnt = pca->size();
		for(size_t i = 0; i < cnt; ++i)
		{
			Vect3f pos(pca->get_point(i)->dest_pos());
			pos.z = z;
			pca->get_point(i)->set_dest_pos(pos);
		}
	}
}

MoveOCntr::MoveOCntr(Vect2s off, qdGameObjectState* pstate, bool transform_correction):
  offset_(off)
, pstate_(pstate)
, transformCorrection_(transform_correction)
{
}

MoveOCntr::~MoveOCntr(void)
{
}

Vect2i MoveOCntr::operator()(const qdCamera* pcam)
{
	qdGameObjectAnimated* const pa = 
		static_cast<qdGameObjectAnimated*>(pstate_->owner());
	qdGameObjectState* const pCurState = pa->get_cur_state();
	bool bResetState = false;
	if (pCurState != pstate_)
	{
		pa->set_state(pstate_);
		bResetState = true;
	}

	pa->update_screen_pos();
	Vect2i screen_pos = pa->screen_pos();

	Vect3f pos(pcam->global2camera_coord(pa->R()));
	Vect2i scr_pos(pcam->camera_coord2scr(pos));

	scr_pos.x += offset_.x;
	scr_pos.y += offset_.y;
	scr_pos = pcam->scr2rscr(scr_pos);

	if (pstate_->coords_animation()->is_empty()) 
	{
		pa->set_pos(pcam->rscr2global(scr_pos, pos.z));
		pa->set_default_pos(pa->R());
		adjust_state_offsets(offset_);
	}
	else
	{
		qdCoordsAnimation* const pca = pstate_->coords_animation();
		apply_offset(pca, pcam, offset_);
		pca->reset_cur_point();
	}
	if (ptree_helpers::is_obj_moving(pa))
	{
		if (pstate_->coords_animation()->is_empty()) 
		{
			//производим выравнивание только в этом случае, 
			//потому что qdCoordsAnimation::reset_cur_point вызывает adjust_z()
			static_cast<qdGameObjectMoving*>(pa)->adjust_z();
			pa->set_default_pos(pa->R());
		}
		else
			adjust_ca_z(pstate_->coords_animation(), pcam->get_grid_center().z);
	}

	Vect2i delta = screen_pos - pcam->camera_coord2scr(pcam->global2camera_coord(pa->R()));

	if(transformCorrection_){
		Vect2f scale = pa->screen_scale();
		float alpha = pa->screen_rotation();

		offset_.x = round(float(delta.x) / scale.x * cosf(alpha) + float(delta.y) / scale.x * sinf(alpha));
		offset_.y = round(-float(delta.x) / scale.y * sinf(alpha) + float(delta.y) / scale.y * cosf(alpha));
	}
	else
		offset_ = delta;

	if (bResetState)
		pa->set_state(pCurState);

	return pstate_->center_offset() - offset_;
}

void MoveOCntr::adjust_state_offsets(const Vect2i& object_offset)
{
	qdGameObjectAnimated* p = static_cast<qdGameObjectAnimated*>(pstate_->owner());

	for(int i = 0 ; i < p->max_state(); i++){
		qdGameObjectState* st = p->get_state(i);
		if(st != pstate_){
			Vect2i object_offs = object_offset;

			if(st->has_transform()){
				float angle = st->transform().angle();

				float sn = sinf(angle);
				float cs = cosf(angle);

				object_offs.x = round(float(object_offset.x) * cs + float(object_offset.y) * sn);
				object_offs.y = round(-float(object_offset.x) * sn + float(object_offset.y) * cs);
			}

			Vect2i center_offs = st->center_offset() - object_offs;
			st->set_center_offset(center_offs);
		}
	}
}
