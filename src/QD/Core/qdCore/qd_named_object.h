#ifndef __QD_NAMED_OBJECT_H__
#define __QD_NAMED_OBJECT_H__

#include "qd_named_object_base.h"

//! ���� ������������� ��������.
enum qdNamedObjectType
{
	QD_NAMED_OBJECT_GENERIC,
	QD_NAMED_OBJECT_SCALE_INFO,
	QD_NAMED_OBJECT_TRIGGER_CHAIN,
	QD_NAMED_OBJECT_SOUND,
	QD_NAMED_OBJECT_ANIMATION,
	QD_NAMED_OBJECT_ANIMATION_INFO,
	QD_NAMED_OBJECT_COORDS_ANIMATION,
	QD_NAMED_OBJECT_OBJ_STATE,
	QD_NAMED_OBJECT_STATIC_OBJ,
	QD_NAMED_OBJECT_ANIMATED_OBJ,
	QD_NAMED_OBJECT_MOVING_OBJ,
	QD_NAMED_OBJECT_MOUSE_OBJ,
	QD_NAMED_OBJECT_SCENE,
	QD_NAMED_OBJECT_LOCATION,
	QD_NAMED_OBJECT_DISPATCHER,
	
	QD_NAMED_OBJECT_ANIMATION_SET,
	QD_NAMED_OBJECT_GRID_ZONE,
	QD_NAMED_OBJECT_VIDEO,
	QD_NAMED_OBJECT_INVENTORY,
	QD_NAMED_OBJECT_MINIGAME,
	QD_NAMED_OBJECT_MUSIC_TRACK,
	QD_NAMED_OBJECT_GRID_ZONE_STATE,
	
	QD_NAMED_OBJECT_SOUND_INFO,
	QD_NAMED_OBJECT_ANIMATION_SET_INFO,
	
	QD_NAMED_OBJECT_GAME_END,
	QD_NAMED_OBJECT_COUNTER,

	QD_NAMED_OBJECT_FONT_INFO,

	QD_NAMED_OBJECT_MAX_TYPE
};

//! ������������� ������.
class qdNamedObject : public qdNamedObjectBase
{
public:
	qdNamedObject();
	qdNamedObject(const qdNamedObject& p);
	~qdNamedObject();

	qdNamedObject& operator = (const qdNamedObject& p);

	//! ���������� ��������� �������.
	qdNamedObject* owner() const { return owner_; }
	//! ���������� ��������� �������, ��� �������� tp.
	qdNamedObject* owner(qdNamedObjectType tp) const;

#ifndef _QUEST_EDITOR
	//! ������������� ��������� �������.
	void set_owner(qdNamedObject* p){ owner_ = p; }
#else
	qdNamedObject* ref_owner() const { return ref_owner_; }
	void set_ref_owner(qdNamedObject* p){ ref_owner_ = p; }
	void set_owner(qdNamedObject* p){ owner_ = ref_owner_ = p; }
#endif

	//! ������������� ����.
	void set_flag(int fl){ flags_ |= fl; }
	//! ��������� ����.
	void drop_flag(int fl){ flags_ &= ~fl; }
	//! ���������� true, ���� ���������� ���� fl.
	bool check_flag(int fl) const { if(flags_ & fl) return true; return false; }
	//! ������� �����.
	void clear_flags(){ flags_ = 0; }
	//! ���������� �������� ������ �������.
	int flags() const { return flags_; }

	//! ���������� ��� �������.
	virtual int named_object_type() const = 0;

	//! �������� ������ �� �����.
	virtual bool load_data(qdSaveStream& fh,int save_version);
	//! ������ ������ � ����.
	virtual bool save_data(qdSaveStream& fh) const;

	//! ��������� ������ �� ��������� �� ������.
	void add_trigger_reference(){ trigger_reference_count_++; }
	//! ������� ������ �� ��������� �� ������.
	void remove_trigger_reference(){ if(trigger_reference_count_) trigger_reference_count_--; }
	//! ������� ������� ������ �� ��������� �� ������.
	void clear_trigger_references(){ trigger_reference_count_ = 0; }
	//! ���������� true, ���� �� ������ ���� ������ �� ���������.
	bool is_in_triggers() const { return (trigger_reference_count_ > 0); }

private:

	//! ����� �������� �������.
	int flags_;

	//! ���������� ������ �� ������ �� ���������.
	int trigger_reference_count_;

	//! �������� �������.
	mutable qdNamedObject* owner_;
#ifdef _QUEST_EDITOR
	mutable qdNamedObject* ref_owner_;
#endif
};

appLog& operator << (appLog& log,const qdNamedObject* obj);

#endif /* __QD_NAMED_OBJECT_H__ */