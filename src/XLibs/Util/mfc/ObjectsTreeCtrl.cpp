#include "StdAfx.h"
#include "ObjectsTreeCtrl.h"
#include "kdw/Win32Proxy.h"

IMPLEMENT_DYNAMIC(CObjectsTreeCtrl, CWnd)

BEGIN_MESSAGE_MAP(CObjectsTreeCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CObjectsTreeCtrl::CObjectsTreeCtrl(kdw::ObjectsTree* tree)
: proxy_(0)
, tree_(tree)
{
	WNDCLASS wndclass;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if(!::GetClassInfo (hInst, className(), &wndclass)){
        wndclass.style			= CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW|WS_TABSTOP;
		wndclass.lpfnWndProc	= ::DefWindowProc;
		wndclass.cbClsExtra		= 0;
		wndclass.cbWndExtra		= 0;
		wndclass.hInstance		= hInst;
		wndclass.hIcon			= NULL;
		wndclass.hCursor		= AfxGetApp ()->LoadStandardCursor (IDC_ARROW);
		wndclass.hbrBackground	= reinterpret_cast<HBRUSH> (COLOR_WINDOW);
		wndclass.lpszMenuName	= NULL;
		wndclass.lpszClassName	= className();

		if(!AfxRegisterClass(&wndclass))
			AfxThrowResourceException();
	}
}

int CObjectsTreeCtrl::initControl(DWORD dwStyle, CWnd* parent)
{
    if(::IsWindow(GetSafeHwnd())){
    }
	else{
		CRect rt(0, 0, 100, 100);
        if(!Create (WS_VISIBLE | WS_CHILD, rt, parent, 0))
            return -1;
    }

	xassert(::IsWindow(GetSafeHwnd()));
	proxy_ = new kdw::Win32Proxy(GetSafeHwnd());
	if(!tree_)
		tree_ = new kdw::ObjectsTree();
	proxy_->add(tree_);
	proxy_->showAll();
    return 0;
}

BOOL CObjectsTreeCtrl::Create(DWORD style, const CRect& rect, CWnd* parentWnd, UINT id)
{
	DWORD dwExStyle = 0;

    if(!CWnd::Create(className(), "unnamed", style | WS_TABSTOP, rect, parentWnd, id, 0))
		return FALSE;

	return TRUE;
}

int CObjectsTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CObjectsTreeCtrl::OnSize(UINT type, int cx, int cy)
{
	if(proxy_)
		proxy_->_setPosition(Recti(0, 0, cx, cy));
}

kdw::TreeObject* CObjectsTreeCtrl::addObject(kdw::TreeObject* object,
											 kdw::TreeObject* rootObject,
											 kdw::TreeObject* afterObject)
{
	if(!rootObject)
		rootObject = root();
	return rootObject->add(object, afterObject);
}


void CObjectsTreeCtrl::clear()
{
	xassert(tree_);
	tree_->model()->clear();
}

kdw::TreeModel::Selection CObjectsTreeCtrl::selection()
{
	return tree_->model()->selection();
}

kdw::TreeObject* CObjectsTreeCtrl::selected()
{
	kdw::TreeModel::Selection sel(tree()->model()->selection());
	if(!sel.empty())
		return safe_cast<kdw::TreeObject*>(tree()->model()->rowFromPath(sel.front()));
	else
		return 0;
}


kdw::TreeObject* CObjectsTreeCtrl::root()
{
	return safe_cast<kdw::TreeObject*>(tree_->model()->root());
}

const kdw::TreeObject* CObjectsTreeCtrl::root() const
{
	return safe_cast<const kdw::TreeObject*>(tree_->model()->root());
}
