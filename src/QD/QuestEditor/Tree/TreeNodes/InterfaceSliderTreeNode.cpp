#include "stdafx.h"
#include "resource.h"
#include ".\interfaceslidertreenode.h"
#include "ptree_consts.h"

#include "InterfSldrStateProp.h"
#include "InterfSldrProp.h"

#include "qd_interface_slider.h"

InterfaceSliderTreeNode::InterfaceSliderTreeNode(qdInterfaceObjectBase* object)
: InterfaceElementTreeNode(object)
{
}

InterfaceSliderTreeNode::~InterfaceSliderTreeNode(void)
{
}

bool InterfaceSliderTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
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
DWORD InterfaceSliderTreeNode::getMenuResId() const{
	return IDR_INTERFACE_ELE_NODE;
}

bool InterfaceSliderTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<InterfSldrProp> ptrPosPage(new InterfSldrProp);
	ptrPosPage->set_data(static_cast<qdInterfaceSlider*>( getObject()));
	ptrPages[0] = ptrPosPage.get();

	std::auto_ptr<InterfSldrStateProp> 
		ptrSSp1(new InterfSldrStateProp(SC_SLIDER, IDS_CAP_SLIDER_FORGRND_PROP));
	ptrSSp1->set_data(static_cast<qdInterfaceSlider*>(getObject()));
	ptrPages[1] = ptrSSp1.get();

	std::auto_ptr<InterfSldrStateProp> 
		ptrSSp2(new InterfSldrStateProp(SC_BACK, IDS_CAP_SLIDER_BACKGRND_PROP));
	ptrSSp2->set_data(static_cast<qdInterfaceSlider*>(getObject()));
	ptrPages[2] = ptrSSp2.get();

	return showPropDlg(ptrPages, iActive, IDS_INTERF_SLIDER_PROPERTIES);
}
