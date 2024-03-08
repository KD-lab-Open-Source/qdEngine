#include "stdafx.h"
#include ".\counterobjecttreenode.h"
#include "resource.h"
#include "ptree_consts.h"

#include "CounterPropPage.h"
#include "name_op.h"
#include "qd_counter.h"

CounterObjectTreeNode::CounterObjectTreeNode(qdNamedObject* object)
: NamedObjectTreeNode(object)
{
}

CounterObjectTreeNode::~CounterObjectTreeNode(void)
{
}

bool CounterObjectTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic, 
								 ITreeNode *parent, qdNamedObjectBase const* owner,
								 ITreeNode* insertAfter)
{
	CTreeListItem *const item = insertToTree(treeLogic, 
										getObject()->name(), parent,
										II_COUNTER_NODE, 
										reinterpret_cast<DWORD_PTR>(getObject()), 
										insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD CounterObjectTreeNode::getMenuResId() const{
	return IDR_COUNTER_NODE;
}

bool CounterObjectTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<CounterPropPage> ptrPage(new CounterPropPage);
	ptrPage->set_data(logic.getGameDispatcher(), static_cast<qdCounter*>(getObject()));
	ptrPages[0] = ptrPage.get();
	return showPropDlg(ptrPages, iActive, IDS_PPCAP_COUNTER_PROPERTIES);
}

bool CounterObjectTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask, statDepends))
		return false;

	if (logic.getGameDispatcher()->remove_counter(static_cast<qdCounter*>(getObject())))
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


bool CounterObjectTreeNode::canRename(TreeLogic& logic)
{
	return true;
}

bool CounterObjectTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return gameDisp->rename_counter(static_cast<qdCounter*>(getObject()), newName);
}

bool CounterObjectTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return uniq_name(newName, gameDisp->counter_list());
}
