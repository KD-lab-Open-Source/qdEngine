#ifndef Figure_h
#define  Figure_h true

#include "pre.h"

struct box{
	int x;
	int y;
	qdMinigameObjectInterface* obj;
	box(): x(-100), y(-100), obj(NULL){}
	box(int _x, int _y): x(_x), y(_y), obj(NULL){}
	~box(){}
};

typedef std::list<box*> box_container_t;

class Figure{
	// ручная обработка
	bool custom;	
	// левый верхний угол
	int left_x;
	int top_y;
	// общая кордината объекта
	int the_x;
	int the_y;
	// составляющие части
	box_container_t body;

	int width;
	int height;

	// цвет фигуры
	int color;
	// глубина
	int deep;
	// флаг симметричности фигуры
	bool simmetric;
	// отклонение от базового состояния
	int base_state;
	// возможность вращать
	int rotate_ability;
	// центр вращения
	int center_x, center_y;
	// размер плавного сдвига вниз
	int shift_down;
	// будущая координата Y
	int virt_y;

	// удалить все, что можно
	void clear(void);
	
	// создать структуры для хранения инфы об объекте
	void create(char**, int cx, int cy);
	// посчитать bound
	void bild_bound(void);

public:
	Figure();
	Figure(Figure*, int =0);
	~Figure();

	// указатель на класс игры
	static class AntiT_minigame *game;
	
	// создать фигуру по шаблону
	void construct_by_template(int type, int clr = 0, int x = -100, int y = -100);
	// создать пустую фигуру
	void construct_empty(void);
	// добавить кусок по заданным координатам
	int add_chip(int x, int y);
	// удалить кусок по заданным координатам
	int kill_chip(int x, int y);

	// задать цвет фигуры (состояние обектов)
	void set_color(int clr);
	// задать глубину фигуры на экране
	void set_deep(int dp);

	// задать координаты фигуры
	void set_xy(int x, int y);
	// dir - направление вращения (true - по часовой)
	bool rotate(bool dir = false, bool without_check = false);
	// относительный сдвиг в квадратах
	bool move(int sx, int sy, bool slow =true);
	// сдвинуть вниз на 1 пиксель
	bool move_down(bool with_check = true);

	// добавить себя к стакану
	void add2glass(int cnt);

	// нарисовать обект в текущих координатах
	void draw(void);

	//проверить пересечение с другой фигурой
	bool collision(Figure*);
	// возможность нахождения фигуры в текущем/новом месте;
	bool is_able(int sx =0, int sy =0);
};

#endif