#include <vector>
#include <list>
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
	qdMinigameObjectInterface*	bord;
	int x, y;
	float deep;
	ObjNode(): obj(NULL), bord(NULL), x(-1), y(-1), deep(0){
	}
	~ObjNode(){}
};

typedef std::list<ObjNode*> chip_container_t;
typedef std::vector<mgVect2i> vect2_container_t;

class ParentMinigame : public qdMiniGameInterface
{
public:
	ParentMinigame();
	~ParentMinigame(){ }

	// »нициализаци€ игры.
	bool init(const qdEngineInterface* p);

	// ќбсчЄт логики игры, параметр - врем€, которое должно пройти в игре (в секундах).
	bool quant(float dt);

	// ƒеинициализаци€ игры.
	bool finit();
private:
	// почистить все интерфейсы и массивы
	void clear_all(void);
	
	// пересчет из игровых индексных координат в мировые
	mgVect3f game2world_coords(int x, int y, float deep = 0){
		return scene_ -> screen2world_coords(mgVect2i(x * img_width + shift_x, y * img_height + shift_y), deep);}

	// подсветить/потушить €чейку
	void flash(int);
	
	// фон
	qdMinigameObjectInterface* back_object;
	/// флаг победы
	qdMinigameObjectInterface* state_flag;


	// фишки дл€ размещени€ на поле
	chip_container_t chip_array;

	// выигрышна€ комбинаци€
	vect2_container_t win_array;

	// текуща€ активна€ €чейка
	ObjNode* CurrentActive;
	
	mgVect2i last_mouse_position;

	// размер пол€ в полигонах по вертикали
	int v_size;
	// размер пол€ в полигонах по горизонтали
	int h_size;

	// заранее посчитанный сдвиг картинки
	int shift_x;
	int shift_y;

	// ширина и высота €чейки
	int	img_width;
	int img_height;

	// интерфейс к движку
	const qdEngineInterface* engine_;
	// интерфейс к текущей сцене
	qdMinigameSceneInterface* scene_;

	// загрузка игрового пол€
	bool load_level(const char* fname);

	// нарисовать фрагмент
	void draw(ObjNode* node);
	
	// сдвинуть вместе с потомками
	bool move(int sx, int sy, bool only_check = false);
protected:
	
};

qdMiniGameInterface* open_game_interface(const char*)
{
	return new ParentMinigame;
}

bool close_game_interface(qdMiniGameInterface* p)
{
	p -> finit();
	delete p;
	return true;
}

ParentMinigame::ParentMinigame() : back_object(NULL), chip_array(NULL),
				CurrentActive(NULL)
{
}

void ParentMinigame::clear_all(){
	for (chip_container_t::iterator akt_chip = chip_array.begin(); akt_chip != chip_array.end(); ++akt_chip){
		scene_ -> release_object_interface((*akt_chip)->obj);
		delete *akt_chip;
	}
	chip_array.clear();

	win_array.clear();

	h_size = 0;
	v_size = 0;
	
	img_width = 0;
	img_height = 0;

	CurrentActive = NULL;
}

bool ParentMinigame::finit()
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

bool ParentMinigame::init(const qdEngineInterface* p)
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

