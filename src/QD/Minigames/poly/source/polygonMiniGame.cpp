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

struct ObjNode{
	qdMinigameObjectInterface*	obj;
	int ind;
	int st;
	ObjNode(): obj(NULL), st(-1), ind(-1){
	}
	~ObjNode(){}
};


class polygonMinigame : public qdMiniGameInterface
{
public:
	polygonMinigame();
	~polygonMinigame(){ }

	/// ������������� ����.
	bool init(const qdEngineInterface* p);

	/// ������ ������ ����, �������� - �����, ������� ������ ������ � ���� (� ��������).
	bool quant(float dt);

	/// ��������������� ����.
	bool finit();
private:
	/// �������� ��������� �������� �� �����
	void init_flags(void);

	/// ��������� ��� ���������� � �������
	void clear_all(void);
	
	/// �������� �� �������� �� X � Y � ������ �������
	int _2D21D(int x, int y) {return y * h_size + x;}
	
	/// �������� �� ������� ��������� ��������� � �������
	mgVect3f game2world_coords(int ind);
	
	/// ������ ������� �� ����� �������� ���������
	void set_z(qdMinigameObjectInterface* obj, float z);

	/// ����������/�������� ������
	void flash_on(qdMinigameObjectInterface* obj);
	void flash_off(qdMinigameObjectInterface* obj);

	/// ���
	qdMinigameObjectInterface* back_object;
		/// ���� ������
	qdMinigameObjectInterface* state_flag;


	typedef std::vector<qdMinigameObjectInterface*> field_container_t;
	/// ������� ���� �� ���������
	field_container_t field_objects;

	typedef std::vector<ObjNode*> chip_container_t;
	/// ����� ��� ���������� �� ����
	chip_container_t chip_array;

	typedef std::vector<int> int_container_t;
	/// ���������� ����������
	int_container_t win_array;
	/// ������� ������������ �����
	int_container_t chip_index;

	/// ������� �������� ������
	int CurrentActive;

	/// ������ ���� � ��������� �� ���������
	int v_size;
	/// ������ ���� � ��������� �� �����������
	int h_size;

	// ������� ����������� ����� ��������
	int shift_x;
	int shift_y;

	/// ������ � ������ ������
	int	img_width;
	int img_height;

	/// ��������� � ������
	const qdEngineInterface* engine_;
	/// ��������� � ������� �����
	qdMinigameSceneInterface* scene_;

	/// �������� �������� ����
	bool load_level(const char* fname);
	/// �������� �� ������� ����� � ������ ������ �� ���� � ��� ����� ����� ����
	bool is_chip(int x, int y, int self);
	/// �������� �� ���������� ������
	bool is_lock(int ind, int&, int self = -1);
	/// ����������� �����
	bool move(int source, int dest);
protected:
	
};

qdMiniGameInterface* open_game_interface(const char*)
{
	return new polygonMinigame;
}

bool close_game_interface(qdMiniGameInterface* p)
{
	p -> finit();
	delete p;
	return true;
}

mgVect3f polygonMinigame::game2world_coords(int ind){
	int x = ind % h_size; 
	int y = ind / h_size;

	int _x = x * img_width;
	int _y = y * img_height*3/4;

	_x += (y%2)?img_width/2:0;

	return scene_ -> screen2world_coords(mgVect2i(_x + shift_x, _y + shift_y));
}

polygonMinigame::polygonMinigame() : back_object(NULL),
	field_objects(NULL), chip_array(NULL)
{
	init_flags();
}

void polygonMinigame::clear_all(){
	for (chip_container_t::iterator akt_chip = chip_array.begin(); akt_chip != chip_array.end(); ++akt_chip){
		scene_ -> release_object_interface((*akt_chip)->obj);
		delete *akt_chip;
	}
	chip_array.clear();

	for (field_container_t::iterator akt_fi = field_objects.begin(); akt_fi != field_objects.end(); ++akt_fi)
		scene_ -> release_object_interface(*akt_fi);
	field_objects.clear();

	chip_index.clear();
	win_array.clear();
}

bool polygonMinigame::finit()
{
	if (NULL != scene_)
	{		
		clear_all();

		if(back_object)
			scene_ -> release_object_interface(back_object);

		if(state_flag)
			scene_ -> release_object_interface(state_flag);


		engine_ -> release_scene_interface(scene_);
		scene_ = NULL;
	}
	return true;
}

void polygonMinigame::init_flags(){
	h_size = 0;
	v_size = 0;
	
	img_width = 0;
	img_height = 0;

	CurrentActive = -1;
}

bool polygonMinigame::init(const qdEngineInterface* p)
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

bool polygonMinigame::quant(float dt)
{
	mgVect2i mouse = engine_ -> mouse_cursor_position();
	
	if(!engine_->is_mouse_event_active(qdEngineInterface::MOUSE_EV_LEFT_DOWN))
		return true;
	
	mgVect2i r;
	mouse = scene_ -> world2screen_coords(scene_ -> screen2grid_coords(mouse));
	int state = -1;

	// ������� � ����� ������ ������
	int ind = 0; int but_rad = img_width * img_width / 4;
	for (field_container_t::iterator akt_fi = field_objects.begin(); akt_fi != field_objects.end(); ++akt_fi, ind++){
		r = scene_ -> world2screen_coords((*akt_fi)->R());
		if((mouse.x - r.x) * (mouse.x - r.x) + (mouse.y - r.y) * (mouse.y - r.y) <= but_rad){
			state = (*akt_fi)->current_state_index();
			break;
		}
   	}

	// ���� ��� ������ � �� ������
	if(state < 0) return true;

	// ��������� ���� �� ��� ������������ ������
	if(CurrentActive == -1){
		// ������ ��� -> ������������ ������� � ���������� �
		if(chip_index[ind] == -1) return true; // ������ ��� ����� �� ������������
		CurrentActive = ind;
		flash_on(field_objects[ind]);
		return true;
	}

	// � ����� ������ ��, ��� ���� ���������� ����� ��������
	flash_off(field_objects[CurrentActive]);
	
	// ������� ������� �����
	bool is_moved = move(CurrentActive, ind);
	
	CurrentActive = -1;

	// ���� ������ �� �������� �� ������� ������ �� ���������
	if(!is_moved) 
		return true;
	
	bool the_win = true;
	for (ind = 0; ind < win_array.size(); ind++)
		if(chip_array[ind]->ind != win_array[ind]){
			the_win = false;
			break;
		}

	if(!the_win) return the_win;	
	
	// ���� ����� �� ����, �� ���� ��������
	// ��� ��������� ���� � ���� ���-�� ������
		state_flag->set_state("win");

	//
	
	return true;

}

