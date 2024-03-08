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

const unsigned int BACKUP_BALLS_INT = 3; /// Количество запасных шариков (сколько игрок может упустить шаров)

/// Пример миниигры - простой арканоид.
class minigameSample : public qdMiniGameInterface
{
public:
	minigameSample();
	~minigameSample(){ }

	/// Инициализация игры.
	bool init(const qdEngineInterface* p);

	/// Обсчёт логики игры, параметр - время, которое должно пройти в игре (в секундах).
	bool quant(float dt);

	/// Деинициализация игры.
	bool finit();
private:

	/// шарик
	qdMinigameObjectInterface* ball_object_;
	/// ракетка
	qdMinigameObjectInterface* board_object_;
	/// поле
	qdMinigameObjectInterface* field_object_;

	/// режимы движения шарика
	enum {
		/// шарик жестко привязан к ракетке
		BALL_LOCKED,
		/// шарик свободно движется по игровому полю
		BALL_FREE
	} ball_movement_mode_;

	/// Количество оставшихся шариков
	int backup_balls_num_;

	typedef std::vector<qdMinigameObjectInterface*> ball_objects_container_t;
	/// запасные шарики
	ball_objects_container_t backup_balls_;

	/// текущая скорость шарика
	mgVect2f ball_speed_;
	/// начальная скорость шарика, задаётся в параметрах
	mgVect2f ball_speed_initial_;
	/// случайная добавка к начальной скорости шарика, задаётся в параметрах
	mgVect2f ball_speed_initial_rnd_;

	typedef std::list<qdMinigameObjectInterface*> brick_objects_container_t;
	/// игровое поле, которое надо разбить шариком
	brick_objects_container_t brick_objects_;

	/// интерфейс к движку
	const qdEngineInterface* engine_;
	/// интерфейс к текущей сцене
	qdMinigameSceneInterface* scene_;

	/// ограничивает положение объекта object - его границы не доолжны выходить за границы объекта bound_object
	/**
	возвращает true, если object вылезал за границы bound_object и был подвинут
	*/
	bool constrain_object_position(qdMinigameObjectInterface* object, const qdMinigameObjectInterface* bound_object);
	/// обработка столкновения шарика с объектом object
	/**
	возвращает true, если было столкновение
	*/
	bool object_collision(qdMinigameObjectInterface* object);

	/// проверка, сталкивается ли шарик с вертикальным отрезком (v0, v1)
	bool vertical_edge_collision(const mgVect3f& v0, const mgVect3f& v1) const;
	/// проверка, сталкивается ли шарик с горизонтальным отрезком (v0, v1)
	bool horizontal_edge_collision(const mgVect3f& v0, const mgVect3f& v1) const;

