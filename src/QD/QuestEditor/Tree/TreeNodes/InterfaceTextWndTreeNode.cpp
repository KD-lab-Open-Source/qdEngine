#include "stdafx.h"
#include "resource.h"
#include ".\interfacetextwndtreenode.h"
#include "ptree_consts.h"

#include "InterfaceTextWindowPP.h"

#include "qd_interface_text_window.h"

InterfaceTextWndTreeNode::InterfaceTextWndTreeNode(qdInterfaceObjectBase* object)
: InterfaceElementTreeNode(object)
{
}

InterfaceTextWndTreeNode::~InterfaceTextWndTreeNode(void)
{
}

bool InterfaceTextWndTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
									ITreeNode * parent, qdNamedObjectBase const* owner,
									ITreeNode* insertAfter)
{
	CTreeListItem* const item = insertToTree(treeLogic,
											getObject()->name(),
											parent,
											II_INTERFACE_ELEMENT_NODE,
											reinterpret_cast<DWORD_PTR>(getObject()), 
											insertAfter);

	treeLogic.showNodeCheck(this, true);
	treeLogic.setNodeCheck(this, 
		static_cast<qdInterfaceElement*>(getObject())->is_visible());
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD InterfaceTextWndTreeNode::getMenuResId() const{
	return IDR_INTERFACE_TEXT_WINDOW_NODE;
}

bool InterfaceTextWndTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<InterfaceTextWindowPP> ptrTextWindow(new InterfaceTextWindowPP);
	ptrTextWindow->set_data(static_cast<qdInterfaceTextWindow*>(getObject()));
	ptrPages[0] = ptrTextWindow.get();
	return showPropDlg(ptrPages, iActive, IDS_INTERF_TEXT_WINDOW_PROPERTIES);
}
