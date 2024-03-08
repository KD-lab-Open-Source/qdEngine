#ifndef __VIEW_MANAGER_H_INCLUDED__
#define __VIEW_MANAGER_H_INCLUDED__

#include "XTL\Rect.h"

class ViewManager {
public:
	ViewManager ()
		: view_offset_ (Vect2f::ZERO)
		, view_scale_ (1.0f)
		, view_size_ (Vect2i::ZERO)
		, zoom_index_ (5)
	{}
    float viewScale() const { return zoomTable(zoom_index_); }
	const Vect2f& viewOffset() const { return view_offset_; }
	Vect2f coordsToScreen (const Vect2f& pos) const {
        return (pos + view_offset_) * viewScale () + Vect2f (view_size_) * 0.5f;
    }
	Vect2f coordsFromScreen (const Vect2f& pos) const {
        return (pos - Vect2f (view_size_) * 0.5f) / viewScale () - view_offset_;
    }
    void addOffset (Vect2f delta) {
        view_offset_ += delta;
    }
	void centerOn(const Vect2f& point) {
		view_offset_ = -point;
	}
    void setSize (int cx, int cy) {
        view_size_.set (cx, cy);
    }

	void setCustomZoom(float zoom)
	{
		zoom_index_ = -1;
		customZoom_ = zoom;
	}
	/*
    void setZoom (int index) {
        zoom_index_ = clamp (index, 0, maxZoom ());
    }
    int zoom() const {
        return zoom_index_;
    }
	*/
	/*
	void setOffset(const Vect2f& point){
		view_offset_ = point;
	}
	*/
	void zoom(bool in = true) {
		if(zoom_index_ == -1)
			for(int i = 0; i < maxZoom(); ++i)
				if(fabsf(zoomTable(i) - customZoom_) < FLT_COMPARE_TOLERANCE){
					zoom_index_ = i;
					break;
				}

		if(zoom_index_ == -1){
			if(customZoom_ > zoomTable(0))
				zoom_index_ = 0;
			else if(customZoom_ < zoomTable(maxZoom()))
				zoom_index_ = maxZoom();
			else
				for(int i = 1; i <= maxZoom(); ++i){
					if(zoomTable(i - 1) > customZoom_ && zoomTable(i) < customZoom_)
						zoom_index_ = in ? i - 1 : i;
				}
		}
		else{
			zoom_index_ = clamp(zoom_index_ + (in ? -1: 1), 0, maxZoom());
		}
	}
private:
    static int maxZoom () { return 9 - 1; }
    static float zoomTable (int index) {
        static const float zoom_table [] = { 8.0f, 4.0f, 3.0f, 2.0f, 1.5f, 1.0f, 0.75f, 0.5f, 0.25f };
        return zoom_table[index];
    }

	Vect2f view_offset_;
	float view_scale_;
	Vect2i view_size_;
	int zoom_index_;

	float customZoom_;
};

#endif
