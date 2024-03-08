#pragma once

#include "IRectChangeListener.h"

class SelectionManager;
class qdTriggerChain;
class TEWnd;

class FrameSelector : public IRectChangeListener
{
public:
	FrameSelector(TEWnd* window, 
					SelectionManager* selectionManager,
					qdTriggerChain* triggerChain);
	~FrameSelector(void);


	SelectionManager*	getSelectionManager();
	qdTriggerChain*		getTriggerChain();
	TEWnd*				getWindow();

protected:
	void setSelectionManager(SelectionManager* selectionManager);
	void setTriggerChain(qdTriggerChain* triggerChain);
	void setWindow(TEWnd* window);

	virtual void onRectChange(CustomRectTracker& rectTracker, CRect const& oldRect);
private:
	SelectionManager* selMngr_;
	qdTriggerChain*	triggerChain_;
	//! Окно, которое будем перерисовывать
	TEWnd* window_;
};
