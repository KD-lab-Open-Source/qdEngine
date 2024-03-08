#ifndef __QD_GAME_OBJECT_STATE_H__
#define __QD_GAME_OBJECT_STATE_H__

#include "xml_fwd.h"

#include "snd_sound.h"

#include "qd_contour.h"
#include "qd_sound_info.h"
#include "qd_animation_info.h"
#include "qd_animation_set_info.h"
#include "qd_conditional_object.h"
#include "qd_named_object.h"
#include "qd_coords_animation.h"
#include "qd_camera_mode.h"
#include "qd_sound_handle.h"
#include "qd_screen_text.h"
#include "qd_grid_zone.h"

class XStream;

class qdScreenTransform
{
public:
	qdScreenTransform(float angle = 0.f, const Vect2f& scale = Vect2f(1.f, 1.f)) : angle_(angle), scale_(scale) { }

	bool operator == (const qdScreenTransform& trans) const;
	bool operator != (const qdScreenTransform& trans) const { return !(*this == trans); }
	bool operator ()() const { return *this != ID; }

	qdScreenTransform operator * (float value) const { return qdScreenTransform(angle_ * value, scale_ * value); }

	qdScreenTransform& operator += (const qdScreenTransform& trans)
	{
		angle_ += trans.angle_;
		scale_ += trans.scale_;

		return *this;
	}

	bool change(float dt, const qdScreenTransform& target_trans, const qdScreenTransform& speed);

	float angle() const { return angle_; }
	void set_angle(float angle){ angle_ = angle; }

	const Vect2f& scale() const { return scale_; }
	void set_scale(const Vect2f& scale){ scale_ = scale; }
	bool has_scale() const { return fabs(scale_.x - 1.f) > FLT_EPS || fabs(scale_.y - 1.f) > FLT_EPS; }

	/// ��������� �������������
	static qdScreenTransform ID;

private:

	float angle_;
	Vect2f scale_;

	friend XStream& operator < (XStream& fh, const qdScreenTransform& trans);
	friend XStream& operator > (XStream& fh, qdScreenTransform& trans);
};

XStream& operator < (XStream& fh, const qdScreenTransform& trans);
XStream& operator > (XStream& fh, qdScreenTransform& trans);

//! ��������� ������������� ������� - ������� �����.
class qdGameObjectState : public qdConditionalObject
{
public:
	static const Vect3f DEFAULT_BOUND;

