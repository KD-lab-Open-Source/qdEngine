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

	// »нициализаци€ игры.
	bool init(const qdEngineInterface* p);

	// ќбсчЄт логики игры, параметр - врем€, которое должно пройти в игре (в секундах).
	bool quant(float dt);

	// ƒеинициализаци€ игры.
	bool finit();
private:

	// почистить все интерфейсы и массивы
	void clear_all(void);

	int templates_count;
	figure_shablon* figure_templates;
	// загрузить шаблоны фигур
	bool load_template(const char* fname);
	
	// пересчет из игровых индексных координат в мировые
	mgVect3f game2world_coords(int x, int y, float deep = 0){
		return scene_ -> screen2world_coords(mgVect2i(x + shift_x, y + shift_y), deep);}

	mgVect3f game2world_coords(int ind, float deep = 0){
		return game2world_coords(ind % h_size * img_width, ind / h_size * img_height, deep);}
	
	// фон
	qdMinigameObjectInterface* back_object;
	/// флаг победы
	qdMinigameObjectInterface* state_flag;


	// хранилище объектов
	chip_cont_t chip_cont;

	// список опускающихс€ объектов
	sh_dwn_cont_t shift_down_cont;
	
	// функции дл€ работы с хранилищем объектов
	qdMinigameObjectInterface* GetChip(void);
	void RetChip(qdMinigameObjectInterface*);

	//центральна€ лини€
	Figure* vline;
	void create_vline(int size);
	// верхнй сегмент дл€ продлени€
	int last_x_vline;
	int last_y_vline;
	float prob_shift;
	bool need_vline;

	// обработка времени нажати€ и дребезга клавиш
	float bounce;
	int pressed_key;
	int last_process_key;
	
	// стакан
	int** glass;
	// список фигур в стакане
	figure_cont_t figures;
	// падающа€ фигура
	sh_dwn_obj* current;
	// создать текущую фигуру
	void create_current(void);
	// обработка клавиатуры
	void process_keyboard(float dt);

	// обработа очереди сдвига вниз
	bool move_down_quere(float dt);

	// анализ стакана дл€ сдвигов
	void calc_state(void);

	// удалить часть фигур
	void kill_chips(int h, int v1, int v2, bool with_vline =false);

	// флаг проигрышаж
	bool flag_lose;

	// флаг необходимости обсчета состо€ни€
	bool flag_calc_state;

	int crushing_lines_count;
	// флаг текущего процесса падени€
	bool is_change_process;

	// необходимость создать новую фигуру на следующем шаге
	bool need_current;

	// размер пол€ по вертикали в €чейках
	int v_size;
	// размер пол€ по горизонтали в €чейках
	int h_size;

	// ширина и высота €чейки
	int	img_width;
	int img_height;

	// максимальна€ длина вертикального сегмента
	int max_vseg;

	// сколько (минимум) €чеек по горизонтале дл€ уничтожен€
	int crush_width;

	// сколько линий центральной линии надо заполнить дл€ сдвига
	int crush_height;
	
	// заранее посчитанный сдвиг картинки
	int shift_x;
	int shift_y;

	// интерфейс к движку
	const qdEngineInterface* engine_;
	// интерфейс к текущей сцене
	qdMinigameSceneInterface* scene_;

};

#endif