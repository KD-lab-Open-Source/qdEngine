#include "stdafx.h"
#include "resource.h"
#include ".\fontfoldertreenode.h"

#include "FontsPropPage.h"

#include "ptree_consts.h"

FontFolderTreeNode::FontFolderTreeNode(void)
: FolderTreeNode(QD_NAMED_OBJECT_FONT_INFO)
{
}

FontFolderTreeNode::~FontFolderTreeNode(void)
{
}

bool FontFolderTreeNode::load(ITreeNodeFactory* factory, TreeLogic& treeLogic,
							  ITreeNode * parent, qdNamedObjectBase const* owner,
							  ITreeNode* insertAfter)
{
	CString str;
	str.LoadString(IDS_FONTS_CATALOG);
	CTreeListItem* const item = insertToTree(treeLogic,
											 str,
											 parent,
											 II_FONTS_CATALOG,
											 getFolderId(),
											 insertAfter);
	return (item != NULL);
}

//! Возвращает идентификатор ресурса меню для узла
DWORD FontFolderTreeNode::getMenuResId() const{
	return IDR_FONTS_NODE;
}

bool FontFolderTreeNode::showProperties(TreeLogic& logic, int activePage)
{
	CPropertyPage* ptrPages[] = {0, 0};
	std::auto_ptr<FontsPropPage> ptrPage(new FontsPropPage);
	ptrPage->set_data(logic.getGameDispatcher());
	ptrPages[0] = ptrPage.get();
	return showPropDlg(ptrPages, activePage, IDS_PPCAP_FONTS_PROPERTIES);
}