	//! ����� ���������.
	enum {
		//! ������ �������.
		QD_OBJ_STATE_FLAG_HIDDEN			= 0x01,
		//! ��������������� ���������� ��������� �� ��������� ���������.
		QD_OBJ_STATE_FLAG_RESTORE_PREV_STATE		= 0x04,
		//! ������� ������ �� ��������� ���������.
		QD_OBJ_STATE_FLAG_HIDE_OBJECT			= 0x08,
		//! ������, �������� ����������� ��������� - ����������.
		QD_OBJ_STATE_FLAG_GLOBAL_OWNER			= 0x10,
		//! ��������� ��� ���������.
		QD_OBJ_STATE_FLAG_INVENTORY			= 0x20,
		//! �������� ������ � ��������� �� ��������� ���������.
		QD_OBJ_STATE_FLAG_MOVE_TO_INVENTORY		= 0x40,
		//! �������� � ��������� �� �������.
		QD_OBJ_STATE_FLAG_MOVE_TO_INVENTORY_FAILED	= 0x80,
		//! � ��������� ����� �����.
		QD_OBJ_STATE_FLAG_HAS_BOUND			= 0x100,
		//! ������������ �������� ����� ���������� ���������.
		QD_OBJ_STATE_FLAG_ACTIVATION_TIMER		= 0x200,
		//! �������� ����� ���������� ��������� ����������.
		QD_OBJ_STATE_FLAG_ACTIVATION_TIMER_END		= 0x400,
		//! �������� ��� ��������� ����� - ������� ����� ��� ��������.
		/**
		��������, ��� ���� �� ������� ��������� ��������� - ���� ����� ��
		���������������� ������ �� ������.
		*/
		QD_OBJ_STATE_FLAG_DIALOG_PHRASE			= 0x800,
		//! ���������������� �������� �� �����.
		/**
		����� ������������� ����, �������� ���������������.
		�������� ������ ��� qdGameObjectStateStatic ��� ������������ ��������.
		*/
		QD_OBJ_STATE_FLAG_SOUND_SYNC			= 0x1000,
		//! ��������� ��������� ���������.
		/**
		���� ���� ����������, �� ��������� ����� ���� �������� ��
		��������� ��� ������ (�������� ��� ��������� - ������ ����).
		*/
		QD_OBJ_STATE_FLAG_ENABLE_INTERRUPT		= 0x2000,
		//! ��������� ��� ���� ������������.
		/**
		����������, ��� ��������� ��� ���� ���� �� ���� ��� ������������.
		*/
		QD_OBJ_STATE_FLAG_WAS_ACTIVATED			= 0x4000,
		//! ��������� ��������� ������ � ��������� ����� ���������.
		/**
		����� ����� ������ ���� ��� ��������� � �������� � ����� �������
		(need_to_walk() == true).
		*/
		QD_OBJ_STATE_FLAG_DISABLE_WALK_INTERRUPT	= 0x8000,
		//! �������� ���������, ����� ������ ���� �� ����.
		QD_OBJ_STATE_FLAG_MOUSE_STATE			= 0x10000,
		//! �������� ���������, ����� ���� ��� �������� � ���������.
		QD_OBJ_STATE_FLAG_MOUSE_HOVER_STATE		= 0x20000,
		//! �� ����������� ������ �� ���������, ����� �� ���� �� ����.
		QD_OBJ_STATE_FLAG_STAY_IN_INVENTORY		= 0x40000,
		/** 
		������������� ��������� ������� ��������� � ������ ��� ������ �� �����,
		�� ��������� �� �� ������ �� �����.
		*/
		QD_OBJ_STATE_FLAG_FORCED_LOAD			= 0x80000,
		//! ��������� ����� ��������� ������ ����.
		QD_OBJ_STATE_FLAG_ENABLE_SKIP			= 0x100000,
		//! ��� ��������� ��������� ������ ������ �� ���� ��� �����.
		QD_OBJ_STATE_FLAG_MOVE_TO_ZONE			= 0x200000,
		//! ��� ��������� ��������� ������ ������ � ����� ������� ��� �����.
		QD_OBJ_STATE_FLAG_MOVE_ON_OBJECT		= 0x400000,
		//! �� ��������� ������ ��������� ������ ���������-��������� ��������.
		QD_OBJ_STATE_FLAG_ACTIVATE_PERSONAGE		= 0x800000,
		//! ������������� ��������� ���� ��� ��������� ���������.
		QD_OBJ_STATE_FLAG_AUTO_LOAD					= 0x1000000,
		//! ������������� ������ ���� ��� ��������� ���������.
		QD_OBJ_STATE_FLAG_AUTO_SAVE					= 0x2000000,
		QD_OBJ_STATE_FLAG_FADE_IN					= 0x4000000,
		QD_OBJ_STATE_FLAG_FADE_OUT					= 0x8000000,
	};

	//! ��� ���������.
	enum StateType {
		//! ����������� ��������.
		STATE_STATIC,
		//! �������.
		STATE_WALK,
		//! ����� �� ����.
		STATE_MASK
	};

	enum {	
		CURSOR_UNASSIGNED = -1
	};

	qdGameObjectState(StateType tp);
	qdGameObjectState(const qdGameObjectState& st);
	virtual ~qdGameObjectState();

	virtual qdGameObjectState& operator = (const qdGameObjectState& st);

	int named_object_type() const { return QD_NAMED_OBJECT_OBJ_STATE; }

