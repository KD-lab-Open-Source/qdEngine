#include "pre.h"

extern "C" {
__declspec(dllexport) qdMiniGameInterface* open_game_interface(const char*);
__declspec(dllexport) bool close_game_interface(qdMiniGameInterface* p);
};

qdMiniGameInterface* open_game_interface(const char*)
{
	return new AntiT_minigame;
}

bool close_game_interface(qdMiniGameInterface* p)
{
	p -> finit();
	delete p;
	return true;
}

AntiT_minigame::~AntiT_minigame(){
	Figure::game = NULL;
}

AntiT_minigame::AntiT_minigame(): glass(NULL), figure_templates(NULL), templates_count(0),
	back_object(NULL), chip_cont(NULL), vline(NULL), shift_down_cont(NULL), figures(NULL),
	bounce(KEY_FIRST_BOUNCE), pressed_key(-1), last_process_key(-1),
	last_x_vline(0), last_y_vline(0), prob_shift(0), crushing_lines_count(0), need_vline(false),
	current(NULL), need_current(true), flag_calc_state(false), flag_lose(false), is_change_process(false){
	Figure::game = this;
}

void AntiT_minigame::clear_all(){
	for (chip_cont_t::iterator pch = chip_cont.begin(); pch != chip_cont.end(); ++pch){
		(*pch)->obj->set_R(game2world_coords(-100, -100));
		(*pch)->in_use = false;
	}

	if(vline)
		delete vline;
	vline = NULL;

	for (figure_cont_t::iterator pfc = figures.begin();	pfc != figures.end(); ++pfc)
		delete *pfc;
	figures.clear();
	
	for (sh_dwn_cont_t::iterator pdo = shift_down_cont.begin();	pdo != shift_down_cont.end(); ++pdo)
		delete *pdo;
	shift_down_cont.clear();
	
	if(figure_templates){
		for(int figure = 0; figure < templates_count; figure++){
			for(int line = 0; line < figure_max_height; line++)
				delete figure_templates[figure].fig[line];
			delete figure_templates[figure].fig;
		}
		delete figure_templates;
		figure_templates = NULL;
	}
	
	if(glass){
		for(int v = 0; v < v_size; v++)
			delete glass[v];
		delete glass;
		glass = NULL;
	}
	
}

bool AntiT_minigame::finit()
{
	if (NULL != scene_){		
		clear_all();
		for (chip_cont_t::iterator pch = chip_cont.begin(); pch != chip_cont.end(); ++pch){
			scene_ -> release_object_interface((*pch)->obj);
			delete *pch;
		}
		chip_cont.clear();
		
		if(back_object)
			scene_ -> release_object_interface(back_object);
		if(state_flag)
			scene_ -> release_object_interface(state_flag);


		engine_ -> release_scene_interface(scene_);
		scene_ = NULL;

		delete current;
	}
	return true;
}

bool AntiT_minigame::init(const qdEngineInterface* p){
	qdMinigameObjectInterface* obj;
	engine_ = p;
	if(scene_ = p -> current_scene_interface()){
		back_object = scene_ -> object_interface(scene_ -> minigame_parameter("back_object_name"));
		//back_object->set_R(scene_ -> screen2world_coords(mgVect2i(0, 0), 200));
		state_flag = scene_ -> object_interface("state_flag");
		if(!state_flag){
			finit();
			return false;
		}else{
			state_flag->set_state("game");
		}


		clear_all();

		char name[255];
		for(int i = 0;;i++){
			sprintf(name,"%s%.4d",scene_ -> minigame_parameter("chip_object_name"),i);
			if(!(obj = scene_ -> object_interface(name)))
				break;
			chip* node = new chip;
			node->obj = obj;
			chip_cont.push_back(node);
		}

		sscanf(scene_ -> minigame_parameter("field_size"),"%D%D",&h_size, &v_size);
		sscanf(scene_ -> minigame_parameter("image_size"),"%D%D",&img_width, &img_height);
		sscanf(scene_ -> minigame_parameter("max_hieght_segment"),"%D",&max_vseg);
		sscanf(scene_ -> minigame_parameter("crush_width"),"%D",&crush_width);
		sscanf(scene_ -> minigame_parameter("crush_height"),"%D",&crush_height);

		load_template(scene_ -> minigame_parameter("figure_templates"));

		sprintf(name,"%s",scene_ -> minigame_parameter("glass_object_name"));
		obj = scene_ -> object_interface(name);
		mgVect2i shift = scene_ -> world2screen_coords(obj->R());

		shift_x = shift.x - img_width * (h_size-1) / 2;
		shift_y = shift.y - img_height * (v_size-1) / 2;

		glass = new int*[v_size];
		for(i = 0; i < v_size; i++)
			glass[i] = new int[h_size+1];
		
		vline = new Figure;
		last_x_vline = h_size / 2;
		last_y_vline = v_size-1;
		vline->construct_empty();
		vline->set_color(3);
		vline->set_deep(-400);
		create_vline(v_size);

		current = new sh_dwn_obj;
		create_current();
		
	} else
		return false;
	
	return true;
}

