#ifndef __CURVE_EDITOR_H_INCLUDED__
#define __CURVE_EDITOR_H_INCLUDED__

#include "kdw/_WidgetWithWindow.h"

namespace kdw{

class CurveEditorElement;
class CurveEditorImpl;


class CurveEditor : public _WidgetWithWindow{
public:
	CurveEditor(int border = 0);

	void clear();
	void addElement(CurveEditorElement* element);
	void rebuild();
	void recalculateViewport();

	float time() const;
    void setTime(float time);
	float duration() const{ return duration_; }
	void setDuration(float duration);

	void setSelectedElement(CurveEditorElement* element, bool grab);

	sigslot::signal0& signalTimeChanged(){ return signalTimeChanged_; }

protected:
	static const int TIMELINE_HEIGHT = 20; // pixels

	sigslot::signal0 signalTimeChanged_;
	CurveEditorImpl& impl();

	float time_;
	float duration_;
};

}

#endif
