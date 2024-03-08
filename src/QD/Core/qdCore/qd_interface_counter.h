#ifndef __QD_INTERFACE_COUNTER__
#define __QD_INTERFACE_COUNTER__

#include "qd_named_object_reference.h"
#include "qd_screen_text.h"

#include "qd_interface_element.h"

//! ������������ ������� ��� ����������� �������� ��������.

class qdInterfaceCounter : public qdInterfaceElement
{
public:
	qdInterfaceCounter();
	qdInterfaceCounter(const qdInterfaceCounter& counter);
	~qdInterfaceCounter();

	qdInterfaceCounter& operator = (const qdInterfaceCounter& counter);

	//! ���������� ��� ��������.
	qdInterfaceElement::element_type get_element_type() const { return qdInterfaceElement::EL_COUNTER; }

	//! ���������� ������� ����.
	bool mouse_handler(int x,int y,mouseDispatcher::mouseEvent ev);
	//! ���������� ����� � ����������.
	bool keyboard_handler(int vkey);

	grScreenRegion screen_region() const;

	//! ������������� ��������. 
	/**
	���������� ������ ��� ��� ������ �� �����, �������� ����������� �������.
	*/
	bool init(bool is_game_active = true);

	bool redraw() const;
	bool need_redraw() const;
	bool post_redraw();

	//! ���������� ������ �������� �� ����������� � ��������.
	int size_x() const;
	//! ���������� ������ �������� �� ��������� � ��������.
	int size_y() const;

	/// ���������� ������������ ��������
	int digits() const { return digits_; }
	void setDigits(int digits) { digits_ = digits; }

	const qdScreenTextFormat& textFormat() const { return textFormat_; }
	void setTextFormat(const qdScreenTextFormat& text_format){ textFormat_ = text_format; }

	const qdCounter* counter() const { return counter_; }
	void setCounter(const qdCounter* counter);

	const char* counterName() const;
	void setCounterName(const char* name);

protected:

	//! ������ ������ � ������.
	bool save_script_body(XStream& fh,int indent = 0) const;
	//! �������� ������ �� �������.
	bool load_script_body(const xml::tag* p);

	const char* data() const;

private:

	qdScreenTextFormat textFormat_;

	int digits_;

	std::string counterName_;
	const qdCounter* counter_;

	int lastValue_;
};

#endif /* __QD_INTERFACE_COUNTER__ */
