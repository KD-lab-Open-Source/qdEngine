#include "stdafx.h"
#include "resource.h"
#include ".\interfacebuttontreenode.h"
#include "ptree_consts.h"

#include "InterfBtnProp.h"

#include "qd_interface_button.h"
#include "qd_interface_element_state.h"

InterfaceButtonTreeNode::InterfaceButtonTreeNode(qdInterfaceObjectBase* object)
: InterfaceElementTreeNode(object)
{
}

InterfaceButtonTreeNode::~InterfaceButtonTreeNode(void)
{
}

bool InterfaceButtonTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
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
	loadStates(factory, treeLogic);
	treeLogic.expand(this, TreeLogic::EC_EXPAND);
	return (item != NULL);
}

bool InterfaceButtonTreeNode::loadStates(ITreeNodeFactory* factory, 
										 TreeLogic& treeLogic) 
{
	qdInterfaceButton * btn = static_cast<qdInterfaceButton*>(getObject());
	int const count = btn->num_states();
	for(int i = 0; i < count; ++i)
	{
		std::auto_ptr<ITreeNode> p(factory->createInterfaceObjectNode(btn->get_state(i)));
		if (!p.get()||!p->load(factory, treeLogic, this, btn))
			return false;
		p.release();
	}
	return true;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD InterfaceButtonTreeNode::getMenuResId() const{
	return IDR_INTERFACE_BTN_NODE;
}

bool InterfaceButtonTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[10] = {0};
	std::auto_ptr<InterfBtnProp> ptrPosPage(new InterfBtnProp);
	ptrPosPage->set_data(qdInterfaceDispatcher::get_dispatcher(), 
		static_cast<qdInterfaceButton*>( getObject()));
	ptrPages[0] = ptrPosPage.get();
	return showPropDlg(ptrPages, iActive, IDS_INTERF_BTN_PROPERTIES);
}