	//! ���������� ��������� �� ���������� �������� ������� ��� ���������.
	qdCoordsAnimation* coords_animation(){ return &coords_animation_; }
	//! ���������� ��������� �� ���������� �������� ������� ��� ���������.
	const qdCoordsAnimation* coords_animation() const { return &coords_animation_; }

	//! �������� ������ �� �������.
	virtual bool load_script(const xml::tag* p) = 0;
	//! ������ ������ � ������.
	virtual bool save_script(XStream& fh,int indent = 0) const = 0;

	//! ������������� ���������, ���������� ��� ������ � ����������� ����.
	bool init();

	//! �������� ������ �� �����.
	bool load_data(qdSaveStream& fh,int save_version);
	//! ������ ������ � ����.
	bool save_data(qdSaveStream& fh) const;

	//! ����������� �������� ��������� � ���������� ��������.
	virtual bool register_resources();
	//! ������ ����������� �������� ��������� � ���������� ��������.
	virtual bool unregister_resources();
	//! �������� ��������.
	virtual bool load_resources();
	//! �������� ��������.
	virtual bool free_resources();

	//! ���������� true, ���� ������� ��������� ��������� �����������.
	bool check_conditions();

	//! ��������� �������� ������ ������� ��� ��������� (� �������� �����������).
	const Vect2s& center_offset() const { return center_offset_; }
	//! ������������� �������� ������ ������� ��� ��������� (� �������� �����������).
	void set_center_offset(const Vect2s offs){ center_offset_ = offs; }

	//! ���������� ����� ���������.
	void quant(float dt);

	//! ���������� ��������� �� ���������� ���������.
	qdGameObjectState* prev_state(){ return prev_state_; }
	//! ������������� ���������� ���������.
	void set_prev_state(qdGameObjectState* p){ prev_state_ = p; }

	//! ���������� ��� ���������.
	StateType state_type() const { return state_type_; }

	//! ���������� ������������� �������� ������� ��� ���������.
	int mouse_cursor_ID() const { return mouse_cursor_id_; }
	//! ������������� ������������� �������� ������� ��� ���������.
	/**
	���� ������ �������������� ������ ��-��������� -
	�������� ������ ���� ����� CURSOR_UNASSIGNED.
	*/
	void set_mouse_cursor_ID(int id){ mouse_cursor_id_ = id; }
	//! ���������� true, ���� ��������� �������� ������ ������� ������.
	bool has_mouse_cursor_ID() const { return mouse_cursor_id_ != CURSOR_UNASSIGNED; }

	//! ������������� ��� ��������� �������, ������������ � ���������.
	const char* sound_name() const { return sound_info_.name(); }
	//! ���������� ��� ��������� �������, ������������ � ���������.
	void set_sound_name(const char* p){ sound_info_.set_name(p); }
	//! ���������� ��������� �� �������� ������, ����������� � ���������.
	qdSound* sound() const;
	//! ���������� true, ���� ��� ��������� ����� ����.
	bool has_sound() const { if(sound_name()) return true; else return false; }
	//! ���������� ����� �����.
	const qdSoundHandle* sound_handle() const { return &sound_handle_; }
	//! ��������� ����, position - ��������� �������, �� 0.0 �� 1.0.
	bool play_sound(float position = 0.0f);
	//! ������������� ����.
	bool stop_sound() const;
	//! ��������� ������� �����.
	bool set_sound_frequency(float frequency_coeff) const;
	bool is_sound_finished() const;
	bool is_sound_playing() const;

	//! ������������� ���� ��� �����.
	void set_sound_flag(int fl){ sound_info_.set_flag(fl); }
	//! ��������� ���� ��� �����.
	void drop_sound_flag(int fl){ sound_info_.drop_flag(fl); }
	//! ���������� true, ���� ���������� ���� ��� �����.
	bool check_sound_flag(int fl) const { return sound_info_.check_flag(fl); }

