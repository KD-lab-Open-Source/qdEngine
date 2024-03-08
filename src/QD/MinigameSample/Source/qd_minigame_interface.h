#ifndef __QD_MINIGAME_INTERFACE_H__
#define __QD_MINIGAME_INTERFACE_H__

class qdEngineInterface;

//! Интерфейсный класс для подключения мини-игр через dll.
/**
В dll с игрой должны быть две функции:
qdMiniGameInterface* open_game_interface(const char* game_name); - открывает интерфейс игры,
bool close_game_interface(qdMiniGameInterface* p); - закрывает интерфейс игры.

Формат конфигурационного .ini файла (этот файл указывается в редакторе в параметрах миниигры):

Имя секции - название параметра,
ключи:

1. type = "string" / "file" / "object" - тип данных (строка, файл, объект)

string: просто некие текстовые данные для миниигры.

file: данные - имя файла, который нужен для работы миниигы.
Если есть такие файлы, то их обязательно надо заносить в параметры, чтобы 
они корректно копировались при финальной сборке игры.

object: данные - имя объекта из сцены для миниигры
Используется в ситуации, когда миниигре требуется большое количество одинаковых объектов из сцены.
При загрузке игры в сцене создаётся заказанное количество копий объекта.

2. count - количество копий объекта, для строк и файлов игнорируется
3. value - строка данных по-умолчанию (в зависимости от типа данных - просто строка, имя файла или имя объекта,
для копий объекта к этому имени добавляется черырёхзначный индекс, отсчитывающийся от нуля)

В редакторе эту строку можно менять, при этом изменения пишутся в скрипт игры.

4. comment - комментарий

Если тип данных не указан, то считается равным "string".

Пример:

[ParameterExample]
type = "object"
count = 2
value = "Object0"
comment = "пример параметра"
*/
class qdMiniGameInterface
{
public:
	//! Возвращает указатель на интерфейс к игре, параметр - имя конфигурационного файла.
	typedef qdMiniGameInterface* (*interface_open_proc)(const char* game_name);
	//! Очищает интерфейс к игре.
	typedef bool (*interface_close_proc)(qdMiniGameInterface* p);

	virtual ~qdMiniGameInterface(){ };

	//! Инициализация игры.
	virtual bool init(const qdEngineInterface* engine_interface) = 0;

	//! Обсчёт логики игры, параметр - время, которое должно пройти в игре (в секундах).
	virtual bool quant(float dt) = 0;

	//! Деинициализация игры.
	virtual bool finit() = 0;

	/// Версия интерфейса игры, трогать не надо.
	enum { INTERFACE_VERSION = 101 };
	virtual int version() const { return INTERFACE_VERSION; }
};

template<class T>
class mgVect2 {
public:
	mgVect2(T value = (T)0) : x(value), y(value) { }
	mgVect2(T xx, T yy) : x(xx), y(yy) { }

	mgVect2& operator += (const mgVect2& v) { x += v.x; y += v.y; return *this; }
	mgVect2& operator -= (const mgVect2& v) { x -= v.x; y -= v.y; return *this; }
	mgVect2& operator *= (T v) { x *= v; y *= v; return *this; }
	mgVect2& operator /= (T v) { x /= v; y /= v; return *this; }

	mgVect2 operator + (const mgVect2& v) { return mgVect2(*this) += v; }
	mgVect2 operator - (const mgVect2& v) { return mgVect2(*this) -= v; }
	mgVect2 operator / (T v) { return mgVect2(*this) /= v; }
	mgVect2 operator * (T v) { return mgVect2(*this) *= v; }

	T x;
	T y;
};

