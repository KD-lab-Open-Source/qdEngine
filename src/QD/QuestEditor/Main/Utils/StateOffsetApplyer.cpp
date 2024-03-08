#include "stdafx.h"

#include ".\stateoffsetapplyer.h"
#include "obj_type_detectors.h"

#include "qd_game_object_animated.h"
#include "qd_animation.h"

#include "Main/StateTraits.h"

#include <boost/rtl.hpp>

///	 \brief ��� ������� ������� ��������� , ������� ���������� �������� �� 
///	offset_states::anims_ � ��������� ��� ��������
class  offset_states
{
	//! ���������, ������� ������� ������
	qdGameObjectStateStatic const* const pst_ex_;
	//! ��������, �� ������� ����� ������� ��������� ���������
	Vect2i const&  offs_;
	//! \brief ��������� ��������, ������� ���������� �������� �� �����
	//! ������, �������� ��������
	std::vector<LPCTSTR> const& anims_;
	//! �������� � pst_ex_ �������� �������������
	bool bFlipHorz_;
	//! �������� � pst_ex_ �������� �����������
	bool bFlipVert_;
public:
	offset_states(qdGameObjectStateStatic const* pst_exclude, Vect2i const& offs,
		std::vector<LPCTSTR> const& anims):
		pst_ex_(pst_exclude)
		, offs_(offs)
		, anims_(anims)
		{
			bFlipHorz_ = pst_ex_->animation_info()->
				check_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL);
			bFlipVert_ = pst_ex_->animation_info()->
				check_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL);
		}

	void operator()(qdNamedObject* pobj)
	{
		qdGameObjectAnimated* const panim = static_cast<qdGameObjectAnimated*>(pobj);
		boost::rtl::filtered_range
			<
			qdGameObjectStateVector const,
			bool (*)(qdGameObjectState const*)
			> r(panim->state_vector(), StateTraits::isStaticState);

		for (; r; ++r)
		{
			qdGameObjectStateStatic* const pst = static_cast<qdGameObjectStateStatic*>(*r);
			if (pst != pst_ex_)
			{
				boost::rtl::crange<std::vector<LPCTSTR> const> sr(anims_);
				for(; sr; ++sr)
				{
					LPCTSTR const name = pst->animation_info()->name();
					if (name&&!_tcscmp(name, *sr))
					{
						//! ��� �������� ��������� � ������. ����� �������.
						Vect2i offs(offs_);
						if (pst->animation_info()->
							check_flag(QD_ANIMATION_FLAG_FLIP_HORIZONTAL) != bFlipHorz_)
							offs.x = -offs.x;

						if (pst->animation_info()->
							check_flag(QD_ANIMATION_FLAG_FLIP_VERTICAL) != bFlipVert_)
							offs.y = -offs.y;

						qdAnimation* pa = pst->animation_info()->animation();
						if (pa) 
							pa->load_resources();//! ������ ��� ���� �������� ��� �������
						pst->set_center_offset(pst->center_offset() + offs);
						break;
					}
				}
			}
		}
	}
};

StateOffsetApplyer::StateOffsetApplyer(void)
{
}

StateOffsetApplyer::~StateOffsetApplyer(void)
{
}


void StateOffsetApplyer::apply(qdGameObjectStateStatic const* query_state, 
		   Vect2i const& offset, 
		   qdGameScene* pscene,
		   std::vector<LPCTSTR> const& anim_lst)
{
	boost::rtl::filtered_range<
		qdGameObjectList const,
		bool (*)(qdNamedObject const*)
	> r(pscene->object_list(), ptree_helpers::is_animated);
	boost::rtl::rng::for_each(r, offset_states(query_state, offset, anim_lst));
}
