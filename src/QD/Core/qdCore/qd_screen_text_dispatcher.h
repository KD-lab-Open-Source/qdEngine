#ifndef __QD_SCREEN_TEXT_DISPATCHER__
#define __QD_SCREEN_TEXT_DISPATCHER__

#include "mouse_input.h"
#include "qd_screen_text_set.h"

//! ��������� �������� �������.
class qdScreenTextDispatcher
{
public:
	qdScreenTextDispatcher();
	~qdScreenTextDispatcher();

	//! ������� ���� �������.
	void clear_texts();
	//! ������� ���� ������� � ���������� owner.
	void clear_texts(qdNamedObject* owner);

	//! ���������� ������ � ������������ �����.
	qdScreenText* add_text(int set_ID,const qdScreenText& txt);
	//! ���������� ������ �������.
	qdScreenTextSet* add_text_set(const qdScreenTextSet& set){ text_sets_.push_back(set); return &text_sets_.back(); }
	//! ����� ������ �������.
	const qdScreenTextSet* get_text_set(int id) const;
	//! ����� ������ �������.
	qdScreenTextSet* get_text_set(int id);

	//! ��������� �������.
	void redraw() const;
	void pre_redraw() const;
	void post_redraw();

	bool save_script(XStream& fh,int indent = 0) const;

private:

	typedef std::vector<qdScreenTextSet> text_sets_container_t;
	//! ������ �������.
	text_sets_container_t text_sets_;
};

#endif /* __QD_SCREEN_TEXT_DISPATCHER__ */






