	//! ���������� true, ���� ��������� ������.
	virtual bool is_state_empty() const;

	//! ���������� true, ���� � ������ ������ ��������� ��������.
	bool is_active() const;
	//! ���������� true, ���� ��������� ���������.
	bool is_default() const;

	//! ���������� �������� (� ��������) ����� ���������� ���������.
	float activation_delay() const { return activation_delay_; }
	//! ������������� �������� (� ��������) ����� ���������� ���������.
	void set_activation_delay(float tm){ activation_delay_ = tm; }
	//! ������������� ������ ����� ���������� ���������.
	void set_activation_timer(){ activation_timer_ = activation_delay_; }

	//! ���������� ��� ��������� ���������.
	void start(){ cur_time_ = 0.0f; is_sound_started_ = false; }

	//! ���������� ����� � ��������, ��������� � ������� ��������� ���������.
	float cur_time() const { return cur_time_; }
	//! ���������� ������������ ��������� � ��������.
	float work_time() const;
	//! ������������� ������������ ��������� � ��������.
	void set_work_time(float tm){ work_time_ = tm; }
	
	//! ���������� true, ���� ��������� ��������� ������� � ����� ��������� ���������.
	bool need_to_walk() const { 
		if(!coords_animation_.is_empty() 
			&& coords_animation_.check_flag(QD_COORDS_ANM_OBJECT_START_FLAG)) 
			return true; 
		else 
			return false;
	}

	virtual qdGameObjectState* clone() const = 0;

	virtual float adjust_direction_angle(float angle) const { return angle; }

#ifndef _QUEST_EDITOR
	//! ���������� ���������� �����, � ������� ������ �������������� ���������.
	const Vect3f& start_pos() const {
		if(!coords_animation_.is_empty()){
			return coords_animation_.get_point(0) -> dest_pos();
		}
		else
			return Vect3f::ZERO;
	}
	//! ���������� ����������� ������� � �����, � ������� ������ �������������� ���������.
	float start_direction_angle() const {
		if(!coords_animation_.is_empty()){
			return coords_animation_.get_point(0) -> direction_angle();
		}
		else
			return -1.0f;
	}
#endif	

	//! ���������� ���������� ������ �� ���������.
	int reference_count() const { return reference_count_; }
	//! ��������� ���������� ������ �� ���������.
	void inc_reference_count(){ reference_count_++; }
	//! ��������� ���������� ������ �� ���������.
	void dec_reference_count(){ if(reference_count_) reference_count_--; }

	//! ���������� true, ���� � ��������� ����� ����� ���������.
	bool has_text() const { return (!text_ID_.empty() || !short_text_ID_.empty()); }
	//! ���������� ����� ���������.
	const char* text() const { if(has_full_text()) return full_text(); else return short_text(); }
	//! ���������� ������ ����� ���������.
	const char* full_text() const;
	//! ���������� ����������� ����� ���������.
	const char* short_text() const;

	//! ���������� true, ���� � ��������� ����� ����� ���������.
	bool has_full_text() const { return !text_ID_.empty(); }
	//! ���������� true, ���� � ��������� ����� ����������� ����� ���������.
	bool has_short_text() const { return !short_text_ID_.empty(); }
	//! ���������� ������ ����� ���������.
	const char* full_text_ID() const { return text_ID_.c_str(); }
	//! ������������� ������ ����� ���������.
	/**
	���� �������� �������, �� ����� ���������.
	*/
	void set_full_text_ID(const char* p){ if(p) text_ID_ = p; else text_ID_.clear(); }
	//! ���������� ����������� ����� ���������.
	const char* short_text_ID() const { return short_text_ID_.c_str(); }
	//! ������������� ����������� ����� ���������.
	/**
	���� �������� �������, �� ����� ���������.
	*/
	void set_short_text_ID(const char* p){ if(p) short_text_ID_ = p; else short_text_ID_.clear(); }

