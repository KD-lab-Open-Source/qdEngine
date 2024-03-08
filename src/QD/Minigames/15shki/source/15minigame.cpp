//#include "stdafx.h"
#include <list>
#include <vector>
#include <fstream>

#include <math.h>
#include <stdio.h>

#include "qd_minigame_interface.h"

extern "C" {
__declspec(dllexport) qdMiniGameInterface* open_game_interface(const char*);
__declspec(dllexport) bool close_game_interface(qdMiniGameInterface* p);
};

class minigameSample : public qdMiniGameInterface
{
public:
	minigameSample();
	~minigameSample(){ }

	/// ������������� ����.
	bool init(const qdEngineInterface* p);

	/// ������ ������ ����, �������� - �����, ������� ������ ������ � ���� (� ��������).
	bool quant(float dt);

	/// ��������������� ����.
	bool finit();
private:
	// �������� �� �������� �� X � Y � ������ �������
	int _2D21Dcoord(int x, int y);
	
	void ShiftRow(int row, int dir);
	void ShiftCol(int col, int dir);

	/// ����
	qdMinigameObjectInterface* back_object;

	/// ���� ������
	qdMinigameObjectInterface* state_flag;

	typedef std::vector<qdMinigameObjectInterface*> butt_container_t;
	/// ������ ����������
	butt_container_t butt_array;

	//������ ���� �� ���������
	int v_size;
	//������ ���� �� �����������
	int h_size;

	// ������� ����������� ����� ��������
	int shift_x;
	int shift_y;

	int	img_width;
	int img_height;

	typedef std::vector<qdMinigameObjectInterface*> field_container_t;
	/// ������� ����
	field_container_t field_objects;
	
	typedef std::vector<int> field_objects_win_t;
	field_objects_win_t field_objects_win;

	/// ��������� � ������
	const qdEngineInterface* engine_;
	/// ��������� � ������� �����
	qdMinigameSceneInterface* scene_;

	mgVect3f game2world_coords(int x, int y);
	/// �������� �������� ����
	bool load_level(const char* fname);
};

qdMiniGameInterface* open_game_interface(const char*)
{
	return new minigameSample;
}

bool close_game_interface(qdMiniGameInterface* p)
{
	p -> finit();
	delete p;
	return true;
}

mgVect3f minigameSample::game2world_coords(int x, int y){
	x *= img_width;
	y *= img_height;

	return scene_ -> screen2world_coords(mgVect2i(x + shift_x, y + shift_y));
}

minigameSample::minigameSample() : back_object(NULL),
	butt_array(NULL), field_objects(NULL), state_flag(NULL),
	h_size(0), v_size(0),
	img_width(0), img_height(0)
{
}

bool minigameSample::finit()
{
	if (NULL != scene_)
	{		
		for (butt_container_t::iterator akt_butt = butt_array.begin(); akt_butt != butt_array.end(); ++akt_butt)
			scene_ -> release_object_interface(*akt_butt);
		butt_array.clear();
	
		for (field_container_t::iterator akt_fi = field_objects.begin(); akt_fi != field_objects.end(); ++akt_fi)
			scene_ -> release_object_interface(*akt_fi);
		field_objects.clear();

		if(back_object)
			scene_ -> release_object_interface(back_object);
		if(state_flag)
			scene_ -> release_object_interface(state_flag);
		
		engine_ -> release_scene_interface(scene_);
		scene_ = NULL;
	}
	return true;
}

bool minigameSample::init(const qdEngineInterface* p)
{
	engine_ = p;
	if(scene_ = p -> current_scene_interface()){
		back_object = scene_ -> object_interface(scene_ -> minigame_parameter("back_object_name"));
		state_flag = scene_ -> object_interface("state_flag");
		if(!state_flag){
			finit();
			return false;
		}else{
			state_flag->set_state("game");
		}

		const char* file_name = scene_ -> minigame_parameter("map_file_name");
		if(!load_level(file_name)){
			finit();
			return false;
		}
	} else
		return false;
	return true;
}

bool minigameSample::quant(float dt)
{
	
	mgVect2i mouse = engine_ -> mouse_cursor_position();
	
	if(!engine_->is_mouse_event_active(qdEngineInterface::MOUSE_EV_LEFT_DOWN))
		return true;
	
	mgVect2i r;
	mouse = scene_ -> world2screen_coords(scene_ -> screen2grid_coords(mouse));
	int state = -1;

	// ������� � ����� ������ ������
	int ind = 0;
	for (butt_container_t::iterator akt_butt = butt_array.begin(); akt_butt != butt_array.end(); ++akt_butt, ind++){
		r = scene_ -> world2screen_coords((*akt_butt)->R()) - mgVect2i(img_width/2, img_height/2);
		if(mouse.x >= r.x && mouse.x <= r.x + img_width &&
			mouse.y >= r.y && mouse.y <= r.y + img_height){
			state = (*akt_butt)->current_state_index();
			break;
		}
   	}

	// ���� �������� �� ����� �� ��-�� ��������� � ������
	if(state < 0) return true;
	
	
	switch(state){ // �������� ������ ��� �������
	case 0: // �����
		ShiftCol(ind, -1);
		break;
	case 1: // �����
		ShiftRow((ind - h_size)/2, -1);
		break;
	case 2: // ������
		ShiftRow((ind - h_size - 1)/2, 1);
		break;
	case 3: // ����
		ShiftCol(ind - h_size - 2*v_size, 1);
		break;
	default:
		return true;
	}
	
	bool fl_win = true;
	// �������� ������� ������
	ind = 0;
	for (ind = 0; ind < v_size*h_size; ind++){
		int i1 = field_objects_win[ind];
		int i2 = field_objects[ind]->current_state_index();
		if(i1 != i2){
			fl_win = false;
		}
	}

/*		if(field_objects_win[ind] != field_objects[ind]->current_state_index()){
			fl_win = false;
			break;
		}
*/	

	if(!fl_win) // ���� �� ��������, ������ ������
		return true;

	// ��������� �������� ������
	state_flag->set_state("win");

	return true;

}


