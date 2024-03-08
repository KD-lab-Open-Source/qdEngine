#include <vector>
#include <algorithm>
#include <fstream>

//#include <math.h>
//#include <afx.h>
#include <stdio.h>

#include "qd_minigame_interface.h"

extern "C" {
__declspec(dllexport) qdMiniGameInterface* open_game_interface(const char*);
__declspec(dllexport) bool close_game_interface(qdMiniGameInterface* p);
};

typedef std::vector<qdMinigameObjectInterface*> obj_container_t;

#define LAYER_DEEP_START 1000
#define LAYER_DELTA 100

class Square;

class OpenSquare_minigame : public qdMiniGameInterface
{
	friend class Square;
public:
	OpenSquare_minigame();
	~OpenSquare_minigame();

	// »нициализаци€ игры.
	bool init(const qdEngineInterface* p);

	// ќбсчЄт логики игры, параметр - врем€, которое должно пройти в игре (в секундах).
	bool quant(float dt);

	// ƒеинициализаци€ игры.
	bool finit();
private:
	// загрузка начальных значений во флаги
	void init_flags(void);

	// почистить все интерфейсы и массивы
	void clear_all(void);
	
	// пересчет из индексов по X и Y в индекс вектора
	int _2D21D(int x, int y) {return y * h_size + x;}
	
	// пересчет из игровых индексных координат в мировые
	mgVect3f game2world_coords(int ind, float deep = 0);
	
	// задать глубину не мен€€ экранных координат
	void set_z(qdMinigameObjectInterface* obj, float z){
		obj -> set_R(scene_->screen2world_coords(scene_->world2screen_coords(obj->R()), z)); }

	// фон
	qdMinigameObjectInterface* back_object;
	/// флаг победы
	qdMinigameObjectInterface* state_flag;


	typedef std::vector<Square*> field_container_t;
	// игровое поле
	field_container_t field_objects;

	typedef std::vector<Square*>::reverse_iterator ri_field_container;
	
	Square* game_pad;
	Square* win_zone;

	// текуща€ активна€ €чейка
	Square* CurrentActive;

	// размер пол€ в полигонах по вертикали
	int v_size;
	// размер пол€ в полигонах по горизонтали
	int h_size;

	// ширина и высота €чейки
	int	img_width;
	int img_height;

	// заранее посчитанный сдвиг картинки
	int shift_x;
	int shift_y;

	mgVect2i last_mouse_position;

	// интерфейс к движку
	const qdEngineInterface* engine_;
	// интерфейс к текущей сцене
	qdMinigameSceneInterface* scene_;

	// загрузка игрового пол€
	bool load_level(const char* fname);

	//
	void sort_by_depth();
	
protected:
	
};

class Square{
	// глубина сло€
	int deep;
	
	obj_container_t border;
	obj_container_t body;

	void destroy(void);

	void draw();

	// размеры пр€моугольника в игровых квадратах
	int width;
	int height;
	// левый верхний угол
	int the_x;
	int the_y;

	bool IsActive;
	bool IsBorder;
	int FillState;

public:
	Square();
	~Square();

	// указатель на класс игры
	static class OpenSquare_minigame *game;
	
	// создать пр€моугольник
	bool construct(int x1, int y1, int x2, int y2, int dp, bool bord, bool activ, int fstate, int &brd_obj_num, int &bod_obj_num);

	int GetDeep(void){ return deep;};
	void SetDeep(int);
	
	// сдвинуть
	void Shift(int dx, int dy);

	// проверить пересечение пр€моугольников
	bool IsCross(Square*);

	// проверить вхождение единичного квадрата
	bool IsInside(int x, int y);

	// включить/выключить подсветку бордюра
	void SetActive(bool activ);
};
OpenSquare_minigame* Square::game = NULL;

qdMiniGameInterface* open_game_interface(const char*)
{
	return new OpenSquare_minigame;
}

bool close_game_interface(qdMiniGameInterface* p)
{
	p -> finit();
	delete p;
	return true;
}

void OpenSquare_minigame::init_flags(){

	//CurrentActive = -1;
}

OpenSquare_minigame::~OpenSquare_minigame(){
	Square::game = NULL;
}

OpenSquare_minigame::OpenSquare_minigame() : back_object(NULL),
	field_objects(NULL), CurrentActive(NULL)
{
	init_flags();
	Square::game = this;
}