qdMinigameObjectInterface* AntiT_minigame::GetChip(){
	if(!chip_cont.size()) return NULL;
	for (chip_cont_t::iterator pch = chip_cont.begin(); pch != chip_cont.end(); ++pch)
		if(!(*pch)->in_use){
			(*pch)->in_use = true;
			return (*pch)->obj;
		}

	return NULL;
}

void AntiT_minigame::RetChip(qdMinigameObjectInterface* obj){
	for (chip_cont_t::iterator pch = chip_cont.begin(); pch != chip_cont.end(); ++pch)
		if((*pch)->obj == obj){
			(*pch)->in_use = false;
			break;
		}
}

bool AntiT_minigame::quant(float dt)
{
	if(flag_lose) return true;
		
	if(flag_calc_state)
		calc_state();

	move_down_quere(dt);

	process_keyboard(dt);
	
	return true;
}

bool AntiT_minigame::load_template(const char* fname){
	std::fstream file;
	file.open(fname, std::ios::in);
	if (file.fail()) return false;

	file >> templates_count;

	figure_templates = new figure_shablon[templates_count];

	for(int figure = 0; figure < templates_count; figure++){
		char** fig = (figure_templates[figure].fig = new char*[figure_max_height]);
		for(int line = 0; line < figure_max_height; line++){
			fig[line] = new char[figure_max_width];
			for(int col = 0; col < figure_max_width; col++){
				int p;
				file >> p;
				fig[line][col] = (char)p;
			}

		}
		file >> figure_templates[figure].x >> figure_templates[figure].y;
	}

	return true;
	
}

bool AntiT_minigame::move_down_quere(float dt){
	for (sh_dwn_cont_t::iterator pdo = shift_down_cont.begin();
			pdo != shift_down_cont.end();
			++pdo){
		sh_dwn_obj* dwn = *pdo;
		dwn->pause -= dt;
		if(dwn->pause <= 0){
			dwn->obj->move_down();
			if(--(dwn->shift) <= 0){
				delete dwn;
				*pdo = NULL;
			}
			dwn->pause = dwn->speed;
		}
	}
	shift_down_cont.remove(NULL);
	
	if(is_change_process){
		if(shift_down_cont.size() == 0){
			if(need_vline){
				create_vline(1);
				need_vline = false;
			}
			flag_calc_state = true;
		}
		return true;
	}

	current->pause -= dt;
	if(current->pause <= 0){ // пора двигать вниз на пиксел
		current->pause = current->speed; // время до следующего сдвига
		if(current->obj->move_down())
			current->shift--;
		else
			current->shift = 0;
		if(!current->shift){ // закончен сдвиг на квадрат
			current->shift = img_height;
			if(!current->obj->is_able(0, 1)){ // вниз двигать больше нельзя
				if(need_current){
					figures.push_back(current->obj);
					create_current();			
					flag_calc_state = true;
				} else
					need_current = true;
			}
		}

	}

	return true;
}