bool minigameSample::load_level(const char* fname)
{
	std::fstream file;
	file.open(fname, std::ios::in);
	if (file.fail()) return false;

	// ������� ���������� ��� �������
	for (field_container_t::iterator akt_fi = field_objects.begin(); akt_fi != field_objects.end(); ++akt_fi)
		scene_ -> release_object_interface(*akt_fi);
	field_objects.clear();
	for (butt_container_t::iterator akt_butt = butt_array.begin(); akt_butt != butt_array.end(); ++akt_butt)
			scene_ -> release_object_interface(*akt_butt);
	butt_array.clear();

	
	file >> h_size >> v_size;
	int blocks_num = h_size * v_size;

	mgVect2i shift = scene_ -> world2screen_coords(back_object->R());

	char name[256];
	for (int i = 0; i < blocks_num; i++)
	{
		sprintf(name,"%s%.4d",scene_ -> minigame_parameter("field_object_name"),i);
		if(qdMinigameObjectInterface* obj = scene_ -> object_interface(name))
		{
			int state;
			
			file >> state;

			int y = i / h_size;
			int x = i % h_size;
			
			if(!img_width){
				mgVect2i size = obj->screen_size();
				img_width = size.x;
				img_height= size.y;
				shift_x = shift.x - img_width * h_size / 2;
				shift_y = shift.y - img_height * v_size / 2;
			}
			
			obj -> set_state(state);
			obj -> set_R(game2world_coords(x, y));

			field_objects.push_back(obj);
		}
	}

	// ������ ������� ������
	for (i = 0; i < blocks_num; i++)
	{
		int state;
		file >> state;

		field_objects_win.push_back(state);
	}
	
	// ������� ������� ������
	int butt_num = 2 * (h_size + v_size);
	for (i = 0; i < butt_num; i++)
	{
		sprintf(name,"%s%.4d",scene_ -> minigame_parameter("butt_object_name"),i);
		if(qdMinigameObjectInterface* obj = scene_ -> object_interface(name))
			butt_array.push_back(obj);
	}

	// ������ ������
	for(i = 0; i < h_size; i++){
		qdMinigameObjectInterface* obj = butt_array[i];
		obj->set_state(0); // ������ �����
		
		int x = i;
		int y = -1;

		obj->set_R(game2world_coords(x, y));
		
		obj = butt_array[i + 2 * v_size + h_size];
		obj->set_state(3); // ������ ���� state = 3
		obj->set_R(game2world_coords(x, y + v_size + 1));
	}
	
	for(i = 0; i < v_size; i++){
		qdMinigameObjectInterface* obj = butt_array[h_size + 2 * i];
		obj->set_state(1); // ������ ����� state = 1
		
		int x = -1;
		int y = i;

		obj->set_R(game2world_coords(x, y));
	   
		obj = butt_array[h_size + 2 * i + 1];
		obj->set_state(2); // ������ ������ state = 2
		obj->set_R(game2world_coords(x + h_size + 1, y));
	}

	  
	file.close();
	return true;
}

void minigameSample::ShiftCol(int col, int dir)
{
	int _x = col;
	int _y = (dir == 1) ? v_size-1 : 0;

	int save = field_objects[_2D21Dcoord(_x, _y)]->current_state_index();
	for(int i = 0; i < v_size-1; i++){
		field_objects[_2D21Dcoord(_x, _y)]->set_state(field_objects[_2D21Dcoord(_x, _y-dir)]->current_state_index());
		_y -= dir;
	}
	field_objects[_2D21Dcoord(_x, _y)]->set_state(save);

}

void minigameSample::ShiftRow(int row, int dir)
{
	int _x = (dir == 1) ? h_size-1 : 0;
	int _y = row;

	int save = field_objects[_2D21Dcoord(_x, _y)]->current_state_index();
	for(int i = 0; i < h_size-1; i++){
		field_objects[_2D21Dcoord(_x, _y)]->set_state(field_objects[_2D21Dcoord(_x-dir, _y)]->current_state_index());
		_x -= dir;
	}
	field_objects[_2D21Dcoord(_x, _y)]->set_state(save);
}

int minigameSample::_2D21Dcoord(int x, int y)
{
	if(x > h_size)	return _2D21Dcoord(x - h_size, y);
	if(x < 0)		return _2D21Dcoord(x + h_size, y);
	if(y > v_size)	return _2D21Dcoord(x, y - v_size);
	if(y < 0)		return _2D21Dcoord(x, y + v_size);
	return y * h_size + x;
}
