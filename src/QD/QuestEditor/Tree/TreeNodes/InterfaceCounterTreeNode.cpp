#include "stdafx.h"
#include "resource.h"
#include ".\interfacecountertreenode.h"
#include "ptree_consts.h"

#include "InterfCounterPP.h"

#include "qd_interface_counter.h"

InterfaceCounterTreeNode::InterfaceCounterTreeNode(qdInterfaceObjectBase* object)
: InterfaceElementTreeNode(object)
{
}

InterfaceCounterTreeNode::~InterfaceCounterTreeNode(void)
{
}

bool InterfaceCounterTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
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
DWORD InterfaceCounterTreeNode::getMenuResId() const{
	return IDR_INTERFACE_COUNTER_NODE;
}

bool InterfaceCounterTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<InterfCounterPP> ptrPosPage(new InterfCounterPP);
	ptrPosPage->set_data(static_cast<qdInterfaceCounter*>( getObject()));
	ptrPages[0] = ptrPosPage.get();
	return showPropDlg(ptrPages, iActive, IDS_INTERF_COUNTER_PROPERTIES);
}
