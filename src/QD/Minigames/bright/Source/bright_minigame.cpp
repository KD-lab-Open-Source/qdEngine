#include <list>
#include <vector>
#include <fstream>

//#include <math.h>
#include <stdio.h>

#include "qd_minigame_interface.h"

extern "C" {
__declspec(dllexport) qdMiniGameInterface* open_game_interface(const char*);
__declspec(dllexport) bool close_game_interface(qdMiniGameInterface* p);
};


struct ObjNode{
	// ������� ����
	qdMinigameObjectInterface*	field;
	// ������� ��������
	qdMinigameObjectInterface*	chip;
	// ������� �������� � ���������� ��������� �������
	qdMinigameObjectInterface*	bw;
	// �����������
	qdMinigameObjectInterface*	mod;
	// ����������� ���������
	int state;
	ObjNode(): field(NULL), chip(NULL), bw(NULL), mod(NULL), state(-1){}
	~ObjNode(){}
};


class bright_minigame : public qdMiniGameInterface
{
public:
	bright_minigame();
	~bright_minigame(){ }

	// ������������� ����.
	bool init(const qdEngineInterface* p);

	// ������ ������ ����, �������� - �����, ������� ������ ������ � ���� (� ��������).
	bool quant(float dt);

	// ��������������� ����.
	bool finit();
private:
	// �������� ��������� �������� �� �����
	void init_flags(void);

	// ��������� ��� ���������� � �������
	void clear_all(void);
	
	// �������� �� �������� �� X � Y � ������ �������
	int _2D21D(int x, int y) {return y * h_size + x;}
	
	// �������� �� ������� ��������� ��������� � �������
	mgVect3f game2world_coords(int ind, float deep = 0);
	
	// ������ ������� �� ����� �������� ���������
	void set_z(qdMinigameObjectInterface* obj, float z){
		obj -> set_R(scene_->screen2world_coords(scene_->world2screen_coords(obj->R()), z)); }

	// ����������/�������� ������
	void flash_on(ObjNode* obj);
	void flash_off(ObjNode* obj);

	// ���
	qdMinigameObjectInterface* back_object;

	/// ���� ������
	qdMinigameObjectInterface* state_flag;


	typedef std::vector<ObjNode*> field_container_t;
	// ������� ����
	field_container_t field_objects;

	typedef std::vector<int> int_container_t;
	// ���������� ���������� ���������
	int_container_t win_array;

	// ������� �������� ������
	int CurrentActive;

	// ������ ���� � ��������� �� ���������
	int v_size;
	// ������ ���� � ��������� �� �����������
	int h_size;
	// ���������� ��������� �������
	int states_count;
	// ���������� ��������� �������
	int normal_state;

	// ������ � ������ ������
	int	img_width;
	int img_height;

	// ������� ����������� ����� ��������
	int shift_x;
	int shift_y;

	// ��������� � ������
	const qdEngineInterface* engine_;
	// ��������� � ������� �����
	qdMinigameSceneInterface* scene_;

	// �������� �������� ����
	bool load_level(const char* fname);

	// ����������� ������������
	bool move(int source, int dest);
protected:
	
};

qdMiniGameInterface* open_game_interface(const char*)
{
	return new bright_minigame;
}

bool close_game_interface(qdMiniGameInterface* p)
{
	p -> finit();
	delete p;
	return true;
}

void bright_minigame::init_flags(){
	img_width = 0;
	img_height = 0;

	CurrentActive = -1;
}

bright_minigame::bright_minigame() : back_object(NULL),
	field_objects(NULL)
{
	init_flags();
}

void bright_minigame::clear_all(){
	for (field_container_t::iterator pnode = field_objects.begin(); pnode != field_objects.end(); ++pnode){
		scene_ -> release_object_interface((*pnode) -> field);
		scene_ -> release_object_interface((*pnode) -> chip);
		scene_ -> release_object_interface((*pnode) -> bw);
		scene_ -> release_object_interface((*pnode) -> mod);
		delete *pnode;
	}
	field_objects.clear();

	win_array.clear();
}

bool bright_minigame::finit()
{
	if (NULL != scene_){		
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

bool bright_minigame::init(const qdEngineInterface* p)
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

		
		sscanf(scene_ -> minigame_parameter("field_size"),"%D%D",&h_size, &v_size);
		sscanf(scene_ -> minigame_parameter("states_count"),"%D",&states_count);

		normal_state = states_count / 2;

		const char* file_name = scene_ -> minigame_parameter("map_file_name");
		if(!load_level(file_name)){
			finit();
			return false;
		}
	} else
		return false;
	
	return true;
}

mgVect3f bright_minigame::game2world_coords(int ind, float deep){
	
	int x = ind % h_size; 
	int y = ind / h_size;

	int _x = x * img_width;
	int _y = y * img_height;

	return scene_ -> screen2world_coords(mgVect2i(_x + shift_x, _y + shift_y), deep);
}