void OpenSquare_minigame::clear_all(){
	for (field_container_t::iterator pnode = field_objects.begin(); pnode != field_objects.end(); ++pnode)
		delete *pnode;
	field_objects.clear();
	CurrentActive = NULL;
}

bool OpenSquare_minigame::finit()
{
	if (NULL != scene_){		
		clear_all();
		delete win_zone;
		delete game_pad;

		if(back_object)
			scene_ -> release_object_interface(back_object);
		
		if(state_flag)
			scene_ -> release_object_interface(state_flag);

		engine_ -> release_scene_interface(scene_);
		scene_ = NULL;
	}
	return true;
}

bool OpenSquare_minigame::init(const qdEngineInterface* p)
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

		
		sscanf(scene_ -> minigame_parameter("image_size"),"%D%D",&img_width, &img_height);
		sscanf(scene_ -> minigame_parameter("field_size"),"%D%D",&h_size, &v_size);

		game_pad = new Square;
		win_zone = new Square;

		const char* file_name = scene_ -> minigame_parameter("map_file_name");
		if(!load_level(file_name)){
			finit();
			return false;
		}
	} else
		return false;
	
	return true;
}

mgVect3f OpenSquare_minigame::game2world_coords(int ind, float deep){
	
	int x = ind % h_size; 
	int y = ind / h_size;

	int _x = x * img_width;
	int _y = y * img_height;

	return scene_ -> screen2world_coords(mgVect2i(_x + shift_x, _y + shift_y), deep);
}

bool OpenSquare_minigame::quant(float dt)
{
	mgVect2i mouse = engine_ -> mouse_cursor_position();
	
	bool mouse_down = engine_->is_mouse_event_active(qdEngineInterface::MOUSE_EV_LEFT_DOWN);
	bool mouse_up	= engine_->is_mouse_event_active(qdEngineInterface::MOUSE_EV_LEFT_UP);
	if(mouse_up)
	{	// отпустили кнопку, если был выделенный его нужно отпустить
		if(CurrentActive){
			CurrentActive->SetActive(false);
			CurrentActive = NULL;

			// проверим на победу
			bool IsWin = true;
			for(field_container_t::iterator pnode = field_objects.begin(); pnode != field_objects.end(); pnode++)
				if(win_zone->IsCross(*pnode)){
					IsWin = false;
					break;
				}

			if(IsWin){
				state_flag->set_state("win");
				return true;
			}

		}
		return true;
	}
	
	mouse = scene_ -> world2screen_coords(scene_ -> screen2grid_coords(mouse));
	
	mouse.x = (mouse.x - shift_x + img_width/2) / img_width;
	mouse.y = (mouse.y - shift_y + img_height/2) / img_height;

	if(mouse.x >= h_size || mouse.x < 0 || mouse.y >= v_size || mouse.y < 0)
		return true;

	Square* obj = NULL;
	
	if(CurrentActive){
		mgVect2i sh = mouse - last_mouse_position;
		CurrentActive->Shift(sh.x, sh.y);
	}else{
		if(mouse_down){
			// надо определить и выделить объект
			int ind = field_objects.size()-1;
			// смотрим в какой пр€моугольник попали
			for (;ind >= 0; ind--)
				if(field_objects[ind]->IsInside(mouse.x, mouse.y)){
					obj = field_objects[ind];
					break;
				}
		
			// если так никуда и не попали
			if(!obj) return true;
		
			// провер€ем не лежит ли сверху еще кто-то
			bool is_free = true;
			while(ind++ < field_objects.size()-1){
				if(obj->IsCross(field_objects[ind])){
					is_free = false;
					break;
				}
			}

			// пр€моугольник завален сверху
			if(!is_free) return true;

			CurrentActive = obj;
			CurrentActive->SetActive(true);
			CurrentActive->SetDeep(field_objects.back()->GetDeep()-LAYER_DELTA);
			sort_by_depth();
		}
	}

	last_mouse_position = mouse;			

	return true;
	
}

