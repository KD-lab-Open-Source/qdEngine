#ifndef __QD_MINIGAME_INTERFACE_H__
#define __QD_MINIGAME_INTERFACE_H__

class qdEngineInterface;

//! ������������ ����� ��� ����������� ����-��� ����� dll.
/**
� dll � ����� ������ ���� ��� �������:
qdMiniGameInterface* open_game_interface(const char* game_name); - ��������� ��������� ����,
bool close_game_interface(qdMiniGameInterface* p); - ��������� ��������� ����.

������ ����������������� .ini ����� (���� ���� ����������� � ��������� � ���������� ��������):

��� ������ - �������� ���������,
�����:

1. type = "string" / "file" / "object" - ��� ������ (������, ����, ������)

string: ������ ����� ��������� ������ ��� ��������.

file: ������ - ��� �����, ������� ����� ��� ������ �������.
���� ���� ����� �����, �� �� ����������� ���� �������� � ���������, ����� 
��� ��������� ������������ ��� ��������� ������ ����.

object: ������ - ��� ������� �� ����� ��� ��������
������������ � ��������, ����� �������� ��������� ������� ���������� ���������� �������� �� �����.
��� �������� ���� � ����� �������� ���������� ���������� ����� �������.

2. count - ���������� ����� �������, ��� ����� � ������ ������������
3. value - ������ ������ ��-��������� (� ����������� �� ���� ������ - ������ ������, ��� ����� ��� ��� �������,
��� ����� ������� � ����� ����� ����������� ������������� ������, ��������������� �� ����)

� ��������� ��� ������ ����� ������, ��� ���� ��������� ������� � ������ ����.

4. comment - �����������

���� ��� ������ �� ������, �� ��������� ������ "string".

������:

[ParameterExample]
type = "object"
count = 2
value = "Object0"
comment = "������ ���������"
*/

class qdMinigameSceneInterface;

class qdMiniGameInterface
{
public:
	//! ���������� ��������� �� ��������� � ����, �������� - ��� ����������������� �����.
	typedef qdMiniGameInterface* (*interface_open_proc)(const char* game_name);
	//! ������� ��������� � ����.
	typedef bool (*interface_close_proc)(qdMiniGameInterface* p);

	virtual ~qdMiniGameInterface(){ };

	//! ������������� ����.
	virtual bool init(const qdEngineInterface* engine_interface) = 0;

	//! ������ ������ ����, �������� - �����, ������� ������ ������ � ���� (� ��������).
	virtual bool quant(float dt) = 0;

	//! ��������������� ����.
	virtual bool finit() = 0;

	/// ������������� ��������, ���������� ��� ������ � ����������� ����.
	virtual bool new_game(const qdEngineInterface* engine_interface){ return true; }
	/// ���������� ������, ���������� ��� ���������� �����, �� ������� �������� ��������.
	virtual int save_game(const qdEngineInterface* engine_interface, const qdMinigameSceneInterface* scene_interface, char* buffer, int buffer_size){ return 0; }
	/// �������� ������, ���������� ��� �������� �����, �� ������� �������� ��������.
	virtual int load_game(const qdEngineInterface* engine_interface, const qdMinigameSceneInterface* scene_interface, const char* buffer, int buffer_size){ return 0; }

	/// ������ ���������� ����, ������� �� ����.
	enum { INTERFACE_VERSION = 112 };
	virtual int version() const { return INTERFACE_VERSION; }
};

template<class T>
class mgVect2 {
public:
	explicit mgVect2(T value = (T)0) : x(value), y(value) { }
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
	explicit mgVect3(T value = (T)0) : x(value), y(value), z(value) { }
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

//! ��������� � ������������� �������.
class qdMinigameObjectInterface
{
public:
	virtual ~qdMinigameObjectInterface(){ };

	/// ��� �������.
	virtual const char* name() const = 0;