template<class T>
class mgVect3 {
public:
	mgVect3(T value = (T)0) : x(value), y(value), z(value) { }
	mgVect3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) { }

	mgVect3& operator += (const mgVect3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	mgVect3& operator -= (const mgVect3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	mgVect3& operator *= (T v) { x *= v; y *= v; z *= v; return *this; }
	mgVect3& operator /= (T v) { x /= v; y /= v; z /= v; return *this; }

	mgVect3 operator + (const mgVect3& v) { return mgVect3(*this) += v; }
	mgVect3 operator - (const mgVect3& v) { return mgVect3(*this) -= v; }
	mgVect3 operator / (T v) { return mgVect3(*this) /= v; }
	mgVect3 operator * (T v) { return mgVect3(*this) *= v; }

	T x;
	T y;
	T z;
};

typedef mgVect3<float> mgVect3f;
typedef mgVect2<float> mgVect2f;
typedef mgVect2<int> mgVect2i;

/// Возвращает случайное значение в интервале [0, v-1].
unsigned mg_rnd(unsigned v);
/// Возвращает случайное значение в интервале [-v, v].
float mg_frnd(float v);
/// Возвращает случайное значение в интервале [0, v].
float mg_fabs_rnd(float v);

//! Интерфейс к динамическому объекту.
class qdMinigameObjectInterface
{
public:
	virtual ~qdMinigameObjectInterface(){ };

	//! Возвращает true, если у объекта есть состояние с именем state_name.
	virtual bool has_state(const char* state_name) const = 0;
	//! Возвращает имя активного в данный момент состояния.
	//! Вернёт 0, если активного состояния нету.
	virtual const char* current_state_name() const = 0;
	//! Возвращает true, если состояние с именем state_name включено в данный момент.
	virtual bool is_state_active(const char* state_name) const = 0;
	//! Возвращает true, если состояние с именем state_name в данный момент ожидает активации.
	virtual bool is_state_waiting_activation(const char* state_name) const = 0;
	//! Возвращает номер включенного в данный момент состояния.
	virtual int current_state_index() const = 0;
	//! Включает состояние с именем state_name.
	virtual bool set_state(const char* state_name) = 0;
	//! Включает состояние номер state_index (отсчитывается от нуля).
	virtual bool set_state(int state_index) = 0;
	//! Возвращает номер состояния с именем state_name.
	/**
	Отсчитывается от нуля, если такого состояния нет, то
	возвращает -1.
	*/
	virtual int state_index(const char* state_name) const = 0;

	//! Возвращает координаты объекта в мировой системе координат.
	virtual mgVect3f R() const = 0;
	//! Устанавливает координаты объекта в мировой системе координат.
	virtual void set_R(const mgVect3f& r) = 0;

	//! Возвращает координаты объекта в экранной системе координат.
	virtual mgVect2i screen_R() const = 0;
	//! Обновляет координаты объекта в экранной системе координат.
	virtual bool update_screen_R() = 0;
	//! Возвращает текущие экранные размеры объекта в пикселах.
	virtual mgVect2i screen_size() const = 0;

	//! Возвращает true, если объект не спрятан.
	virtual bool is_visible() const = 0;

	//! Возвращает размеры объекта в мировой системе координат.
	virtual mgVect3f bound() const = 0;

	//! Команда персонажу идти к точке target_position.
	/**
	 Если второй параметр равен false, то если target_position непроходима
	 персонаж идёт к ближайшей от target_position проходимой точке.
	 Для обычных объектов - всегда возвращает false
	*/
	virtual bool move(const mgVect3f& target_position,bool disable_target_change = false) = 0;
};

//! Интерфейс к сцене.
class qdMinigameSceneInterface
{
public:
	virtual ~qdMinigameSceneInterface(){ }

	//! Создаёт интерфейс к объекту с именем object_name.
	virtual qdMinigameObjectInterface* object_interface(const char* object_name) = 0;
	//! Создаёт интерфейс к персонажу с именем personage_name.
	virtual qdMinigameObjectInterface* personage_interface(const char* personage_name) = 0;
	//! Активация персонажа с именем personage_name.
	virtual bool activate_personage(const char* personage_name) = 0;

	//! Преобразование из экранных координат в мировые.
	virtual mgVect3f screen2world_coords(const mgVect2i& screen_pos,float screen_depth = 0) const = 0;
	//! Преобразование из мировых координат в экранные.
	virtual mgVect2i world2screen_coords(const mgVect3f& world_pos) const = 0;
	//! Возвращает "глубину" точки с координатами pos в мировой системе координат
	virtual float screen_depth(const mgVect3f& pos) const = 0;
	//! Возвращает мировые координаты точки на сетке по её экранным координатам.
	virtual mgVect3f screen2grid_coords(const mgVect2i& screen_pos) const = 0;

	//! Создаёт интерфейс к объекту, который взят мышью в данный момент.
	virtual qdMinigameObjectInterface* mouse_object_interface() const = 0;

	virtual const char* minigame_parameter(const char* parameter_name) const = 0;

	virtual void release_object_interface(qdMinigameObjectInterface* p) const = 0;
};

/// Интерфейс к счётчику.
class qdMinigameCounterInterface
{
public:
	virtual ~qdMinigameCounterInterface(){ }

	/// возвращает текущее значение счётчика
	virtual int value() const = 0;
	/// устанавливает текущее значение счётчика
	virtual void set_value(int value) = 0;
	/// добавляет к текущему значению счётчика value_delta
	virtual void add_value(int value_delta) = 0;
};

class qdEngineInterface
{
public:
	virtual ~qdEngineInterface(){ }

	//! Идентификаторы событий для работы с мышью.
	enum qdMinigameMouseEvent {
		//! Нажатие левой кнопки.
		MOUSE_EV_LEFT_DOWN = 0,
		//! Нажатие правой кнопки.
		MOUSE_EV_RIGHT_DOWN,
		//! Двойное нажатие левой кнопки.
		MOUSE_EV_LEFT_DBLCLICK,
		//! Двойное нажатие правой кнопки.
		MOUSE_EV_RIGHT_DBLCLICK,
		//! Отжатие левой кнопки.
		MOUSE_EV_LEFT_UP,
		//! Отжатие правой кнопки.
		MOUSE_EV_RIGHT_UP,
		//! Перемещение мыши.
		MOUSE_EV_MOUSE_MOVE
	};

	virtual qdMinigameSceneInterface* current_scene_interface() const = 0;
	virtual void release_scene_interface(qdMinigameSceneInterface* p) const = 0;

	virtual qdMinigameCounterInterface* counter_interface(const char* counter_name) const = 0;
	virtual void release_counter_interface(qdMinigameCounterInterface* p) const = 0;

	//! Возвращает размер экрана в пикселах.
	virtual mgVect2i screen_size() const = 0;

	//! Возвращает true, если на клавиатуре нажата кнопка vkey.
	virtual bool is_key_pressed(int vkey) const = 0;

	//! Возвращает true, если в данный момент произошло событие event_id.
	virtual bool is_mouse_event_active(qdMinigameMouseEvent event_id) const = 0;

	//! Возвращает текущие координаты мышиного курсора.
	virtual mgVect2i mouse_cursor_position() const = 0;

	/// Возвращает случайное значение в интервале [0, m-1].
	virtual unsigned rnd(unsigned m) const = 0;
	/// Возвращает случайное значение в интервале [-x, x].
	virtual float frnd(float x) const = 0;
	/// Возвращает случайное значение в интервале [0, x].
	virtual float fabs_rnd(float x) const = 0;
};

#endif /* __QD_MINIGAME_INTERFACE_H__ */

