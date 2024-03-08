#ifndef __KDW_EDIT_H_INCLUDED__
#define __KDW_EDIT_H_INCLUDED__

#include <string>
#include "kdw/_WidgetWithWindow.h"
#include "kdw/_Enums.h"

namespace Win32{
	class Window;
}

namespace kdw{
class KDW_API EntrySelection{
public:
	EntrySelection(int start = 0, int end = 0)
	: start_(start), end_(end)
	{
	}
	int start() const{ return start_; }
	int end() const{ return end_; }
	int length() const{ return end_ - start_; }
protected:
	int start_;
	int end_;
};


class EntryImpl;
class KDW_API Entry : public _WidgetWithWindow{
public:
	Entry(const char* text = "", bool multline = false, int border = 0);
	~Entry();

	TextAlignHorizontal textAlign() const{ return align_; }
	void setTextAlign(TextAlignHorizontal textAlign) { align_ = textAlign; }

	EntrySelection selection() const;
	void setSelection(EntrySelection selection);

	void replace(EntrySelection selection, const char* text);
	const char* text() const{ return text_.c_str(); }
	void setText(const char* text);

	void setSwallowReturn(bool swallow){ swallowReturn_ = swallow; }

	void serialize(Archive& ar);

	virtual void onChanged() { signalChanged_.emit(); }
	sigslot::signal0& signalChanged() { return signalChanged_; }

	void commit(); // вызывает onEdited, если есть изменения
	virtual void onEdited() { signalEdited_.emit(); }
	sigslot::signal0& signalEdited() { return signalEdited_; }

	virtual void onSelectionChanged() { signalSelectionChanged_.emit(); }
	sigslot::signal0& signalSelectionChanged() { return signalSelectionChanged_; }
protected:
	friend EntryImpl;
	EntryImpl* impl() const;

	sigslot::signal0 signalEdited_;
	sigslot::signal0 signalChanged_;
	sigslot::signal0 signalSelectionChanged_;

	std::string text_;
	TextAlignHorizontal align_;
	bool swallowReturn_;
};

}

#endif