void AntiT_minigame::process_keyboard(float dt){
	
	int old_key = pressed_key;
	int key_for_process = -1;
	
	if(engine_->is_key_pressed(VK_UP)){
		pressed_key = VK_UP;
	}else if(engine_->is_key_pressed(VK_DOWN)){
		pressed_key = VK_DOWN;
	}else if(engine_->is_key_pressed(VK_LEFT)){
		pressed_key = VK_LEFT;
	}else if(engine_->is_key_pressed(VK_RIGHT)){
		pressed_key = VK_RIGHT;
	}else if(engine_->is_key_pressed(VK_HOME)){
		pressed_key = VK_HOME;
	}else if(engine_->is_key_pressed(VK_END)){
		pressed_key = VK_END;
	}else{
		pressed_key = -1;
	}
		
	if(pressed_key != old_key)
	{ // изменение в нажатых кнопках
		if(pressed_key != -1 && last_process_key == -1) // нажали кнопку
			key_for_process = pressed_key;
		bounce = KEY_FIRST_BOUNCE;
	} else if(pressed_key == -1){
		last_process_key = -1;
	} else {
		bounce -= dt;
		if(bounce <= 0){
			key_for_process = pressed_key;
			last_process_key = pressed_key;
			bounce = KEY_NEXT_BOUNCE;
		}
	}
	
	if(key_for_process < 0)
		return;
	
	switch(key_for_process){
	case VK_UP:
		if(!current->obj->rotate(true))
			bounce = 0;
		break;
	case VK_DOWN:
		current->speed = 0;
		break;
	case VK_LEFT:
		if(!current->obj->move(-1, 0))
			bounce = 0;
		break;
	case VK_RIGHT:
		if(!current->obj->move(1, 0))
			bounce = 0;
		break;
	case VK_HOME:
		break;
	case VK_END:
		current->obj->move(0, 1, false);
		break;
	}

}

void AntiT_minigame::create_current(void){
	current->obj = new Figure;
	current->obj->construct_by_template(engine_ -> rnd(templates_count), 8, h_size/2-1, 0);
	if(!current->obj->is_able() || !current->obj->is_able(0, 1)){
		flag_lose = true;
		state_flag->set_state("lose");
	}
	current->shift = img_height;
	current->speed = FIRST_LEVEL_SPEED;
	current->pause = 2*current->speed;
	flag_calc_state = true;
	need_current = false;
	current->obj->draw();
}

void AntiT_minigame::calc_state(void){

	if(crushing_lines_count){
		crushing_lines_count--;
		kill_chips(v_size-1, 0, v_size-1, true);
		vline->move(0, 1);
		flag_calc_state = false;
		need_vline = true;
		return;
	}

	is_change_process = false;

	for(int y = 0; y < v_size; y++)
		for(int x = 0; x <= h_size; x++)
			glass[y][x] = 0;

  	for (figure_cont_t::iterator pfc = figures.begin();	pfc != figures.end(); ++pfc)
		(*pfc)->add2glass(1);
	
	vline->add2glass(2);

	int filled = 0;
	for(y = v_size -1; y >= v_size - crush_height; y--)
		for(int x = 0; x < h_size; x++)
			if(glass[y][x] > 2){
				filled++;
				break;
			}
	
	if(filled == crush_height){
		is_change_process = true;
		crushing_lines_count = crush_height;
	}else{
		for(y = v_size-1; y >= 0 && !is_change_process; y--){
			int zero = -1;
			for(int x = 0; x <= h_size; x++)
				if(glass[y][x] % 2 == 0){
					if(x - zero - 1 >= crush_width){
						kill_chips(y, zero+1, x-1);
						is_change_process = true;
						break;
					} else
						zero = x;
				}
		}
		flag_calc_state = false;
	}
	
}

void AntiT_minigame::kill_chips(int y, int x1, int x2, bool with_vline){
	for(figure_cont_t::iterator pfc = figures.begin();	pfc != figures.end(); ++pfc)
		for(int x = x1; x <= x2; x++){
			if(with_vline){
				vline->kill_chip(x, y);
			}
			if((*pfc)->kill_chip(x, y) == 0){ // это был последний куcок
				delete *pfc;
				*pfc = NULL;
				break;
			}
		}
	figures.remove(NULL);
	// сдвигаем вниз что можно
	figure_cont_t f_copy = figures;
	bool is_down;
	do{
		is_down = false;
		for(figure_cont_t::iterator pfc = f_copy.begin(); pfc != f_copy.end(); ++pfc)
			if((*pfc)->is_able(0, 1)){
				(*pfc)->move(0, 1);
				is_down = true;
				(*pfc) = NULL;
			}
		f_copy.remove(NULL); 
	}while(is_down);
}


void AntiT_minigame::create_vline(int size){
	int center = h_size / 2;
	float delta_prob_shift = 1. / max_vseg;
	int max_delta = 2;	
	for(int y = 0; y < size; y++, last_y_vline--){
		float rnd = engine_->fabs_rnd((float)0.99);
		if(rnd < prob_shift){
			prob_shift = delta_prob_shift;
			int current_delta = last_x_vline - (center - max_delta);
			if(rnd > current_delta * 1. / (2*max_delta))
				last_x_vline++;
			else
				last_x_vline--;
		}else{
			prob_shift += delta_prob_shift;
		}
		vline->add_chip(last_x_vline, last_y_vline);
	}
	vline->draw();
}
