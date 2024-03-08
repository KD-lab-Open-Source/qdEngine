#ifndef AntiT_minigame_h
#define  AntiT_minigame_h true

#include "pre.h"

typedef struct{
	Figure* obj;
	float pause;
	float speed;
	int shift;
}sh_dwn_obj;

typedef std::list<sh_dwn_obj*> sh_dwn_cont_t;

typedef std::list<Figure*> figure_cont_t;

typedef struct{
	char** fig;
	int x;
	int y;
}figure_shablon;

struct chip{
	qdMinigameObjectInterface* obj;
	bool in_use;
	chip(): in_use(false), obj(NULL){}
	~chip(){}
};

typedef std::vector<chip*> chip_cont_t;

class AntiT_minigame : public qdMiniGameInterface
{
	friend class Figure;
public:
	AntiT_minigame();
	~AntiT_minigame();

	// ������������� ����.
	bool init(const qdEngineInterface* p);

	// ������ ������ ����, �������� - �����, ������� ������ ������ � ���� (� ��������).
	bool quant(float dt);

	// ��������������� ����.
	bool finit();
private:

	// ��������� ��� ���������� � �������
	void clear_all(void);

	int templates_count;
	figure_shablon* figure_templates;
	// ��������� ������� �����
	bool load_template(const char* fname);
	
	// �������� �� ������� ��������� ��������� � �������
	mgVect3f game2world_coords(int x, int y, float deep = 0){
		return scene_ -> screen2world_coords(mgVect2i(x + shift_x, y + shift_y), deep);}

	mgVect3f game2world_coords(int ind, float deep = 0){
		return game2world_coords(ind % h_size * img_width, ind / h_size * img_height, deep);}
	
	// ���
	qdMinigameObjectInterface* back_object;
	/// ���� ������
	qdMinigameObjectInterface* state_flag;


	// ��������� ��������
	chip_cont_t chip_cont;

	// ������ ������������ ��������
	sh_dwn_cont_t shift_down_cont;
	
	// ������� ��� ������ � ���������� ��������
	qdMinigameObjectInterface* GetChip(void);
	void RetChip(qdMinigameObjectInterface*);

	//����������� �����
	Figure* vline;
	void create_vline(int size);
	// ������ ������� ��� ���������
	int last_x_vline;
	int last_y_vline;
	float prob_shift;
	bool need_vline;

	// ��������� ������� ������� � �������� ������
	float bounce;
	int pressed_key;
	int last_process_key;
	
	// ������
	int** glass;
	// ������ ����� � �������
	figure_cont_t figures;
	// �������� ������
	sh_dwn_obj* current;
	// ������� ������� ������
	void create_current(void);
	// ��������� ����������
	void process_keyboard(float dt);

	// �������� ������� ������ ����
	bool move_down_quere(float dt);

	// ������ ������� ��� �������
	void calc_state(void);

	// ������� ����� �����
	void kill_chips(int h, int v1, int v2, bool with_vline =false);

	// ���� ����������
	bool flag_lose;

	// ���� ������������� ������� ���������
	bool flag_calc_state;

	int crushing_lines_count;
	// ���� �������� �������� �������
	bool is_change_process;

	// ������������� ������� ����� ������ �� ��������� ����
	bool need_current;

	// ������ ���� �� ��������� � �������
	int v_size;
	// ������ ���� �� ����������� � �������
	int h_size;

	// ������ � ������ ������
	int	img_width;
	int img_height;

	// ������������ ����� ������������� ��������
	int max_vseg;

	// ������� (�������) ����� �� ����������� ��� ����������
	int crush_width;

	// ������� ����� ����������� ����� ���� ��������� ��� ������
	int crush_height;
	
	// ������� ����������� ����� ��������
	int shift_x;
	int shift_y;

	// ��������� � ������
	const qdEngineInterface* engine_;
	// ��������� � ������� �����
	qdMinigameSceneInterface* scene_;

};

#endif