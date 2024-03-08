#include "stdafx.h"
#include ".\viewactivator.h"
#include "Project.h"

void ViewActivator::operator = (bool b){
	if (b != isSceneViewActive_) {
		if (!project_->onChangingViews(isSceneViewActive_, b)) 
			return;
	}

	if (b) 
		project_->showSceneView();
	else
		project_->showTEView();

	bool old = isSceneViewActive_;
	isSceneViewActive_ = b;

	if (old != b) 
		project_->onChangeViews(isSceneViewActive_, b);
}

bool ViewActivator::getSceneViewActive() const{
	return isSceneViewActive_;
}
