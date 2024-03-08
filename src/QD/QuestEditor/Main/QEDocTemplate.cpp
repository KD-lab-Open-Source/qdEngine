#include "stdafx.h"
#include ".\qedoctemplate.h"
#include "Main/MainFrame.h"
#include "Main/Project.h"


IMPLEMENT_DYNAMIC(QEDocTemplate, CSingleDocTemplate);

QEDocTemplate::QEDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
			CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass):
 CSingleDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

QEDocTemplate::~QEDocTemplate(void)
{
}

CFrameWnd* QEDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
{
	MainFrame* frame = 
		static_cast<MainFrame*>(CSingleDocTemplate::CreateNewFrame(pDoc, pOther));

	Project* project = static_cast<Project*>(pDoc);
	project->setStructureView(frame->getStructureView());
	project->setSceneView(frame->getSceneView());
	project->setTriggerEditorView(frame->getTriggerEditorView());
	project->setScaleMngr(frame->getScaleMngr());
	project->setObjectList(frame->getObjectList());
	frame->ShowWindow(SW_MAXIMIZE);
	return frame;
}

void QEDocTemplate::SetDefaultTitle(CDocument* pDocument){
}
