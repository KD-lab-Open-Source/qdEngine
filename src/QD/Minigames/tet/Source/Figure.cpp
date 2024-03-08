#include "pre.h"

AntiT_minigame* Figure::game = NULL;

Figure::Figure(): left_x(0), top_y(0), the_x(0), the_y(0), virt_y(0), color(0), deep(-500),
				simmetric(false), base_state(0), rotate_ability(false), custom(false),
				shift_down(0), body(NULL){
	
}

Figure::Figure(Figure* src, int clr){
	left_x = src->left_x;
	top_y = src->top_y;
	the_x = src->the_x;
	the_y = src->the_y;
	virt_y = src->virt_y;
	width = src->width;
	height = src->height;
	color = clr;
	simmetric = src->simmetric;
	base_state = src->base_state;
	center_x = src->center_x;
	center_y = src->center_y;
	rotate_ability = src->rotate_ability;
	shift_down = src->shift_down;
	custom = src->custom;
	for (box_container_t::iterator pbox = src->body.begin(); pbox != src->body.end(); pbox++){
		box* sq = new box((*pbox)->x, (*pbox)->y);
		sq->obj = game->GetChip();
		sq->obj->set_state(color);
		body.push_back(sq);
	}
}

void Figure::clear(void){
	for (box_container_t::iterator pbox = body.begin(); pbox != body.end(); pbox++){
		game->RetChip((*pbox)->obj);
		delete *pbox;
	}
	body.clear();
	the_x = the_y = left_x = top_y = virt_y = 0;
}

Figure::~Figure(){
	clear();
}

bool Figure::rotate(bool dir, bool without_check){
	int save;

	if(!rotate_ability)
		return true;

	if(!without_check){
		Figure test(this, 0);
		test.rotate(dir, true);
		if(!test.is_able()) return false;
	}

	bool effect_dir = dir;
	if(simmetric){
		if(base_state != 0){
			effect_dir = (base_state == -1);
			base_state = 0;
		}else
			base_state = (dir)?1:-1;
	}

	int min_x = figure_max_width;
	int min_y = figure_max_height;

	if(effect_dir){ // против часовой
		for (box_container_t::iterator pbox = body.begin(); pbox != body.end(); pbox++){
			box* bx = *pbox;
			save = bx->y;
			bx->y = bx->x - center_x + center_y;
			bx->x = -save + center_x + center_y;
			if(bx->x < min_x) min_x = bx->x;
			if(bx->y < min_y) min_y = bx->y;
		}
		//save = top_y;
		//left_x += center_x + center_y;
		//top_y += center_y - center_x;
	}else{ // по часовой
		for (box_container_t::iterator pbox = body.begin(); pbox != body.end(); pbox++){
			box* bx = *pbox;
			save = bx->y;
			bx->y = -bx->x + center_x + center_y;
			bx->x = save - center_y + center_x;
			if(bx->x < min_x) min_x = bx->x;
			if(bx->y < min_y) min_y = bx->y;
		}
		//save = top_y;
		//left_x += center_x - center_y;
		//top_y += center_y + center_x;
	}
	left_x = the_x + min_x;
	top_y = the_y + min_y;

	save = width;
	width = height;
	height = save;

	draw();
	return true;
}


void Figure::create(char** sha, int cx, int cy){
	bool flag_out;
	// считаем смещение самой фигуры в шаблоне
	
	int sh_y = 0;
	flag_out = false;
	for(int y = 0; y < figure_max_height && !flag_out; y++)
		for(int x = 0; x < figure_max_width; x++)
			if(sha[y][x]){
				sh_y = y;
				flag_out = true;
				break;
			}

	int sh_x = 0;
	flag_out = false;
	for(int x = 0; x < figure_max_width && !flag_out; x++)
		for(y = 0; y < figure_max_height; y++)
			if(sha[y][x]){
				sh_x = x;
				flag_out = true;
				break;
			}
	
	clear();
	// создаем необходимые фрагменты
	for(y = 0; y < figure_max_height; y++)
		for(x = 0; x < figure_max_width; x++){
			if(!sha[y][x])
				continue;
			box* sq = new box(x-sh_x, y-sh_y);
			sq->obj = game->GetChip();
			sq->obj->set_state((sha[y][x] > 0)?sha[y][x]:color);
			body.push_back(sq);
		}
	bild_bound();
	
	if(width < 0){
		clear();
		return;
	}
	
	// проверяем симметричность, что бы правильно крутить
	
	bool** matr = new bool*[height];
	for(int i = 0; i < height; i++){
		matr[i] = new bool[width];
		for(int j = 0; j < width; j++)
			matr[i][j] = false;
	}

	for (box_container_t::iterator pbox = body.begin(); pbox != body.end(); pbox++){
		box* bx = *pbox;
		matr[bx->y][bx->x] = true;
	}

	simmetric = true;
	for(i = 0; i < height/2+1 && simmetric; i++)
		for(int j = 0; j < width; j++)
			if(matr[i][j] != matr[height-i-1][width-j-1]){
				simmetric = false;
				break;
			}

	if(cx < 0 || cy < 0)
		rotate_ability = false;
	else
		rotate_ability = true;

	center_x = cx - sh_x;
	center_y = cy - sh_y;
}

void Figure::bild_bound(void){
	int min_x = 99999999, max_x = -1;
	int min_y = 99999999, max_y = -1;
	for (box_container_t::iterator pbox = body.begin(); pbox != body.end(); pbox++){
		box* bx = *pbox;
		if(bx->x > max_x) max_x = bx->x;
		if(bx->x < min_x) min_x = bx->x;
		if(bx->y > max_y) max_y = bx->y;
		if(bx->y < min_y) min_y = bx->y;
	}
	left_x = the_x+min_x;
	top_y = the_y+min_y;
	width = (max_x - min_x) + 1;
	height= (max_y - min_y) + 1;
}