	//! ���������� true, ���� � ��������� ����� �����.
	bool has_bound() const { return check_flag(QD_OBJ_STATE_FLAG_HAS_BOUND); }
	//! ���������� ����� ���������.
	const Vect3f& bound() const { return bound_; }
	//! ���������� ������ ���������.
	float radius() const { return radius_; }
	//! ������������� ����� ���������.
	void set_bound(const Vect3f& b);
	//! ������������� ����� ��������� �� ��������.
	virtual bool auto_bound(){ return false; }

	//! ������������� ����� ������ ������, ���������� ��� ��������� ���������.
	void set_camera_mode(const qdCameraMode& mode){ camera_mode_ = mode; }
	//! ����� ������ ������, ���������� ��� ��������� ���������.
	const qdCameraMode& camera_mode() const { return camera_mode_; }
	//! ���������� true, ���� � ��������� ����� ����� ������ ������.
	bool has_camera_mode() const { return camera_mode_.camera_mode() != qdCameraMode::MODE_UNASSIGNED; }

	float rnd_move_radius() const { return rnd_move_radius_; }
	void set_rnd_move_radius(float radius){ rnd_move_radius_ = radius; }

	float rnd_move_speed() const { return rnd_move_speed_; }
	void set_rnd_move_speed(float speed){ rnd_move_speed_ = speed; }

	qdConditionalObject::trigger_start_mode trigger_start();
	bool trigger_can_start() const;

	bool forced_load() const { return check_flag(QD_OBJ_STATE_FLAG_FORCED_LOAD); }

	float text_delay() const { return text_delay_; }
	bool has_text_delay() const { return text_delay_ > FLT_EPS; }
	void set_text_delay(float delay){ text_delay_ = delay; }

	float sound_delay() const { return sound_delay_; }
	bool has_sound_delay() const { return sound_delay_ > FLT_EPS; }
	void set_sound_delay(float delay){ sound_delay_ = delay; }

	int autosave_slot() const {	return autosave_slot_; }
	void set_autosave_slot(int slot){ autosave_slot_ = slot; }

	float fade_time() const { return fade_time_; }
	void set_fade_time(float time){ fade_time_ = time; }

	unsigned shadow_color() const { return shadow_color_; }
	int shadow_alpha() const { return shadow_alpha_; }

	void set_shadow(unsigned color, int alpha){ shadow_color_ = color; shadow_alpha_ = alpha; }

	const qdScreenTextFormat& text_format(bool topic_mode = false) const { 
#ifndef _QUEST_EDITOR
		if(text_format_.is_global_depend()){
			return (topic_mode && check_flag(QD_OBJ_STATE_FLAG_DIALOG_PHRASE)) ? 
				qdScreenTextFormat::global_topic_format() : qdScreenTextFormat::global_text_format();
		}
#endif
		return text_format_; 
	}
	void set_text_format(const qdScreenTextFormat& text_format) { text_format_ = text_format; }

#ifdef _QUEST_EDITOR
	//! ������ ��� �������. ������ ������, ���� ���������� ������ �� ���������
	std::string const& cursor_name() const;
	//! ��������� ����� �������
	void set_cursor_name(std::string const& cursor_name);
	//! �� ����� ������� ���������� ��� �����
	void update_cursor_id(qdGameDispatcher const& gameDispatcher);
#endif // _QUEST_EDITOR

	bool has_transform() const { return transform_() || transform_speed_(); }

	const qdScreenTransform& transform() const { return transform_; }
	void set_transform(const qdScreenTransform& tr){ transform_ = tr; }
	const qdScreenTransform& transform_speed() const { return transform_speed_; }
	void set_transform_speed(const qdScreenTransform& tr_speed){ transform_speed_ = tr_speed; }

protected:

	//! �������� ������ �� �������.
	bool load_script_body(const xml::tag* p);
	//! ������ ������ � ������.
	bool save_script_body(XStream& fh,int indent = 0) const;

