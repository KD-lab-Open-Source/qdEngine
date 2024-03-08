#include "stdafx.h"
#include "resource.h"
#include ".\interfacesavetreenode.h"
#include "ptree_consts.h"

#include "InterfSaveProp.h"

#include "qd_interface_save.h"

InterfaceSaveTreeNode::InterfaceSaveTreeNode(qdInterfaceObjectBase* object)
: InterfaceElementTreeNode(object)
{
}

InterfaceSaveTreeNode::~InterfaceSaveTreeNode(void)
{
}

bool InterfaceSaveTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
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
DWORD InterfaceSaveTreeNode::getMenuResId() const{
	return IDR_INTERFACE_ELE_NODE;
}

bool InterfaceSaveTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<InterfSaveProp> ptrISPage(new InterfSaveProp);
	ptrISPage->set_data(static_cast<qdInterfaceSave*>(getObject()));
	ptrPages[0] = ptrISPage.get();
	return showPropDlg(ptrPages, iActive, IDS_INTERF_SAVE_PROPERTIES);
}
