#include "stdafx.h"
#include ".\restoreimagelayout.h"
#include "qd_game_object_moving.h"
#include "Main/StateTraits.h"

RestoreImageLayout::RestoreImageLayout(void)
{
}

RestoreImageLayout::~RestoreImageLayout(void)
{
}

/*!
 	��������������� ��������� ����������� ��� �����������(��������) ��������� ��
	������ ������� � ������.
	�������� �������� ���������� ��������� ������� � ������������� �������� ����������.
	�������� �������� ���������� ������� ������� � ������������� ����� ����������.
	��������� � �������� ������ ��������� ������� ����� ����������� ���� ��������.
	\remarks \a pnew �������� ������ \a psource
	\param psource	- �������� ���������
	\param po_src	- �������� ������
	\param pmo		- ������, � ������� ��������� ���������
	\param pnew		- ���������, ������� ���������
*/
void RestoreImageLayout::restore(qdGameObjectState* psource, 
								 qdGameObjectAnimated * po_src,
						qdGameObjectAnimated* pmo, qdGameObjectState* pnew)
{
	if (StateTraits::isWalkState(psource)||psource->coords_animation()->is_empty())
		return;

	qdGameObjectState* pCurState = po_src->get_cur_state();
	if (pCurState != psource)
	{
		po_src->set_state(psource);
		po_src->update_screen_pos();
	}
	Vect2i const scr_pos = po_src->screen_pos();
	if (pCurState != psource)
		po_src->set_state(pCurState);//��������������� �������

	pCurState = pmo->get_cur_state();
	pmo->set_state(pnew);
	pmo->update_screen_pos();
	Vect2i const scr_pos2 = pmo->screen_pos();

	pmo->set_state(pCurState);
	pmo->update_screen_pos();

	pnew->set_center_offset(pnew->center_offset()+(scr_pos - scr_pos2));
}

/*!
 	�������� ��������� ��������� �����(�������� \a pnew) � �������� ����������
	RestoreImageLayout(qdGameObjectState* psource, qdGameObjectAnimated * po_src,
	qdGameObjectAnimated* pmo, qdGameObjectState* pnew)
*/
void RestoreImageLayout::restore(qdGameObjectState* psource, 
						qdGameObjectAnimated* pmo, qdGameObjectState* pnew)
{
	ASSERT(psource);
	qdGameObjectAnimated* pobj = static_cast<qdGameObjectAnimated*>(psource->owner());
	restore(psource, pobj, pmo, pnew);
}

/*!
	����������� ��������� ����������� ��� ��������� ��� ����������� ������������� 
	������� � ��������
	\remarks \a pobj - ������ ���� ������ \a psrc
*/
void RestoreImageLayout::restore(qdGameObjectAnimated * psrc, qdGameObjectMoving *pobj)
{
	qdGameObjectStateVector const& v = psrc->state_vector();
	qdGameObjectStateVector const& v2 = pobj->state_vector();
	qdGameObjectStateVector::const_iterator i = v.begin(), e = v.end();
	qdGameObjectStateVector::const_iterator out = v2.begin();
	for(; i != e; ++i, ++out)
	{
		restore(*i, psrc, pobj, *out);
	}
}