bool OpenSquare_minigame::load_level(const char* fname)
{
	std::fstream file;
	file.open(fname, std::ios::in);
	if (file.fail()) return false;

	// —начала чистим все вз€тые интерфейсы и init'им данные;
	clear_all(); init_flags();

	//file >> h_size >> v_size;

	mgVect2i shift = scene_ -> world2screen_coords(back_object->R());

	shift_x = shift.x - img_width * h_size / 2;
	shift_y = shift.y - img_height * v_size / 2;

	int border_num = 0;
	int body_num = 0;
	int current_deep = LAYER_DEEP_START;
	
	game_pad->construct(0, 0, h_size-1, v_size-1, current_deep+LAYER_DELTA, true, false, 0, border_num, body_num);

	int x1, y1, x2, y2;
	sscanf(scene_ -> minigame_parameter("win_zone"),"%D%D%D%D", &x1, &y1, &x2, &y2);
	qdMinigameObjectInterface* obj = scene_ -> object_interface(scene_ -> minigame_parameter("win_image"));
	if(obj){
		int xx = shift_x + img_width  * (0.5 * abs(x2 - x1) + x1);
		int yy = shift_y + img_height * (0.5 * abs(y2 - y1) + y1);
		obj->set_R(scene_ -> screen2world_coords(mgVect2i(xx, yy), current_deep));
	}
	win_zone->construct(x1, y1, x2, y2, current_deep-=LAYER_DELTA, true, false, 0, border_num, body_num);

	//game_pad->SetActive(true);

	int square_count;
	file >> square_count;

	for(;square_count > 0; square_count--){
		file >> x1 >> y1 >> x2 >> y2;
		Square *obj = new Square;
		obj->construct(x1, y1, x2, y2, current_deep-=LAYER_DELTA, true, false, 1, border_num, body_num);
		field_objects.push_back(obj);

		//obj->SetActive(true);
	}

	file.close();

	//sort_by_depth();

	return true;
}

bool compare_deep(Square* x, Square* y){
    return x->GetDeep() > y->GetDeep();
}

void OpenSquare_minigame::sort_by_depth(){
	std::sort(field_objects.begin(), field_objects.end(), compare_deep);
	int deep = win_zone->GetDeep()-LAYER_DELTA;
	for(field_container_t::iterator pnode = field_objects.begin(); pnode != field_objects.end(); ++pnode, deep-=LAYER_DELTA){
		if((*pnode)->GetDeep() != deep)
			(*pnode)->SetDeep(deep);
	}
}

Square::Square(): border(NULL), body(NULL),
	width(0), height(0), deep(0),
	the_x(-1), the_y(-1){
}

Square::~Square(){
	destroy();
}

void Square::destroy(void){
	
	/*
	std::fstream trace;
	trace.open("trace", std::ios::out|std::ios::app);
	trace << "{ DESTROY \n";
	trace << "the_x = " << the_x << ", the_y = " << the_y << ", width = " << width << ", height = " << height << ", &border=" << &border << ", game=" << game << "\n";
	trace << "border.size()="<< border.size() <<"\nborder nodes:\n";
	*/
	
	//AfxTrace("%d\n", mmm);
	//if(width>0){
	for (obj_container_t::iterator pnode = border.begin(); pnode != border.end(); ++pnode){
		//trace << *pnode << "\n";		
		game->scene_ -> release_object_interface(*pnode);
	}
	border.clear();
	//trace << "--- bodys nodes:\n";

	for (pnode = body.begin(); pnode != body.end(); ++pnode){
		//trace << *pnode << "\n";
		game->scene_ -> release_object_interface(*pnode);
	}
	body.clear();
	//}
	//trace << "} /destroy\n\n\n";
	//trace.close();
	width = height = deep = 0;
	the_x = the_y = -1;
}


void Square::draw(){
	qdMinigameObjectInterface* obj;
	for(int y = 0; y < height; y++)
		for(int x = 0; x < width; x++){
			int game_coord = game->_2D21D(the_x + x, the_y + y);
			mgVect3f border_coord = game->game2world_coords(game_coord, deep-10);
			int active_shift = (IsActive)?4:0;
			if(y == 0){ // верхний бордюр
				obj = border[x];
				obj->set_state((IsBorder)?1+active_shift:0);
				obj->set_R(border_coord);
			}
			if(x == 0){ // левый бордюр
				obj = border[width + 2*y];
				obj->set_state((IsBorder)?2+active_shift:0);
				obj->set_R(border_coord);
			}
			if(x == width - 1){ // правый бордюр
				obj = border[width + 2*y + 1];
				obj->set_state((IsBorder)?3+active_shift:0);
				obj->set_R(border_coord);
			}
			if(y == height - 1){ // нижний бордюр
				obj = border[width + 2*height + x];
				obj->set_state((IsBorder)?4+active_shift:0);
				obj->set_R(border_coord);
			}
			obj = body[x + y*width];
			obj->set_state(FillState);
			obj->set_R(game->game2world_coords(game_coord, deep));
		}

}