bool ParentMinigame::quant(float dt)
{
	
	mgVect2i mouse = engine_ -> mouse_cursor_position();
	
	bool mouse_down = engine_->is_mouse_event_active(qdEngineInterface::MOUSE_EV_LEFT_DOWN);
	bool mouse_up	= engine_->is_mouse_event_active(qdEngineInterface::MOUSE_EV_LEFT_UP);
	if(mouse_up)
	{	// отпустили кнопку, если был выделенный его нужно отпустить
		if(CurrentActive){
			flash(0);
			CurrentActive = NULL;

			// проверим на победу
			bool IsWin = true;
			int ind = 0;
			for(chip_container_t::iterator pnode = chip_array.begin(); pnode != chip_array.end(); pnode++, ind++)
				if((*pnode)->x != win_array[ind].x || (*pnode)->y != win_array[ind].y){
					IsWin = false;
					break;
				}

			if(IsWin){
				state_flag->set_state("win");
				return false;
			}

		}
		return true;
	}
	
	mouse = scene_ -> world2screen_coords(scene_ -> screen2grid_coords(mouse));
	
	mouse.x = (mouse.x - shift_x + img_width/2) / img_width;
	mouse.y = (mouse.y - shift_y + img_height/2) / img_height;

	if(mouse.x >= h_size || mouse.x < 0 || mouse.y >= v_size || mouse.y < 0)
		return true;

	ObjNode* obj = NULL;
	
	if(CurrentActive){
		mgVect2i sh = mouse - last_mouse_position;
		move(sh.x, sh.y);
	}else{
		if(mouse_down){
			// смотрим в какой пр€моугольник попали
			for(chip_container_t::iterator pnode = chip_array.begin(); pnode != chip_array.end(); pnode++){
				ObjNode* node = *pnode;
				if(node->x == mouse.x && node->y == mouse.y){
					obj = node;
					break;
				}
			}
		
			// никуда не попали
			if(!obj) return true;
		
			CurrentActive = obj;
			flash(1);
		}
	}

	last_mouse_position = mouse;			
	return true;
}


bool ParentMinigame::load_level(const char* fname)
{
	std::fstream file;
	file.open(fname, std::ios::in);
	if (file.fail()) return false;

	clear_all();

	file >> h_size >> v_size;
	int states;
	file >> states;
	
	mgVect2i shift = scene_ -> world2screen_coords(back_object->R());

	char name[256];
	for (int i = 0; i < states; i++)
	{
		sprintf(name,"%s%.4d",scene_ -> minigame_parameter("chip_object_name"),i);
		qdMinigameObjectInterface* obj = scene_ -> object_interface(name);
		if(!obj) continue;
		sprintf(name,"%s%.4d", scene_ -> minigame_parameter("bord_object_name"),i);
		qdMinigameObjectInterface* brd = scene_ -> object_interface(name);
		if(!brd){
			scene_ -> release_object_interface(obj);
			continue;
		}

		if(!img_width){
			mgVect2i size = obj->screen_size();
			img_width = size.x;
			img_height= size.y;
			shift_x = shift.x - img_width * (h_size - 1) / 2;
			shift_y = shift.y - img_height *(v_size - 1) / 2;
		}
			
		ObjNode* node = new ObjNode;

		obj -> set_state(i);
		node->obj = obj;

		brd -> set_state(0);
		node->bord = brd;

		file >> node->x >> node->y;

		draw(node);
		
		chip_array.push_back(node);
	}

	// читаем выигрышную комбинацию
	for(i = 0; i < states; i++){
		mgVect2i v2(0);
		file >> v2.x >> v2.y;

		win_array.push_back(v2);
	}

	file.close();
	return true;
}

void ParentMinigame::flash(int state){
	bool start = false;
	for(chip_container_t::iterator pnode = chip_array.begin(); pnode != chip_array.end(); pnode++){
		ObjNode* node = *pnode;
		if(node == CurrentActive)
			start = true;
		if(start){
			node->bord->set_state(state);
			node->deep = -100 * state;
			draw(node);
		}		
	}
}

void ParentMinigame::draw(ObjNode* node){
	node->obj->set_R(game2world_coords(node->x, node->y, node->deep));
	node->bord->set_R(game2world_coords(node->x, node->y, node->deep - 50));	
}

bool ParentMinigame::move(int sx, int sy, bool only_check){
	if(!only_check)
		if(!move(sx, sy, true)) return false;

	bool start = false;
	for(chip_container_t::iterator pnode = chip_array.begin(); pnode != chip_array.end(); pnode++){
		ObjNode* node = *pnode;
		if(node == CurrentActive)
			start = true;
		if(start){
			if(node->x + sx >= h_size || node->x + sx < 0 ||
				node->y + sy >= v_size || node->y + sy < 0)
				return false;
			for(chip_container_t::iterator pnode2 = chip_array.begin(); *pnode2 != CurrentActive; pnode2++)
				if(node->x + sx == (*pnode2)->x && node->y + sy == (*pnode2)->y)
					return false;
			if(!only_check){
				node->x += sx;
				node->y += sy;
				draw(node);
			}
		}		
	}
	return true;
}