#include "stdafx.h"
#include "resource.h"
#include "ptree_consts.h"
#include ".\netfoldertreenode.h"

#include "baseplanepage.h"
#include "ISceneViewFacade.h"

#include "qd_named_object.h"
#include "qd_game_scene.h"


NetFolderTreeNode::NetFolderTreeNode()
: FolderTreeNode(QD_NAMED_OBJECT_GRID_ZONE)
{
}

NetFolderTreeNode::~NetFolderTreeNode(void)
{
}


bool NetFolderTreeNode::load(ITreeNodeFactory* factory, 
							 TreeLogic& treeLogic, ITreeNode * parent, 
							 qdNamedObjectBase const* owner,
							 ITreeNode* insertAfter) 
{
	QETreeListCtrl& tree = treeLogic.getTreeListCtrl();
	CString str;
	str.LoadString(IDS_NET_NODE_NAME);
	CTreeListItem* netItem = 
		insertToTree(treeLogic, str, parent, II_NET_NODE, getFolderId(), insertAfter);
	if(netItem)
	{
		treeLogic.expand(this, TreeLogic::EC_EXPAND);

		showCheckBox(treeLogic, true);
		setCheck(treeLogic, false);
		return true;
	}

	return false;
}

//! Возвращает идентификатор ресурса меню для узла
DWORD NetFolderTreeNode::getMenuResId() const{
	return IDR_NET_NODE;
}

bool NetFolderTreeNode::showProperties(TreeLogic& logic, int iActive)
{
	CPropertyPage* ptrPages[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::auto_ptr<BasePlanePage> ptrBPP(new BasePlanePage);
	ptrPages[0] = ptrBPP.get();
	ptrBPP->set_data(logic.getGameDispatcher(), 
		logic.getCurrentSceneNode()->getContent().gameObject);
	return showPropDlg(ptrPages, 0, IDS_PPCAP_NET_PROPERTIES);
}

bool NetFolderTreeNode::onCheckNode(TreeLogic& logic, bool bCheck)
{
	ITreeNode* scene = logic.isNodeUnderScene(this);
	ASSERT(scene);
	
	qdGameScene* ptrScene = static_cast<qdGameScene*>(scene->getContent().gameObject);
	if (bCheck) {
		if(this == logic.getFocusedNode())
			ptrScene->get_camera()->set_redraw_mode(QDCAM_GRID_ABOVE);
		else
			ptrScene->get_camera()->set_redraw_mode(QDCAM_GRID_ZBUFFER);
	}
	else
		ptrScene->get_camera()->set_redraw_mode(QDCAM_GRID_NONE);
	return (ptrScene == logic.getGameDispatcher()->get_active_scene());
}

bool NetFolderTreeNode::onStateChange(TreeLogic& logic, DWORD oldState, DWORD newState)
{
	if (!logic.isSceneViewActive())
		return false;
	if (newState&NS_FOCUSED) {
		logic.getSceneView()->SetViewState(VS_EDIT_NET);
		logic.getSceneView()->SetSubTools(SSTE_NONE);
		return true;
	}
	else
	{
		if (logic.getSceneView()->GetViewState() == VS_EDIT_NET)
		{
			if (logic.getSceneView()->GetSubTools() == SSTE_CREATE_ZONE)
			{
				logic.getSceneView()->OnAbortGridZoneEdit(false);
				return true;
			}
		}
	}
	return false;
}
