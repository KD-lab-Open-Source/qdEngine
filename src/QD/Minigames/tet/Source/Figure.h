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
	// ������ ���������
	bool custom;	
	// ����� ������� ����
	int left_x;
	int top_y;
	// ����� ��������� �������
	int the_x;
	int the_y;
	// ������������ �����
	box_container_t body;

	int width;
	int height;

	// ���� ������
	int color;
	// �������
	int deep;
	// ���� �������������� ������
	bool simmetric;
	// ���������� �� �������� ���������
	int base_state;
	// ����������� �������
	int rotate_ability;
	// ����� ��������
	int center_x, center_y;
	// ������ �������� ������ ����
	int shift_down;
	// ������� ���������� Y
	int virt_y;

	// ������� ���, ��� �����
	void clear(void);
	
	// ������� ��������� ��� �������� ���� �� �������
	void create(char**, int cx, int cy);
	// ��������� bound
	void bild_bound(void);

public:
	Figure();
	Figure(Figure*, int =0);
	~Figure();

	// ��������� �� ����� ����
	static class AntiT_minigame *game;
	
	// ������� ������ �� �������
	void construct_by_template(int type, int clr = 0, int x = -100, int y = -100);
	// ������� ������ ������
	void construct_empty(void);
	// �������� ����� �� �������� �����������
	int add_chip(int x, int y);
	// ������� ����� �� �������� �����������
	int kill_chip(int x, int y);

	// ������ ���� ������ (��������� �������)
	void set_color(int clr);
	// ������ ������� ������ �� ������
	void set_deep(int dp);

	// ������ ���������� ������
	void set_xy(int x, int y);
	// dir - ����������� �������� (true - �� �������)
	bool rotate(bool dir = false, bool without_check = false);
	// ������������� ����� � ���������
	bool move(int sx, int sy, bool slow =true);
	// �������� ���� �� 1 �������
	bool move_down(bool with_check = true);

	// �������� ���� � �������
	void add2glass(int cnt);

	// ���������� ����� � ������� �����������
	void draw(void);

	//��������� ����������� � ������ �������
	bool collision(Figure*);
	// ����������� ���������� ������ � �������/����� �����;
	bool is_able(int sx =0, int sy =0);
};

#endif