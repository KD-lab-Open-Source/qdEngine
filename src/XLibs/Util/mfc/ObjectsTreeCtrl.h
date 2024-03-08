#ifndef __OBJECTS_TREE_CTRL_H_INCLUDED__
#define __OBJECTS_TREE_CTRL_H_INCLUDED__

#include "kdw/ObjectsTree.h"

class CObjectsTreeCtrl : public CWnd, public sigslot::has_slots {
	DECLARE_DYNAMIC(CObjectsTreeCtrl)
public:
    static const char* className() { return "VistaEngineObjectsTreeCtrl"; }
	
	CObjectsTreeCtrl(kdw::ObjectsTree* tree = 0);

    int initControl(DWORD dwStyle, CWnd* parent);

    void clear();
	kdw::TreeObject* root();
	const kdw::TreeObject* root() const;


	kdw::TreeObject* addObject(kdw::TreeObject* new_object,
							   kdw::TreeObject* _root = 0,
							   kdw::TreeObject* _after = 0);

	kdw::TreeModel::Selection selection();
	kdw::TreeObject* selected();

	kdw::ObjectsTree* tree(){ return tree_; }
	const kdw::ObjectsTree* tree() const{ return tree_; }

	BOOL Create(DWORD style, const CRect& rect, CWnd* parentWnd = 0, UINT id = 0);
	int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnSize(UINT type, int cx, int cy);

	DECLARE_MESSAGE_MAP();
private:
	typedef kdw::TreeObjects Objects;

	kdw::TreeObjects draggedObjects_;

	Objects objects_;
	kdw::Win32Proxy* proxy_;
	kdw::ObjectsTree* tree_;
	friend class kdw::TreeObject;
};

#endif