	/// загрузка игрового поля
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

minigameSample::minigameSample() : ball_object_(NULL),
	board_object_(NULL),
	field_object_(NULL),
	ball_movement_mode_(BALL_LOCKED),
	backup_balls_num_(0)
{
}

bool minigameSample::init(const qdEngineInterface* p)
{
	engine_ = p;
	if(scene_ = p -> current_scene_interface()){
		field_object_ = scene_ -> object_interface(scene_ -> minigame_parameter("field_object_name"));

		const char* file_name = scene_ -> minigame_parameter("map_file_name");
		if(!load_level(file_name)){
			finit();
			return false;
		}

		ball_object_ = scene_ -> object_interface(scene_ -> minigame_parameter("ball_object_name"));
		board_object_ = scene_ -> object_interface(scene_ -> minigame_parameter("board_object_name"));

		sscanf(scene_ -> minigame_parameter("ball_speed"),"%f%f",&ball_speed_initial_.x,&ball_speed_initial_.y);
		sscanf(scene_ -> minigame_parameter("ball_speed_rnd"),"%f%f",&ball_speed_initial_rnd_.x,&ball_speed_initial_rnd_.y);

		float x = field_object_ -> R().x;
		float y = field_object_ -> R().y - field_object_ -> bound().y/2;

		board_object_ -> set_R(mgVect3f(x,y,0));

		y += ball_object_ -> bound().y/2 + board_object_ -> bound().y/2;
		ball_object_ -> set_R(mgVect3f(x,y,0));

		ball_movement_mode_ = BALL_LOCKED;

		backup_balls_num_ = BACKUP_BALLS_INT;
		// Создаем шарики, выводимые как запасные (если они еще не были созданы)
		if (backup_balls_.empty())
		{
			backup_balls_.reserve(BACKUP_BALLS_INT);
			for(int i = 0; i < BACKUP_BALLS_INT; i++)
			{
				char name[256];
				sprintf(name,"%s%.4d",scene_ -> minigame_parameter("ball_object"),i);
				backup_balls_[i] = scene_ -> object_interface(name);
			}
		}
		// Расставляем запасные шарики
		for(int i = 0; i < BACKUP_BALLS_INT; i++)
		{
			mgVect3f pos;
			pos.x = field_object_ -> R().x - field_object_ -> bound().x/2 - 1.5f * ball_object_ -> bound().x;
			pos.y = field_object_ -> R().y + field_object_ -> bound().y/2 - i * 2 * ball_object_ -> bound().y;
			pos.z = 0;
			backup_balls_[i]->set_R(pos);
		}
	}
	else
		return false;

	return true;
}

bool minigameSample::quant(float dt)
{
	mgVect3f r = board_object_ -> R();
	r.x = scene_ -> screen2grid_coords(engine_ -> mouse_cursor_position()).x;
	board_object_ -> set_R(r);
	constrain_object_position(board_object_, field_object_);

	switch(ball_movement_mode_){
	case BALL_LOCKED:
		r = board_object_ -> R();
		r.y += ball_object_ -> bound().y/2 + board_object_ -> bound().y/2;
		ball_object_ -> set_R(r);
		if(engine_->is_mouse_event_active(qdEngineInterface::MOUSE_EV_LEFT_DOWN)){
			ball_movement_mode_ = BALL_FREE;

			ball_speed_ = ball_speed_initial_;
			ball_speed_.x += engine_ -> frnd(ball_speed_initial_rnd_.x);
			ball_speed_.y += engine_ -> frnd(ball_speed_initial_rnd_.y);
		}
		break;
	case BALL_FREE:
		r = ball_object_ -> R();
		r.x += ball_speed_.x * dt;
		r.y += ball_speed_.y * dt;
		ball_object_ -> set_R(r);

		object_collision(board_object_);
		for(brick_objects_container_t::iterator it = brick_objects_.begin(); it != brick_objects_.end(); ++it){
			if((*it) -> is_visible() && object_collision(*it)){
				int idx = (*it) -> current_state_index();
				(*it) -> set_state(idx ? idx - 1 : 0);
			}
		}

		if(r.x > field_object_ -> R().x + field_object_ -> bound().x/2 - ball_object_ -> bound().x/2 && ball_speed_.x > 0)
			ball_speed_.x = -ball_speed_.x + engine_ -> frnd(1);
		if(r.x < field_object_ -> R().x - field_object_ -> bound().x/2 + ball_object_ -> bound().x/2 && ball_speed_.x < 0)
			ball_speed_.x = -ball_speed_.x + engine_ -> frnd(1);

		if(r.y > field_object_ -> R().y + field_object_ -> bound().y/2 - ball_object_ -> bound().y/2 && ball_speed_.y > 0)
			ball_speed_.y = -ball_speed_.y + engine_ -> frnd(1);
		// Ситуация проигрыша
		if(r.y < field_object_ -> R().y - field_object_ -> bound().y/2 - ball_object_ -> bound().y)
		{
			if (backup_balls_num_ > 0) 
			{
				// Уводим за экран один запасной шарик
				backup_balls_[backup_balls_num_-1]->set_R(mgVect3f(-1000, 0, 0)); 
				// Восстанавливаем шарик на ракетке за счет использования запасного
				backup_balls_num_--;
				ball_movement_mode_ = BALL_LOCKED;
			}
			else init(engine_);
		}
		break;
	}

	return true;
}

bool minigameSample::finit()
{
	if (NULL != scene_)
	{		
		for (ball_objects_container_t::iterator it = backup_balls_.begin(); it != backup_balls_.end(); ++it)
			scene_ -> release_object_interface(*it);
		backup_balls_.clear();
	
		for (brick_objects_container_t::iterator br_it = brick_objects_.begin(); br_it != brick_objects_.end(); ++br_it)
			scene_ -> release_object_interface(*br_it);	
		brick_objects_.clear();
		
		if (NULL != field_object_) 
			scene_ -> release_object_interface(field_object_);
		field_object_ = NULL;
		if (NULL != board_object_) 
			scene_ -> release_object_interface(board_object_);
		board_object_ = NULL;
		if (NULL != ball_object_) 
			scene_ -> release_object_interface(ball_object_);
		ball_object_ = NULL;

		engine_ -> release_scene_interface(scene_);
		scene_ = NULL;
	}
	return true;
}

bool minigameSample::constrain_object_position(qdMinigameObjectInterface* object, const qdMinigameObjectInterface* bound_object)
{
	bool ret = false;

	mgVect3f r = object -> R();
	mgVect3f bound = object -> bound();

	mgVect3f r0 = bound_object -> R();
	mgVect3f bound0 = bound_object -> bound();

	if(r.x - bound.x/2 < r0.x - bound0.x/2){
		ret = true;
		r.x = r0.x - bound0.x/2 + bound.x/2;
	}
	if(r.x + bound.x/2 > r0.x + bound0.x/2){
		ret = true;
		r.x = r0.x + bound0.x/2 - bound.x/2;
	}

	if(r.y - bound.y/2 < r0.y - bound0.y/2){
		ret = true;
		r.y = r0.y - bound0.y/2 + bound.y/2;
	}
	if(r.y + bound.y/2 > r0.y + bound0.y/2){
		ret = true;
		r.y = r0.y + bound0.y/2 - bound.y/2;
	}

	if(ret)
		object -> set_R(r);

	return ret;
}

bool minigameSample::object_collision(qdMinigameObjectInterface* object)
{
	bool ret = false;

	mgVect3f d = object -> R() - ball_object_ -> R();
	mgVect3f bound = ball_object_ -> bound()/2 + object -> bound()/2;

	if(fabs(d.x) < bound.x && fabs(d.y) < bound.y){
		if(d.x < 0 && ball_speed_.x < 0){
			mgVect3f v0 = object -> R() + object->bound()/2;
			mgVect3f v1 = object -> R() + object->bound()/2;

			v1.y -= object -> bound().y;

			if(vertical_edge_collision(v0, v1)){
				ret = true;
				ball_speed_.x = -ball_speed_.x + engine_ -> frnd(1);
			}
		}
		if(d.x > 0 && ball_speed_.x > 0){
			mgVect3f v0 = object -> R() - object->bound()/2;
			mgVect3f v1 = object -> R() - object->bound()/2;

			v1.y += object -> bound().y;

			if(vertical_edge_collision(v0, v1)){
				ret = true;
				ball_speed_.x = -ball_speed_.x + engine_ -> frnd(1);
			}
		}

		if(d.y < 0 && ball_speed_.y < 0){
			mgVect3f v0 = object -> R() + object->bound()/2;
			mgVect3f v1 = object -> R() + object->bound()/2;

			v1.x -= object -> bound().x;

			if(horizontal_edge_collision(v0, v1)){
				ret = true;
				ball_speed_.y = -ball_speed_.y + engine_ -> frnd(1);
			}
		}
		if(d.y > 0 && ball_speed_.y > 0){
			mgVect3f v0 = object -> R() - object->bound()/2;
			mgVect3f v1 = object -> R() - object->bound()/2;

			v1.x += object -> bound().x;

			if(horizontal_edge_collision(v0, v1)){
				ret = true;
				ball_speed_.y = -ball_speed_.y + engine_ -> frnd(1);
			}
		}
	}

	return ret;
}

bool minigameSample::vertical_edge_collision(const mgVect3f& v0, const mgVect3f& v1) const
{
	float y = ball_object_ -> R().y;
	float y0 = (v0.y < v1.y) ? v0.y : v1.y;
	float y1 = (v0.y < v1.y) ? v1.y : v0.y;

	float r = ball_object_ -> bound().x/2;
	float r2 = r * r;

	double dx = fabs(ball_object_ -> R().x - v0.x);

	if(y < y0){
		return (dx * dx + (y - y0) * (y - y0) < r2);
	}
	else {
		if(y < y1){
			return (dx < r);
		}
		else {
			return (dx * dx + (y - y1) * (y - y1) < r2);
		}
	}

	return false;
}

bool minigameSample::horizontal_edge_collision(const mgVect3f& v0, const mgVect3f& v1) const
{
	float x = ball_object_ -> R().x;
	float x0 = (v0.x < v1.x) ? v0.x : v1.x;
	float x1 = (v0.x < v1.x) ? v1.x : v0.x;

	float r = ball_object_ -> bound().x/2;
	float r2 = r * r;

	double dy = fabs(ball_object_ -> R().y - v0.y);

	if(x < x0){
		return (dy * dy + (x - x0) * (x - x0) < r2);
	}
	else {
		if(x < x1){
			return (dy < r);
		}
		else {
			return (dy * dy + (x - x1) * (x - x1) < r2);
		}
	}

	return false;
}

bool minigameSample::load_level(const char* fname)
{
	std::fstream file;
	file.open(fname, std::ios::in);
	if (file.fail()) return false;

	// Сначала уничтожаем все объекты-кирпичи
	for (brick_objects_container_t::iterator it = brick_objects_.begin(); it != brick_objects_.end(); ++it)
		scene_ -> release_object_interface(*it);
	brick_objects_.clear();
	
	int blocks_num;
	file >> blocks_num;

	char name[256];
	for (int i = 0; i < blocks_num; i++)
	{
		sprintf(name,"%s%.4d",scene_ -> minigame_parameter("brick_object"),i);
		if(qdMinigameObjectInterface* obj = scene_ -> object_interface(name))
		{
			float x, y;
			int state;
			
			file >> x >> y >> state;

			x += field_object_->R().x;
			y += field_object_->R().y;

			obj -> set_state(state);
			obj -> set_R(mgVect3f(x, y, 0));

			brick_objects_.push_back(obj);
		}
	}
	file.close();
	return true;
}
