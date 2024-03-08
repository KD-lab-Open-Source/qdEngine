#include "stdafx.h"
#include ".\frameselector.h"
#include "CustomRectTracker.h"

#include "qd_trigger_chain.h"
#include "SelectionManager.h"
#include "tewnd.h"
#include <boost/rtl.hpp>

#include "te_helpers.h"

namespace {
class SelectIfIntersect
{
	CRect const& rect_;
	SelectionManager& selectionManager_;
public:
	SelectIfIntersect(CRect const& rect, SelectionManager& selectionManager):
	  rect_(rect)
	, selectionManager_(selectionManager)
	{
	}
	void operator()(qdTriggerElementPtr const& ptr) const{
		CRect inters;
		inters.IntersectRect(rect_, &ptr->bounding_rect());
		if (te_h::IsKeyPressed(VK_SHIFT))
		{
			if (inters.IsRectEmpty())
			{
//				if (selectionManager_.isSelected(ptr))
//					selectionManager_.deselect(ptr);
//				else
//					selectionManager_.select(ptr);
				;
			}
			else
			{
//				if (selectionManager_.isSelected(ptr))
//					selectionManager_.deselect(ptr);
//				else
//					selectionManager_.select(ptr);
				;
			}
		}
		else{
			if (inters.IsRectEmpty())
				selectionManager_.deselect(ptr);
			else
				selectionManager_.select(ptr);
		}
	}
};

}
FrameSelector::FrameSelector(TEWnd* window, 
							 SelectionManager* selectionManager,
							 qdTriggerChain* triggerChain):
  window_(window)
, selMngr_(selectionManager)
, triggerChain_(triggerChain)
{
	assert(window_);
	assert(selMngr_);
	assert(triggerChain_);
}

FrameSelector::~FrameSelector(void)
{
}

void FrameSelector::onRectChange(CustomRectTracker& rectTracker, CRect const& oldRect)
{
	CRect rc(rectTracker.m_rect);
	rc.NormalizeRect();
	getWindow()->scr2log(&rc);
	boost::rtl::rng::for_each(getTriggerChain()->elements_list(), 
		SelectIfIntersect(rc, *getSelectionManager()));
	getWindow()->RedrawWindow();
	CDC* pdc = getWindow()->GetDC();
	rectTracker.Draw(pdc);
	getWindow()->ReleaseDC(pdc);
}

void FrameSelector::setSelectionManager(SelectionManager* selectionManager){
	selMngr_ = selectionManager;
}

void FrameSelector::setTriggerChain(qdTriggerChain* triggerChain){
	triggerChain_ = triggerChain;
}

SelectionManager* FrameSelector::getSelectionManager(){
	return selMngr_;
}
qdTriggerChain*	  FrameSelector::getTriggerChain(){
	return triggerChain_;
}

void FrameSelector::setWindow(TEWnd* window){
	window_ = window;
}

TEWnd* FrameSelector::getWindow(){
	return window_;
}