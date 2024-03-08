/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "qd_precomp.h"

#include "AIAStar_API.h"

#include "qd_camera.h"
#include "qd_game_object_moving.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

qdHeuristic::qdHeuristic() : camera_ptr_(NULL), object_ptr_(NULL)
{
}

qdHeuristic::~qdHeuristic()
{
}

int qdHeuristic::GetH(int x,int y)
{
	x -= target_.x;
	y -= target_.y;

	//return sqrt(static_cast<float>(x * x + y * y));
	// ���������� ����� ��������� ��� ����������� �����, ������� ���������
	return static_cast<float>(x * x + y * y);
}

int qdHeuristic::GetG(int x1,int y1,int x2,int y2)
{
	if(!object_ptr_ -> is_walkable(Vect2s(x2,y2)))
		return 10000;
	// ��� ������������ ����������� ������� ��� � ����������� �� �������,
	// ������ ��� ���� ����� ������� �������� ��-�� ������������ ���������������� �������
	if ((x1 != x2) && (y1 != y2) &&
		(
		  !object_ptr_ -> is_walkable(Vect2s(x1,y2)) ||
		  !object_ptr_ -> is_walkable(Vect2s(x2,y1))
		))
		return 10000;
	
	//return abs(x2 - x1) + abs(y2 - y1);
	// 14 - ����������� ����� �� 2, ����������� �� 10. 10 - �������*10. 
	if ((x1 != x2) && (y1 != y2))
		return 14;
	else return 10;
}

void qdHeuristic::init(const Vect3f trg)
{
	target_f_ = trg;
	target_ = camera_ptr_ -> get_cell_index(trg.x,trg.y);
}