	//! ���������� true, ���� ���� ������������� ����.
	virtual bool need_sound_restart() const;

private:

	//! ��� ���������.
	StateType state_type_;

	//! �������� ������ �������� ������������ ������ ������� � �������� �����������.
	Vect2s center_offset_;
	//! �����.
	Vect3f bound_;
	//! ������ �����, ����������� �����.
	float radius_;

	//! ���������� �������� �������.
	qdCoordsAnimation coords_animation_;

	//! �������� ����� ���������� ��������� (� ��������)
	float activation_delay_;
	//! ����� � ��������, ���������� �� ��������� ���������.
	float activation_timer_;

	//! ������������ ��������� � ��������.
	/**
	���� �������, �� ��������� �������� �� ����� ��������
	��� �� ����� ���������� ��������.
	*/
	float work_time_;
	//! ����� � ��������, ��������� � ������� ��������� ���������.
	float cur_time_;

	//! ���������� � �����, ����������� � ���������.
	qdSoundInfo sound_info_;
	//! ����� ��� ���������� ������, ����������� � ���������.
	qdSoundHandle sound_handle_;
	//! �������� ������� ����� �� ������ ��������� (� ��������)
	float sound_delay_;
	//! true, ���� ���� ��������� �������
	bool is_sound_started_;

	//! �������� ����� ���������� ������ �� ������ ��������� (� ��������)
	float text_delay_;
	//! true, ���� ����� ��������� ��������
	bool is_text_shown_;

	//! ����� �������� �������, ������� ����������, ���� ���� ��� �������� � ���� ���������.
	int mouse_cursor_id_;

	//! �����, ��������� �� ����� ��� ������ ��������� (��� �������� � �.�.)
	std::string text_ID_;
	//! �������� ������� ������, ���������� �� ����� ��� ������ ��������� (��� �������� � �.�.)
	std::string short_text_ID_;

	//! ����� ������ ������, ���������� ��� ��������� ���������.
	qdCameraMode camera_mode_;

	float rnd_move_radius_;
	float rnd_move_speed_;

	/// ����� ����� ���������
	int autosave_slot_;

	/// ����� ����� ������ ��� ��������� ���������
	float fade_time_;

	//! ���� ���������.
	unsigned shadow_color_;
	//! ������������ ���������, �������� - [0, 255], ���� ����� QD_NO_SHADOW_ALPHA, �� ������ �� ����������.
	int shadow_alpha_;

	/// �������������� �������� �������, ���������� ��� ��������� ���������
	qdScreenTransform transform_;
	qdScreenTransform transform_speed_;

	//! ������ ������.
	qdScreenTextFormat text_format_;

	//! ���������� ������ �� ���������.
	/**
	���� ������, �������� ����������� ��������� - ���������� (�.�. �����������
	�������� �����������, � �� �����), �� ��������� ����� ������������ ���������� � ����� ��� ����� ������.
	*/
	int reference_count_;

	//! ���������� ���������.
	qdGameObjectState* prev_state_;

#ifdef _QUEST_EDITOR
	std::string cursor_name_;
#endif // _QUEST_EDITOR
};

typedef std::vector<qdGameObjectState*> qdGameObjectStateVector;

//! ��������� ������������� ������� - ����������� ��������.
class qdGameObjectStateStatic : public qdGameObjectState
{
public:
	qdGameObjectStateStatic();
	qdGameObjectStateStatic(const qdGameObjectStateStatic& st);
	~qdGameObjectStateStatic();
	
	qdGameObjectState& operator = (const qdGameObjectState& st);
	qdGameObjectStateStatic& operator = (const qdGameObjectStateStatic& st);
	
	qdAnimation* animation(){ return animation_info_.animation(); }
	const qdAnimation* animation() const { return animation_info_.animation(); }
	qdAnimationInfo* animation_info(){ return &animation_info_; }
#ifdef _QUEST_EDITOR
	qdAnimationInfo const* animation_info() const{ return &animation_info_; }
#endif // _QUEST_EDITOR
	
