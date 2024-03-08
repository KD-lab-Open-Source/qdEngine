#pragma once

#include "kdw/API.h"

namespace kdw{

class KDW_API DragManager
{
public:
	DragManager() { dragging_ = false; }
	void beginDrag(int data);
	void drop();
	int data() const { return data_; }
	bool dragging() const { return dragging_; }
private:
	bool dragging_;
	int data_;
};

extern KDW_API DragManager dragManager;

}
