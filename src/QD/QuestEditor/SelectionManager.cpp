// SelectionManager.cpp: implementation of the SelectionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SelectionManager.h"
#include "qd_trigger_chain.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SelectionManager::SelectionManager()
{

}

SelectionManager::~SelectionManager()
{

}

size_t SelectionManager::getCount() const{
	return selected_.size();
}

bool SelectionManager::empty() const{
	return selected_.empty();
}

bool SelectionManager::isSelected(qdTriggerElementPtr const& element) const{
	return element->selected();
}

void SelectionManager::select(qdTriggerElementPtr const& element){
	select(element, true);
}

void SelectionManager::reselect(qdTriggerElementPtr const& element){
	deselect(element);
	select(element);
}

void SelectionManager::deselect(qdTriggerElementPtr const& element){
	select(element, false);
}

void SelectionManager::deselectAll()
{
	for(; !selected_.empty(); )
		deselect(selected_.front());
}

void SelectionManager::clear(){
	selected_.clear();
}

qdTriggerElementPtr const SelectionManager::getLast() const{
	if (selected_.empty())
		return qdTriggerElementPtr();
	return selected_.back();
}

void SelectionManager::select(qdTriggerElementPtr const element, bool select){
	if (select)
	{
		if (!isSelected(element))
			selected_.push_back(element);
	}
	else
	{
		selected_.erase(
			std::remove(selected_.begin(), 
						selected_.end(),
						element), 
			selected_.end());
	}
	element->select(select);
}

SelectionManager::const_iterator SelectionManager::begin() const{
	return selected_.begin();
}
SelectionManager::const_iterator SelectionManager::end() const{
	return selected_.end();
}
