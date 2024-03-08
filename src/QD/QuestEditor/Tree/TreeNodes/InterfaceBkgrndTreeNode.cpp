#include "stdafx.h"
#include "resource.h"
#include ".\interfacebkgrndtreenode.h"
#include "ptree_consts.h"

#include "InterfBackPP.h"

#include "qd_interface_background.h"

InterfaceBkgrndTreeNode::InterfaceBkgrndTreeNode(qdInterfaceObjectBase* object)
: InterfaceElementTreeNode(object)
{
}

InterfaceBkgrndTreeNode::~InterfaceBkgrndTreeNode(void)
{
}

bool InterfaceBkgrndTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
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
DWORD InterfaceBkgrndTreeNode::getMenuResId() const{
	return IDR_INTERFACE_BACK_NODE;
}

bool InterfaceBkgrndTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<InterfBackPP> ptrPosPage(new InterfBackPP);
	ptrPosPage->set_data(static_cast<qdInterfaceBackground*>( getObject()));
	ptrPages[0] = ptrPosPage.get();
	return showPropDlg(ptrPages, iActive, IDS_INTERF_BACK_PROPERTIES);
}