bool bright_minigame::quant(float dt)
{
	mgVect2i mouse = engine_ -> mouse_cursor_position();
	
	if(!engine_->is_mouse_event_active(qdEngineInterface::MOUSE_EV_LEFT_DOWN))
		return true;
	
	mgVect2i r;
	mouse = scene_ -> world2screen_coords(scene_ -> screen2grid_coords(mouse));
	int state = -1;

	// ������� � ����� ������ ������
	int ind = 0;
	for (field_container_t::iterator pnode = field_objects.begin(); pnode != field_objects.end(); ++pnode, ind++){
		r = scene_ -> world2screen_coords((*pnode)->field->R());
		if(abs(mouse.x - r.x) <= img_width/2 && abs(mouse.y - r.y) <= img_height/2){
			state = (*pnode)->mod->current_state_index();
			break;
		}
   	}

	// ���� ��� ������ � �� ������
	if(state < 0) return true;

	// ��������� ���� �� ��� ������������ ������
	if(CurrentActive == -1){
		// ������ ��� -> ������������ ������� � ���������� �
		if(!state) return true; // ��� ��� ������������
		CurrentActive = ind;
		flash_on(field_objects[ind]);
		return true;
	}

	// � ����� ������ ��, ��� ���� ���������� ����� ��������
	flash_off(field_objects[CurrentActive]);
	
	// ������� ������� �����������
	bool is_moved = move(CurrentActive, ind);
	
	CurrentActive = -1;

	// ���� ������ �� �������� - �� ������� ������ �� ���������
	if(!is_moved) 
		return true;
	
	for (pnode = field_objects.begin(); pnode != field_objects.end(); ++pnode)
		if((*pnode)->state != normal_state)
			return true;
	
	// ���� ����� �� ����, �� ���� ��������
	// ��� ��������� ���� � ���� ���-�� ������
	// .....
		state_flag->set_state("win");
	//
	
	return true;

}

bool bright_minigame::move(int source, int dest){
	
	ObjNode *S = field_objects[source], *D = field_objects[dest];

	if(D->mod->current_state_index() > 0) return false; // ����� ������
	
	 // ��������� ����� ������ � �������� ������
	int delta = abs(source - dest);
	if(delta != 1 && delta != h_size) return false;

	int type = S->mod->current_state_index();
	switch(type){
	case 1:
		if(D->state >= states_count)
			return false; //�������� ������� ������
		if(D->state < 0){
			D->state = 0;
			D->bw->set_state(0);
		}else{
			D->state++;
			if(D->state == states_count)
				D->bw->set_state(2);
		}
		break;
	case 2:
		if(D->state < 0)
			return false; //�������� ������� ������
		if(D->state >= states_count){
			D->bw->set_state(0);
			D->state = states_count - 1;
		}else{
			D->state--;
			if(D->state < 0)
				D->bw->set_state(1);
		}
	}
	D->chip->set_state((D->state >= 0 && D->state < states_count)?D->state:0);
	
	S->mod->set_state("transparent");
	D->mod->set_state(type);
	
	return true;
}

bool bright_minigame::load_level(const char* fname)
{
	std::fstream file;
	file.open(fname, std::ios::in);
	if (file.fail()) return false;

	// ������� ������ ��� ������ ���������� � init'�� ������;
	clear_all(); init_flags();

	int blocks_num = h_size * v_size;

	mgVect2i shift = scene_ -> world2screen_coords(back_object->R());

	qdMinigameObjectInterface *field, *chip, *bw;

	char name[256];

	for (int i = 0; i < blocks_num; i++)
	{
		int state;
		file >> state;

		sprintf(name,"%s%.4d",scene_ -> minigame_parameter("square_object_name"), i);
		field = scene_ -> object_interface(name);
		if(!field) continue;
		sprintf(name,"chip_name_%.3d", i);
		chip = scene_ -> object_interface(scene_ -> minigame_parameter(name));
		if(!chip){
			scene_ -> release_object_interface(field);
			continue;
		}
		sprintf(name,"%s%.4d",scene_ -> minigame_parameter("chip_name_bw"), i);
		bw = scene_ -> object_interface(name);
		if(!bw){
			scene_ -> release_object_interface(field);
			scene_ -> release_object_interface(chip);
			continue;
		}


		if(!img_width){
			mgVect2i size = field->screen_size();
			img_width = size.x;
			img_height= size.y;
			shift_x = shift.x - img_width * h_size / 2;
			shift_y = shift.y - img_height * v_size / 2;
		}
			
		field -> set_state(0);
		field -> set_R(game2world_coords(i, 0));

		chip -> set_R(game2world_coords(i, 200));

		bw -> set_state("bw_trans");
		bw -> set_R(game2world_coords(i, 100));

		if(state < 0)
			bw -> set_state("bw_black");
		else if(state >= states_count)
			bw -> set_state("bw_white");
		else
			chip -> set_state(state);

		ObjNode *node = new ObjNode;
		node -> field = field;
		node -> chip = chip;
		node -> bw = bw;
		node -> state = state;

		field_objects.push_back(node);
	}

 	for (i = 0; i < blocks_num; i++)
	{
		int state;
		file >> state;
		sprintf(name,"%s%.4d",scene_ -> minigame_parameter("modify_object_name"), i);
		if(qdMinigameObjectInterface* mod = scene_ -> object_interface(name)){
			field_objects[i]->mod = mod;					
			mod -> set_R(game2world_coords(i, -100));
			if(state > 0)
				mod -> set_state("plus");
			else if(state < 0)
				mod -> set_state("minus");
			else
				mod -> set_state("transparent");
		}
	}

	file.close();
	return true;
}

void bright_minigame::flash_on(ObjNode* obj){
	set_z(obj->field, -100);
	obj->field->set_state(1);
}

void bright_minigame::flash_off(ObjNode* obj){
	set_z(obj->field, 0);
	obj->field->set_state(0);
}