	//! ���������� true, ���� � ������� ���� ��������� � ������ state_name.
	virtual bool has_state(const char* state_name) const = 0;
	//! ���������� ��� ��������� � ������ ������ ���������.
	//! ����� 0, ���� ��������� ��������� ����.
	virtual const char* current_state_name() const = 0;
	//! ���������� true, ���� ��������� � ������ state_name �������� � ������ ������.
	virtual bool is_state_active(const char* state_name) const = 0;
	//! ���������� true, ���� ��������� � ������ state_name � ������ ������ ������� ���������.
	virtual bool is_state_waiting_activation(const char* state_name) const = 0;
	//! ���������� ����� ����������� � ������ ������ ���������.
	virtual int current_state_index() const = 0;
	//! �������� ��������� � ������ state_name.
	virtual bool set_state(const char* state_name) = 0;
	//! �������� ��������� ����� state_index (������������� �� ����).
	virtual bool set_state(int state_index) = 0;
	//! ���������� ����� ��������� � ������ state_name.
	/**
	������������� �� ����, ���� ������ ��������� ���, ��
	���������� -1.
	*/
	virtual int state_index(const char* state_name) const = 0;

	//! ���������� ���������� ������� � ������� ������� ���������.
	virtual mgVect3f R() const = 0;
	//! ������������� ���������� ������� � ������� ������� ���������.
	virtual void set_R(const mgVect3f& r) = 0;

	/// ��������, �������� �� ����� � ��������� ������������ pos � ������
	virtual bool hit_test(const mgVect2i& pos) const = 0;

	//! ���������� ���������� ������� � �������� ������� ���������.
	virtual mgVect2i screen_R() const = 0;
	//! ��������� ���������� ������� � �������� ������� ���������.
	virtual bool update_screen_R() = 0;
	//! ���������� ������� �������� ������� ������� � ��������.
	virtual mgVect2i screen_size() const = 0;

	//! ������������� ������� �������� �������.
	//! angle - ����, �� ������� ������ ���� �������� ��������, � ��������
	//! speed - �������� ��������, � �������� � �������
	virtual void set_screen_rotation(float angle, float speed) = 0;
	//! ���������� ������� �������� ������� � ��������.
	virtual float screen_rotation() const = 0;
	//! ������������� ��������������� �������� �������.
	virtual void set_screen_scale(const mgVect2f& scale, const mgVect2f& speed) = 0;
	//! ���������� ������� �������� �������.
	virtual mgVect2f screen_scale() const = 0;

	//! ���������� ���� ���������.
	virtual int shadow_color() const = 0;
	//! ���������� ������������ ���������, �������� - [0, 255], ���� ����� -1, �� ��������� ���.
	virtual int shadow_alpha() const = 0;
	//! ������������� ���������.
	virtual bool set_shadow(int shadow_color, int shadow_alpha) = 0;

	//! ���������� true, ���� ������ �� �������.
	virtual bool is_visible() const = 0;

	//! ���������� ������� ������� � ������� ������� ���������.
	virtual mgVect3f bound() const = 0;

	//! ������� ��������� ���� � ����� target_position.
	/**
	 ���� ������ �������� ����� false, �� ���� target_position �����������
	 �������� ��� � ��������� �� target_position ���������� �����.
	 ��� ������� �������� - ������ ���������� false
	*/
	virtual bool move(const mgVect3f& target_position,bool disable_target_change = false) = 0;

	//! ���������� �����������, � ������� ������� ��������.
	//! �������� � ��������, 0 ������������ ����������� ������, ������� �� ������� �������.
	//! ��� ������� �������� ������ ���������� 0.
	virtual float direction_angle() const = 0;
	//! ������������� ����������� ���������.
	virtual bool set_direction_angle(float direction) = 0;
};

//! ��������� � �����.
class qdMinigameSceneInterface
{
public:
	virtual ~qdMinigameSceneInterface(){ }

	/// ��� �����.
	virtual const char* name() const = 0;

	//! ������ ��������� � ������� � ������ object_name.
	virtual qdMinigameObjectInterface* object_interface(const char* object_name) = 0;
	//! ������ ��������� � ��������� � ������ personage_name.
	virtual qdMinigameObjectInterface* personage_interface(const char* personage_name) = 0;
	//! ��������� ��������� � ������ personage_name.
	virtual bool activate_personage(const char* personage_name) = 0;