	bool load_script(const xml::tag* p);
	bool save_script(XStream& fh,int indent = 0) const;

	//! ����������� �������� ��������� � ���������� ��������.
	bool register_resources();
	//! ������ ����������� �������� ��������� � ���������� ��������.
	bool unregister_resources();
	bool load_resources();
	bool free_resources();
	
	bool is_state_empty() const;

	qdGameObjectState* clone() const {
		return new qdGameObjectStateStatic(*this);
	}

	//! ������������� ����� ��������� �� ��������.
	bool auto_bound();

private:
	qdAnimationInfo animation_info_;
};

//! ��������� ������������� ������� - �������.
class qdGameObjectStateWalk : public qdGameObjectState
{
public:
	qdGameObjectStateWalk();
	qdGameObjectStateWalk(const qdGameObjectStateWalk& st);
	~qdGameObjectStateWalk();
	
	qdGameObjectState& operator = (const qdGameObjectState& st);
	qdGameObjectStateWalk& operator = (const qdGameObjectStateWalk& st);

	//! ������ ������������ ��� ���������
	enum movement_type_t {
		//! ����� ������ �����
		MOVEMENT_LEFT,
		//! ����� ������ �����
		MOVEMENT_UP,
		//! ����� ������ ������
		MOVEMENT_RIGHT,
		//! ����� ������ ����
		MOVEMENT_DOWN,
		//! ����� ������ �� �����������
		MOVEMENT_HORIZONTAL,
		//! ����� ������ �� ���������
		MOVEMENT_VERTICAL,
		//! ����� �� ������� ������������
		MOVEMENT_FOUR_DIRS,
		//! ����� �� ������ ������������
		MOVEMENT_EIGHT_DIRS,
		//! ����� �� ������ � ������ ������������ �� ������������
		MOVEMENT_SMOOTH,

		//! ����� ������ �����-�����
		MOVEMENT_UP_LEFT,
		//! ����� ������ ������-�����
		MOVEMENT_UP_RIGHT,
		//! ����� ������ ������-����
		MOVEMENT_DOWN_RIGHT,
		//! ����� ������ �����-����
		MOVEMENT_DOWN_LEFT
	};

	enum OffsetType
	{
		OFFSET_STATIC = 0,
		OFFSET_WALK,
		OFFSET_START,
		OFFSET_END
	};

	qdAnimationSet* animation_set() const;

	qdAnimation* animation(float direction_angle);
	qdAnimation* static_animation(float direction_angle);

	qdAnimationInfo* animation_info(float direction_angle);
	qdAnimationInfo* static_animation_info(float direction_angle);

	const Vect2i& center_offset(int direction_index, OffsetType offset_type = OFFSET_WALK) const;
	const Vect2i& center_offset(float direction_angle, OffsetType offset_type = OFFSET_WALK) const;
	void set_center_offset(int direction_index,const Vect2i& offs, OffsetType offset_type = OFFSET_WALK);

	const float walk_sound_frequency(int direction_index) const;
	const float walk_sound_frequency(float direction_angle) const;
	void set_walk_sound_frequency(int direction_index,float freq);

	//! ��������� ������� �����.
	bool update_sound_frequency(float direction_angle) const;

	qdAnimationSetInfo* animation_set_info(){ return &animation_set_info_; }

	float adjust_direction_angle(float angle) const;

	float direction_angle() const { return direction_angle_; }
	void set_direction_angle(float ang){ direction_angle_ = ang; }

	bool load_script(const xml::tag* p);
	bool save_script(XStream& fh,int indent = 0) const;

	//! ����������� �������� ��������� � ���������� ��������.
	bool register_resources();
	//! ������ ����������� �������� ��������� � ���������� ��������.
	bool unregister_resources();
	bool load_resources();
	bool free_resources();

