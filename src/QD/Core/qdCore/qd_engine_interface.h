#ifndef __QD_ENGINE_INTERFACE_H__
#define __QD_ENGINE_INTERFACE_H__

#include "qd_rnd.h"

class qdGameScene;

namespace qdmg {

/// ��������� � ������ ��� �������, ����������.
class qdEngineInterfaceImpl : public qdEngineInterface
{
public:
	static const qdEngineInterfaceImpl& instance();

	qdMinigameSceneInterface* current_scene_interface() const;
	qdMinigameSceneInterface* scene_interface(qdGameScene* scene) const;
	void release_scene_interface(qdMinigameSceneInterface* p) const;

	qdMinigameCounterInterface* counter_interface(const char* counter_name) const;
	void release_counter_interface(qdMinigameCounterInterface* p) const;
	
	//! ���������� ������ ������ � ��������.
	mgVect2i screen_size() const;

	//! ���������� true, ���� �� ���������� ������ ������ vkey.
	bool is_key_pressed(int vkey) const;

	//! ���������� true, ���� � ������ ������ ��������� ������� event_id.
	bool is_mouse_event_active(qdMinigameMouseEvent event_id) const;

	//! ���������� ������� ���������� �������� �������.
	mgVect2i mouse_cursor_position() const;

	/// ��������� ��������� ���� � ������� ��������.
	/// ���� ����� ��������� ����������, ���������� true.
	bool add_hall_of_fame_entry(int score) const;

	bool set_interface_text(const char* screen_name, const char* control_name, const char* text) const;

	/// ������������� rnd
	void rnd_init(int seed) const { qd_rnd_init(seed); }
	/// ���������� ��������� �������� � ��������� [0, m-1].
	unsigned rnd(unsigned m) const { return qd_rnd(m); }
	/// ���������� ��������� �������� � ��������� [-x, x].
	float frnd(float x) const { return qd_frnd(x); }
	/// ���������� ��������� �������� � ��������� [0, x].
	float fabs_rnd(float x) const { return qd_fabs_rnd(x); }
};

}; // namespace qdmg

#endif /* __QD_ENGINE_INTERFACE_H__ */