void Figure::draw(void){
	if(!color) return; // прозрачные не рисуем
	for (box_container_t::iterator pbox = body.begin(); pbox != body.end(); pbox++){
		box* bx = *pbox;
		bx->obj->set_R(game->game2world_coords(game->img_width*(the_x + bx->x), game->img_height*(the_y + bx->y)+shift_down, deep));
	}	
}

void Figure::set_xy(int x, int y){
	left_x += x - the_x;
	top_y += y - the_y;
	virt_y += y - the_y;
	the_x = x;
	the_y = y;
}

bool Figure::move_down(bool with_check){
	if(with_check && !shift_down)
		if(!is_able(0, 1)){
			virt_y = the_y;
			return false;
		}
	shift_down++;
	if(shift_down >= game->img_height){
		shift_down = 0;
		int delta = virt_y - the_y;
		set_xy(the_x, the_y+1);
		virt_y = the_y + ((delta > 0)?delta-1:0);
	}

	draw();
	return true;
}

bool Figure::move(int sx, int sy, bool slow){
	if(!is_able(sx, sy)) return false;
	if(!slow || sx || sy < 0){
		set_xy(the_x+sx, the_y+sy);
		draw();
	}else{
		for(sh_dwn_cont_t::iterator psd = game->shift_down_cont.begin(); psd != game->shift_down_cont.end(); ++psd)
			if((*psd)->obj == this) return false;
		sh_dwn_obj *dwn = new sh_dwn_obj;
		dwn->obj = this;
		dwn->speed = SHIFT_DOWN_TIME;
		dwn->shift = sy*game->img_height;
		dwn->pause = 0;
		game->shift_down_cont.push_back(dwn);
		virt_y += sy;
	}
	return true;
}

void Figure::set_color(int clr){
	color = clr;
	for (box_container_t::iterator pbox = body.begin(); pbox != body.end(); pbox++)
		(*pbox)->obj->set_state(color);
}

void Figure::set_deep(int dp){
	deep = dp;
}

void Figure::construct_by_template(int type, int clr, int x, int y){
	custom = false;
	color = clr;
	create(game->figure_templates[type].fig, game->figure_templates[type].x, game->figure_templates[type].y);
	base_state = 0;
	set_xy(x, y);
}

void Figure::construct_empty(){
	clear();
	custom = true;
	rotate_ability = false;
	color = 0;
	base_state = 0;
}

bool Figure::collision(Figure* sec){
	Figure *right, *left, *bottom, *top;
	
	// для ускореня для начала проверим пересекаются ли bound области
	if(sec->left_x >= left_x){
		right = sec;
		left = this;
	}else{
		right = this;
		left = sec;
	}
	if(right->left_x <= left->left_x + left->width-1){
		if(sec->top_y >= top_y){
			bottom = sec;
			top = this;
		}else{
			bottom = this;
			top = sec;
		}
		if(bottom->top_y > top->top_y + top->height-1)
			return false;
	
	}else
		return false;
	
	// по крайней мере области пересекаются, тогда теперь проверим сами объекты
	for (box_container_t::iterator pbox1 = body.begin(); pbox1 != body.end(); pbox1++){
		int x1 = the_x + (*pbox1)->x;
		int y1 = the_y + (*pbox1)->y;
		for (box_container_t::iterator pbox2 = sec->body.begin(); pbox2 != sec->body.end(); pbox2++)
			if(x1 == sec->the_x + (*pbox2)->x &&  y1 == sec->the_y + (*pbox2)->y)
				return true;
	}
	
	return false;	
}

bool Figure::is_able(int sx, int sy){
	if(custom) return true;
	set_xy(the_x+sx, the_y+sy);
	
	bool able = true;
	
	if(left_x < 0 || top_y < 0 || left_x + width > game->h_size || top_y + ((shift_down)?1:0) + height > game->v_size)
		able = false;
	
	if(able)
		for(figure_cont_t::iterator pfc = game->figures.begin(); pfc != game->figures.end() && able; ++pfc){
			Figure* f = *pfc;
			if(f == this) continue;
			int save_y = f->the_y;
			f->set_xy(f->the_x, f->virt_y);
			if(collision(f))
				able = false;
			else
				if(shift_down){
					set_xy(the_x, the_y+1);
					able = !collision(f);
					set_xy(the_x, the_y-1);
				}
			f->set_xy(f->the_x, save_y);
		}

	set_xy(the_x-sx, the_y-sy);

	return able;
}

int Figure::kill_chip(int x, int y){
	for (box_container_t::iterator pbox = body.begin(); pbox != body.end(); pbox++){
		box* bx = *pbox;
		if(the_x + bx->x == x && the_y + bx->y == y){
			bx->obj->set_R(game->game2world_coords(-1000, -1000));
			game->RetChip(bx->obj);
			delete bx;
			*pbox = NULL;
			break;
		}
	}
	body.remove(NULL);
	bild_bound();
	return body.size();
}

void Figure::add2glass(int cnt){
	for (box_container_t::iterator pbox = body.begin(); pbox != body.end(); pbox++){
		box* bx = *pbox;
		game->glass[the_y + bx->y][the_x + bx->x] += cnt;
	}
}

int Figure::add_chip(int x, int y){
	box* sq = new box(x, y);
	sq->obj = game->GetChip();
	sq->obj->set_state(color);
	body.push_back(sq);
	//bild_bound();
	return body.size();	
}