bool polygonMinigame::is_chip(int x, int y, int self){
	// ��������� �� �������� �� �� ����
	if(x < 0 || x >= h_size || y < 0 || y >= v_size) return true;
	int ind = _2D21D(x, y);
	// ���� ���� �� ���������
	if(ind == self) return false;

	return (chip_index[ind] != -1);
}

bool polygonMinigame::is_lock(int ind, int &sosedi, int self){
	// c���� ��������� ��� �������� ������
	int sh = 1 - (ind / h_size) % 2;

	int x = ind % h_size;
	int y = ind / h_size;

	// �������� ���������� ����������
	bool round[7];
	round[0] = is_chip(x-sh,	y-1,	self);
	round[1] = is_chip(x-sh+1,	y-1,	self);
	round[2] = is_chip(x+1,		y,		self);
	round[3] = is_chip(x-sh+1,	y+1,	self);
	round[4] = is_chip(x-sh,	y+1,	self);
	round[5] = is_chip(x-1,		y,		self);
	round[6] = round[0];

	bool lock = true;

	int prev = round[0];
	sosedi = 0;
	// ���� ���� ����� ������� ����� � ������ �������
	for(int i = 1; i < 7; i++){
		int akt = round[i];
		sosedi += (int)akt;
		if(!prev && !akt)
			lock = false;
		prev = akt;
	}

	return lock;
}

bool polygonMinigame::move(int source, int dest){
	
	// ����� ������ ��� �������� � ��� �� �����
	if(chip_index[dest] >= 0) return false;
	
	int near_chip;
	// ���� ������� �������� ��� �������� ������ �� ����������
	if(is_lock(source, near_chip) || is_lock(dest, near_chip, source)) return false;

	if(near_chip < 2) return false;

	ObjNode* node = chip_array[chip_index[source]];

	if(node->st == 0) return false; // ���� ���������
	if(node->st > 0) node->st--;

	node->obj -> set_R(game2world_coords(dest));
	set_z(node->obj, 100);
	node->ind = dest;
	chip_index[dest] = chip_index[source];
	chip_index[source] = -1;
	
	return true;
}

bool polygonMinigame::load_level(const char* fname)
{
	std::fstream file;
	file.open(fname, std::ios::in);
	if (file.fail()) return false;

	// ������� ������ ��� ������ ���������� � init'�� ������;
	clear_all(); init_flags();

	file >> h_size >> v_size;
	int blocks_num = h_size * v_size;

	mgVect2i shift = scene_ -> world2screen_coords(back_object->R());

	char name[256];
	for (int i = 0; i < blocks_num; i++)
	{
		sprintf(name,"%s%.4d",scene_ -> minigame_parameter("poly_object_name"),i);
		if(qdMinigameObjectInterface* obj = scene_ -> object_interface(name))
		{
			if(!img_width){
				mgVect2i size = obj->screen_size();
				img_width = size.x;
				img_height= size.y;
				shift_x = shift.x - img_width * h_size / 2;
				shift_y = shift.y - img_height * v_size * 3 / 8;
			}
			
			obj -> set_state(0);
			obj -> set_R(game2world_coords(i));
			
			field_objects.push_back(obj);
		}
	}

	/// ���������� ���������� �������
	int states, steps;
	file >> states >> steps;

	chip_index.resize(blocks_num, -1);

	for(i = 0; i < states; i++){
		sprintf(name,"%s%.4d",scene_ -> minigame_parameter("chip_object_name"),i);
		if(qdMinigameObjectInterface* obj = scene_ -> object_interface(name)){
		
			/// ������ ������������ ���������
			int ind;
			file >> ind;

			chip_index[ind] = i;

			obj -> set_state(i);
			obj -> set_R(game2world_coords(ind));
			
			ObjNode *node = new ObjNode;
			
			set_z(obj, 100);
			node->obj = obj;
			node->st = steps;
			node->ind = ind;

			chip_array.push_back(node);
		}
	}

	// ������ ���������� ����������
	for(i = 0; i < states; i++){
		int ind;
		file >> ind;

		win_array.push_back(ind);
	}

	file.close();
	return true;
}

void polygonMinigame::set_z(qdMinigameObjectInterface* obj, float z){
	mgVect3f R = obj -> R();
	float deep1 = scene_ -> screen_depth(R);
	mgVect2i sc = scene_ -> world2screen_coords(R);
	R = scene_ -> screen2world_coords(sc, z);
	float deep2 = scene_ -> screen_depth(R);
	obj -> set_R(R);
}

void polygonMinigame::flash_on(qdMinigameObjectInterface* obj){
	set_z(obj, -100);
	obj->set_state(1);
}

void polygonMinigame::flash_off(qdMinigameObjectInterface* obj){
	set_z(obj, 0);
	obj->set_state(0);
}