bool Square::construct(int x1, int y1, int x2, int y2, int dp, bool bord, bool activ, int fstate, int &brd_obj_num, int &bod_obj_num){
	char name[255];
	qdMinigameObjectInterface* obj;

	/*
	std::fstream trace;
	trace.open("trace", std::ios::out|std::ios::app);
	trace << "{ CONSTRUCT ---------------\n";
	trace << "(1)->the_x = " << the_x << ", the_y = " << the_y << ", width = " << width << ", height = " << height << ", &border=" << &border << ", game=" << game << "\nborder nodes:\n";
	trace.close();
	 */
	destroy();

	deep = dp;
	IsBorder = bord;
	IsActive = activ;
	FillState = fstate;

	// как координату храним левый верхний угол
	the_x = (x1 < x2)? x1 : x2;
	width = abs(x1 - x2) + 1;

	the_y = (y1 < y2)? y1 : y2;
	height = abs(y1 - y2) + 1;

	/*
	trace.open("trace", std::ios::out|std::ios::app);
	trace << "(2)->the_x = " << the_x << ", the_y = " << the_y << ", width = " << width << ", height = " << height << ", &border=" << &border << ", game=" << game << "\n";
	trace.close();
	*/

	if(the_x + width > game->h_size || the_y + height > game->v_size){
		destroy();
		return false; // не влазит в игровое поле
	}
	
	/*
	trace.open("trace", std::ios::out|std::ios::app);
	trace << "border nodes:\n";
	*/

	for(int i = 0; i < 2*(width+height); i++, brd_obj_num++){
		sprintf(name,"%s%.4d",game->scene_ -> minigame_parameter("border_object_name"), brd_obj_num);
		obj = game->scene_ -> object_interface(name);
		//trace << obj << "\n";
		if(!obj){
			destroy();
			return false;
		}
		//obj->set_state(0);
		border.push_back(obj);
	}
	//trace << "--- bodys nodes:\n";			
	for(i = 0; i < width * height; i++, bod_obj_num++){
		sprintf(name,"%s%.4d",game->scene_ -> minigame_parameter("body_object_name"), bod_obj_num);
		obj = game->scene_ -> object_interface(name);
		//trace << obj << "\n";
		if(!obj){
			destroy();
			return false;
		}
		//obj->set_state(FillState);
		//obj->set_R(game->game2world_coords(game->_2D21D(the_x+i%width, the_y+i/width), deep));
		body.push_back(obj);
	}

	//trace << "} /construct \n\n\n";
	//trace.close();
	draw();
	
	return true;
}

void Square::SetDeep(int dp){
	deep = dp;
	for (obj_container_t::iterator pnode = border.begin(); pnode != border.end(); ++pnode)
		game-> set_z(*pnode, deep-10);
	
	for (pnode = body.begin(); pnode != body.end(); ++pnode)
		game-> set_z(*pnode, deep);
}

void Square::SetActive(bool activ){
	if(!IsBorder || activ == IsActive)
		return; // бордюра нет или состо€ние уже равно запрошенному
	IsActive = activ;

	int shift = (IsActive)?4:-4;
	for(obj_container_t::iterator pnode = border.begin(); pnode != border.end(); ++pnode)
		(*pnode)->set_state((*pnode)->current_state_index() + shift);

}

bool Square::IsCross(Square *sq){
	Square *right, *left, *bottom, *top;
	
	if(sq->the_x >= the_x){
		right = sq;
		left = this;
	}else{
		right = this;
		left = sq;
	}
	if(right->the_x >= left->the_x && right->the_x <= left->the_x + left->width-1){
		if(sq->the_y >= the_y){
			bottom = sq;
			top = this;
		}else{
			bottom = this;
			top = sq;
		}
		if(bottom->the_y >= top->the_y && bottom->the_y <= top->the_y+top->height-1)
			return true;
		else
			return false;
	
	}
	
	return false;
}

bool Square::IsInside(int x, int y){
	if(x >= the_x && x < the_x + width &&
		y >= the_y && y < the_y + height)
		return true;
	
	return false;
}

void Square::Shift(int dx, int dy){
	if(!dx && !dy)
		return;

	if(the_x+dx < 0 || the_x+dx+width > game->h_size ||
		the_y+dy < 0 || the_y+dy+height > game->v_size){
		return;
	}

	the_x += dx;
	the_y += dy;

	draw();
}