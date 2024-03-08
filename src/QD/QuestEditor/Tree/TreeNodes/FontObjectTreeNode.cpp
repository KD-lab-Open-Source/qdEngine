#include "stdafx.h"
#include "resource.h"
#include ".\fontobjecttreenode.h"

#include "name_op.h"
#include "FontPropPage.h"

#include "ptree_consts.h"

FontObjectTreeNode::FontObjectTreeNode(qdNamedObject* object)
: NamedObjectTreeNode(object)
{
}

FontObjectTreeNode::~FontObjectTreeNode(void)
{
}

bool FontObjectTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
							  ITreeNode * parent, qdNamedObjectBase const* owner,
							  ITreeNode* insertAfter)
{
	CTreeListItem* const item = insertToTree(treeLogic,
							getObject()->name(), parent, 
							II_FONT_NODE,
							reinterpret_cast<DWORD_PTR>(getObject()), 
							insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD FontObjectTreeNode::getMenuResId() const{
	return IDR_FONT_NODE;
}

bool FontObjectTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0};
	std::auto_ptr<FontPropPage> ptrPage(new FontPropPage);
	ptrPage->set_data(logic.getGameDispatcher(), static_cast<qdFontInfo*>(getObject()));
	ptrPages[0] = ptrPage.get();
	return showPropDlg(ptrPages, iActive, IDS_PPCAP_FONT_PROPERTIES);
}

bool FontObjectTreeNode::deleteObject(TreeLogic& logic, bool ask, bool statDepends)
{
	if (!preDelete(logic, ask, statDepends))
		return false;
	if (logic.getGameDispatcher()->remove_font_info(
				static_cast<qdFontInfo*>(getObject())))
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

bool FontObjectTreeNode::canRename(TreeLogic& logic)
{
	return true;
}

bool FontObjectTreeNode::rename(TreeLogic& logic, LPCTSTR newName)
{
	if (!nameTest(logic, newName))
		return false;
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return gameDisp->rename_font_info(static_cast<qdFontInfo*>(getObject()), newName);
}

bool FontObjectTreeNode::nameTest(TreeLogic& logic, LPCTSTR newName)
{
	qdGameDispatcher* gameDisp = logic.getGameDispatcher();
	return uniq_name(newName, gameDisp->fonts_list());
}