	bool is_state_empty() const;

	qdGameObjectState* clone() const {
		return new qdGameObjectStateWalk(*this);
	}
	
	//! ������������� ����� ��������� �� ��������.
	bool auto_bound();

	float acceleration() const { return acceleration_; }
	void set_acceleration(float acc){ acceleration_ = acc; }

	float max_speed() const { return max_speed_; }
	void set_max_speed(float max_sp){ max_speed_ = max_sp; }

	void set_movement_type(movement_type_t type){ movement_type_ = type; }
	movement_type_t movement_type() const { return movement_type_; }

protected:

	//! ���������� true, ���� ���� ������������� ����.
	bool need_sound_restart() const;

private:

	float direction_angle_;
	qdAnimationSetInfo animation_set_info_;

	//! ��������� - ��������� ������������� �������� ������������ �� �������.
	float acceleration_;
	//! ������������ ��� ������� �������� ������������.
	/**
	���� ������� - ����������� ���.
	*/
	float max_speed_;

	//! ����� ������������ ���������.
	movement_type_t movement_type_;

	//! �������� ������� �������� �������.
	std::vector<Vect2i> center_offsets_;
	//! �������� ������� ����������� ��������.
	std::vector<Vect2i> static_center_offsets_;
	//! �������� ������� �������� �������.
	std::vector<Vect2i> start_center_offsets_;
	//! �������� ������� �������� ������.
	std::vector<Vect2i> stop_center_offsets_;

	//! ������������ ��� ������� ����� �������.
	std::vector<float> walk_sound_frequency_;
};

//! ��������� ������������� ������� - ����� �� ����������� �������.
class qdGameObjectStateMask : public qdGameObjectState, public qdContour
{
public:
	qdGameObjectStateMask();
	qdGameObjectStateMask(const qdGameObjectStateMask& st);
	~qdGameObjectStateMask();

	qdGameObjectState& operator = (const qdGameObjectState& st);
	qdGameObjectStateMask& operator = (const qdGameObjectStateMask& st);

	const char* parent_name() const { return parent_name_.c_str(); }
	void set_parent_name(const char* p){ parent_name_ = p; parent_ = 0; }

	qdGameObject* parent();
	const qdGameObject* parent() const;

	bool hit(int x,int y) const;
	bool draw_mask(unsigned color) const;

	bool can_be_closed() const { return (contour_size() > 2); }

	bool load_script(const xml::tag* p);
	bool save_script(XStream& fh,int indent = 0) const;

	bool is_state_empty() const;

	bool load_resources();

	qdGameObjectState* clone() const {
		return new qdGameObjectStateMask(*this);
	}
	
#ifdef _QUEST_EDITOR
	void copy_contour(qdGameObjectStateMask const* source);
#endif // _QUEST_EDITOR

private:

	//! ��� �������, � �������� ��������� �����.
	std::string parent_name_;

	//! ��������� �� ������, � �������� ��������� �����.
	qdGameObject* parent_;
};

#ifdef __QD_DEBUG_ENABLE__
inline bool qdbg_is_object_state(const qdNamedObject* obj,const char* scene_name,const char* object_name,const char* state_name)
{
	if(dynamic_cast<const qdGameObjectState*>(obj)){
		if(obj -> name() && !strcmp(state_name,obj -> name())){
			if(!object_name || (obj -> owner() && obj -> owner() -> name() && !strcmp(object_name,obj -> owner() -> name()))){
				if(!scene_name || (obj -> owner() -> owner() && obj -> owner() -> owner() -> name() && !strcmp(obj -> owner() -> owner() -> name(),scene_name)))
					return true;
			}
		}
	}
	return false;
}
#else
inline bool qdbg_is_object_state(const qdNamedObject* obj,const char* scene_name,const char* object_name,const char* state_name){ return false; }
#endif

#endif /* __QD_GAME_OBJECT_STATE_H__ */