	//! �������������� �� �������� ��������� � �������.
	virtual mgVect3f screen2world_coords(const mgVect2i& screen_pos,float screen_depth = 0) const = 0;
	//! �������������� �� ������� ��������� � ��������.
	virtual mgVect2i world2screen_coords(const mgVect3f& world_pos) const = 0;
	//! ���������� "�������" ����� � ������������ pos � ������� ������� ���������
	virtual float screen_depth(const mgVect3f& pos) const = 0;
	//! ���������� ������� ���������� ����� �� ����� �� � �������� �����������.
	virtual mgVect3f screen2grid_coords(const mgVect2i& screen_pos) const = 0;

	//! ������ ��������� � �������, ������� ���� ����� � ������ ������.
	virtual qdMinigameObjectInterface* mouse_object_interface() const = 0;
	//! ������ ��������� � �������, �� �������� �������� �����.
	virtual qdMinigameObjectInterface* mouse_click_object_interface() const = 0;
	//! ������ ��������� � �������, �� �������� �������� ������ ������� ����.
	virtual qdMinigameObjectInterface* mouse_right_click_object_interface() const = 0;
	//! ������ ��������� � �������, ��� ������� ��������� ������� ������.
	virtual qdMinigameObjectInterface* mouse_hover_object_interface() const = 0;

	virtual const char* minigame_parameter(const char* parameter_name) const = 0;

	virtual void release_object_interface(qdMinigameObjectInterface* p) const = 0;
};

/// ��������� � ��������.
class qdMinigameCounterInterface
{
public:
	virtual ~qdMinigameCounterInterface(){ }

	/// ���������� ������� �������� ��������
	virtual int value() const = 0;
	/// ������������� ������� �������� ��������
	virtual void set_value(int value) = 0;
	/// ��������� � �������� �������� �������� value_delta
	virtual void add_value(int value_delta) = 0;
};

class qdEngineInterface
{
public:
	virtual ~qdEngineInterface(){ }

	//! �������������� ������� ��� ������ � �����.
	enum qdMinigameMouseEvent {
		//! ������� ����� ������.
		MOUSE_EV_LEFT_DOWN = 0,
		//! ������� ������ ������.
		MOUSE_EV_RIGHT_DOWN,
		//! ������� ������� ����� ������.
		MOUSE_EV_LEFT_DBLCLICK,
		//! ������� ������� ������ ������.
		MOUSE_EV_RIGHT_DBLCLICK,
		//! ������� ����� ������.
		MOUSE_EV_LEFT_UP,
		//! ������� ������ ������.
		MOUSE_EV_RIGHT_UP,
		//! ����������� ����.
		MOUSE_EV_MOUSE_MOVE
	};

	virtual qdMinigameSceneInterface* current_scene_interface() const = 0;
	virtual void release_scene_interface(qdMinigameSceneInterface* p) const = 0;

	virtual qdMinigameCounterInterface* counter_interface(const char* counter_name) const = 0;
	virtual void release_counter_interface(qdMinigameCounterInterface* p) const = 0;

	//! ���������� ������ ������ � ��������.
	virtual mgVect2i screen_size() const = 0;

	//! ���������� true, ���� �� ���������� ������ ������ vkey.
	virtual bool is_key_pressed(int vkey) const = 0;

	//! ���������� true, ���� � ������ ������ ��������� ������� event_id.
	virtual bool is_mouse_event_active(qdMinigameMouseEvent event_id) const = 0;

	//! ���������� ������� ���������� �������� �������.
	virtual mgVect2i mouse_cursor_position() const = 0;

	/// ��������� ��������� ���� � ������� ��������.
	/// ���� ����� ��������� ����������, ���������� true.
	virtual bool add_hall_of_fame_entry(int score) const = 0;

	/// ������������� ����� � ������������ ��������.
	/// ���� ��� ������ �������, ������� ������ � ������� ������.
	virtual bool set_interface_text(const char* screen_name, const char* control_name, const char* text) const = 0;

	/// ������������� rnd
	virtual void rnd_init(int seed) const = 0;
	/// ���������� ��������� �������� � ��������� [0, m-1].
	virtual unsigned rnd(unsigned m) const = 0;
	/// ���������� ��������� �������� � ��������� [-x, x].
	virtual float frnd(float x) const = 0;
	/// ���������� ��������� �������� � ��������� [0, x].
	virtual float fabs_rnd(float x) const = 0;
};

#endif /* __QD_MINIGAME_INTERFACE_H__ */

