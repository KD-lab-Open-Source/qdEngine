#include "stdafx.h"
#include "resource.h"
#include ".\InterfaceScreenTreeNode.h"

#include "InterfScreenProp.h"
#include "ISceneViewFacade.h"

#include "ptree_consts.h"
#include "Tree/LoadTreeAssist.h"

#include "qd_interface_screen.h"
#include "qd_interface_element.h"

#include "name_op.h"

#include <boost/rtl.hpp>

InterfaceScreenTreeNode::InterfaceScreenTreeNode(qdInterfaceObjectBase* object)
: InterfaceObjectTreeNode(object)
{
}

InterfaceScreenTreeNode::~InterfaceScreenTreeNode(void)
{
}

bool InterfaceScreenTreeNode::load(ITreeNodeFactory* factory, 
								  TreeLogic& treeLogic,
								  ITreeNode * parent, 
								  qdNamedObjectBase const* owner,
								  ITreeNode* insertAfter)
{
	CTreeListItem* const item = insertToTree(treeLogic,
											getObject()->name(),
											parent,
											II_INTERFACE_SCEREEN_NODE,
											reinterpret_cast<DWORD_PTR>(getObject()), 
											insertAfter);
	if (!item)
		return false;
//	treeLogic.showNodeCheck(this, true);
//	treeLogic.setNodeCheck(this, 
//		static_cast<qdInterfaceElement*>(getObject())->is_visible());
	loadContent(factory, treeLogic);
	return true;
}

bool InterfaceScreenTreeNode::loadContent(ITreeNodeFactory* factory, 
										  TreeLogic& treeLogic) 
{
	qdInterfaceScreen const* screen = static_cast<qdInterfaceScreen const*>(getObject());
	boost::rtl::rng::for_each(screen->element_list(),
		load_tree_assist::InsertInterfaceObject(factory, 
		treeLogic, this, NULL, TN_INSERT_SORT));
	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD InterfaceScreenTreeNode::getMenuResId() const{
	return IDR_INTERFACE_SCREEN_NODE;
}

bool InterfaceScreenTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<InterfScreenProp> ptrISPage(new InterfScreenProp);
	ptrISPage->set_data(static_cast<qdInterfaceScreen*>(getObject()));
	ptrPages[0] = ptrISPage.get();
	return showPropDlg(ptrPages, iActive, IDS_INTERF_SCREEN_PROPERTIES);
}

bool InterfaceScreenTreeNode::onStateChange(TreeLogic& logic, 
											DWORD oldState, 
											DWORD newState)
{
	if (!getObject())
		return false;
	if (!logic.isSceneViewActive())
		return false;
	if (newState&NS_FOCUSED)
	{
		logic.getSceneView()->SetViewState(VS_EDIT_INTERFACE);
		logic.getSceneView()->SetSubTools(SSTE_NONE);
		return !logic.selectInterfaceScreen(getObject()->name());
	}
	return false;
}

bool InterfaceScreenTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask))
		return false;
	qdInterfaceDispatcher* const pdisp = 
		static_cast<qdInterfaceDispatcher*>(getObject()->owner());
	qdInterfaceScreen* const pscr = static_cast<qdInterfaceScreen*>(getObject());

	if (pdisp->is_screen_active(pscr))
		pdisp->select_screen(NULL);
	if (pdisp->remove_screen(pscr))
	{
		ITreeNode* prevVisible = logic.getPrevVisibleNode(this);
		delete getObject();
//		setObject(NULL);
		logic.deleteNode(this);
		logic.setFocusedNode(prevVisible);
		return true;
	}
	return false;
}

bool InterfaceScreenTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName) const
{
	qdInterfaceDispatcher* const pd = qdInterfaceDispatcher::get_dispatcher();
	return uniq_name(newName, pd->screen_list());
}

//! Возвращает true, если удалось переимновать, false в противном случае
bool InterfaceScreenTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	qdInterfaceDispatcher *const pd = qdInterfaceDispatcher::get_dispatcher();
	return pd->rename_screen(static_cast<qdInterfaceScreen*>(getObject()), newName);
}
//! Можно ли сменить имя узлу
bool InterfaceScreenTreeNode::canRename(TreeLogic& logic)
{
	return true;
}
