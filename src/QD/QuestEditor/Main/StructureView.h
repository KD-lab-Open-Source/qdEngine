#pragma once

#include "Main/QETreeListCtrl.h"
class Project;
// StructureView view

class StructureView : public CView
{
	DECLARE_DYNCREATE(StructureView)

protected:
	StructureView();           // protected constructor used by dynamic creation
	virtual ~StructureView();

public:
	void setProject(Project* project);
	Project* getProject() const;
	QETreeListCtrl& getTreeListCtrl();
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool LoadStateBitmaps(CImageList* pil);

	DECLARE_MESSAGE_MAP();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	LRESULT OnMenuCommand(WPARAM , LPARAM);
private:
	QETreeListCtrl treeList_;
	CImageList nodeImages_;
	CImageList stateImages_;
};


