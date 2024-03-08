#include "stdafx.h"
#include "DragManager.h"

namespace kdw {

void DragManager::beginDrag(int data)
{
	data_ = data; 
	if(!dragging_)
		SetCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND)));
	dragging_ = true;
}

void DragManager::drop()
{
	if(dragging_)
		SetCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW)));
	dragging_ = false;
}

}