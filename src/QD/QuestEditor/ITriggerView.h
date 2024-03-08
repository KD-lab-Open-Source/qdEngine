#ifndef __ITRIGGER_VIEW_H
#define __ITRIGGER_VIEW_H

#pragma once

enum TVIEW_STATE{ 
//	TVS_NONE,
	TVS_NORMAL,
	TVS_SHOW_ALL,
};

#include <boost/intrusive_ptr.hpp>
class qdTriggerChain;
class qdTriggerElement;
typedef boost::intrusive_ptr<qdTriggerElement> qdTriggerElementPtr;
typedef boost::intrusive_ptr<qdTriggerElement const> qdTriggerElementConstPtr;

#include "IProjectSettings.h"

#include "te_filter_base.h"
const qdTriggerChain* const TE_SHOW_ALL  = 
							reinterpret_cast<const qdTriggerChain * const>(1);

qdTriggerChain* const ALL_SHOWED = reinterpret_cast<qdTriggerChain*>(-1);

interface ITriggerView{
	virtual BOOL AddState(CPoint &offset, 
					class qdGameObjectState*const* ptrState, int count) = 0;
	virtual BOOL SetViewState(TVIEW_STATE estate) = 0;
	virtual BOOL ShowTriggers(class qdTriggerChain** ptrTCs, int count) = 0;
	virtual BOOL Redraw() = 0;
	enum eSelectMode{SM_EXCLUSIVE, SM_INTERSECT, SM_UNION};
	enum eSelectAction{SA_DESELECT, SA_SELECT};
	virtual BOOL SelectByFilter(const te_filter_base& f, 
								eSelectAction sa, eSelectMode mode) = 0;
	//триггер, который показывается в текущий момент
	virtual qdTriggerChain* ShowedTrigger() = 0;
	//! возвращает имя последнего редактируемого триггера
	virtual CString const GetLastEditedTrigger() = 0;
	virtual BOOL ShowLTOfSelected(const te_filter_base& f) = 0;
	virtual BOOL MergeTriggers(qdTriggerChain const* from,qdTriggerChain* to) = 0;
	virtual void EnsureVisible(qdTriggerElementPtr const*eles, int count) = 0;
	virtual void SaveIniSettings(IProjectSettings* pps) = 0;
	virtual void ReadIniSettings(IProjectSettings* pps) = 0;
	virtual void OnEndIniReading() = 0;

	enum eVCAction{VCA_HIDE, VCA_SHOW};
	//! Вызывается до изменения
	virtual bool OnVisibilityChanging(eVCAction action) = 0;
	//! Вызывается после изменения
	virtual void OnVisibilityChange(eVCAction action) = 0;
	//! Пермещает окно так, чтобы элемент объекта был виден
	virtual void ShowTrigger(qdNamedObject const* object) = 0;

	virtual BOOL IsVisible() = 0;
	virtual void activate() = 0;
	virtual void deactivate() = 0;
	virtual void clearUndobuffer() = 0;
	virtual void setGameDisp(qdGameDispatcher* ptrDisp) = 0;
	virtual void reset() = 0;
};
#endif//#ifndef __ITRIGGER_VIEW_H