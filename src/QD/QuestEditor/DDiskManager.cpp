// DDiskManager.cpp : implementation file
//

#include "stdafx.h"
#include "QuestEditor.h"
#include "DDiskManager.h"
#include "qd_game_dispatcher.h"
#include "qd_game_scene.h"
#include "qd_video.h"
#include <algorithm>
#include "ProgDlg.h"

#include <boost/utility.hpp>
#include <boost/rtl.hpp>

#include ".\ddiskmanager.h"

int const SCENE_IMAGE = 0;
int const VIDEO_IMAGE = 1;
int const CD_DISK_IMAGE = 2;
int const SCENE_NOT_IN_CD_IMAGE = 3;
int const VIDEO_NOT_IN_CD_IMAGE = 4;
int const MAX_CDS_COUNT = 30;

namespace{
	/// Вычисляет где находится кнопка Cancel
	CRect getCancelRect(CRect const& rcWnd, CRect const& rcBtn, 
		int bottomOffset, int rightOffset)
	{
		CRect res;
		res.left = rcWnd.right - rcBtn.Width() - rightOffset;
		res.right = rcWnd.right - rightOffset;
		res.top = rcWnd.bottom - rcBtn.Height() - bottomOffset;
		res.bottom = rcWnd.bottom - bottomOffset;
		return res;
	}

	/// Вычисляет где находится кнопка Ok
	CRect getOkRect(CRect const& rcWnd, CRect const& rcBtn, 
		int bottomOffset, int rightOffset)
	{
		CRect res;
		res.left = rcWnd.right - 2*rcBtn.Width() - rightOffset;
		res.right = rcWnd.right - rcBtn.Width() - rightOffset;
		res.top = rcWnd.bottom - rcBtn.Height() - bottomOffset;
		res.bottom = rcWnd.bottom - bottomOffset;
		return res;
	}

	/// Вычисляет где находится дерево проекта
	CRect getTreeRect(CRect const&rcWnd, int leftOffset, 
		int rightOffset, int bottomOffset, int topOffset)
	{
		CRect res(rcWnd);
		res.left += leftOffset;
		res.right -= rightOffset;
		res.top += topOffset;
		res.bottom -= bottomOffset;

		return res;
	}
}

namespace std
{
	/**
		Функция сравнения для объектов
	 */
	template<>
	struct less<qdNamedObject *> : public binary_function<qdNamedObject *, 
														qdNamedObject *, bool>
	{
		bool operator()(qdNamedObject const*lhs, qdNamedObject const*rhs) const{
			return (stricmp(lhs->name(), rhs->name())<0);
		}
	};
}

/**
	Функция вызывающая диалог сортировки по дискам
 */
bool showCDDiskManager(qdGameDispatcher* pdispatcher)
{
	CDDiskManager dlg;
	dlg.setDispatcher(pdispatcher);
	return (dlg.DoModal() == IDOK);
}

/// Возвращает размер в мегабайтах
float getSize(qdFileOwner const* pfo)
{
	return (pfo->files_size()/(1024.f*1024.f));
}

/// Возвращает размер в мегабайтах
float getSize(qdNamedObject const* pno)
{
	return getSize(dynamic_cast<qdFileOwner const*>(pno));
}


/**
	Callback для отображения прогресса сканирования файлов
 */
void scaneFilesProgressCallback(int percents, void * data)
{
	CProgressDlg* dlg = (CProgressDlg*)data;
	dlg->SetPos(percents);
}

namespace{
	/// По объекту возвращает номер изображения, которое будет у объекта в дереве
	int getImage(qdNamedObject const* pobj, bool onCd = false)
	{
		switch(pobj->named_object_type()) {
		case QD_NAMED_OBJECT_SCENE:
			{
				if (onCd) 
					return SCENE_IMAGE;
				return SCENE_NOT_IN_CD_IMAGE;
			}
		case QD_NAMED_OBJECT_VIDEO:
			if (onCd) 
				return VIDEO_IMAGE;
			return VIDEO_NOT_IN_CD_IMAGE;
		}
		ASSERT(0);
		return -1;
	}
}

/// Распределяет объекты под дискам
/**
	Обрабатывает объект. Кладет его в нужные списки в CDDiskManager::diskIndex_
	если объект не принадлежит ни одному диску, то добавляется на первый
 */
class CDDiskManager::SortToDisk
{
public:
	SortToDisk(int cdCount,
		std::map<int, std::vector<qdNamedObject*> > & index)
		:diskIndex_(index), cdCount_(cdCount){}
	void operator()(qdNamedObject * pobj){
		putToIndex(pobj);
	}
protected:
	bool putToIndex(qdNamedObject * pobj)
	{
		qdFileOwner* pfo = dynamic_cast<qdFileOwner*>(pobj);
		bool bOnCD = false;
		for(int i = 0; i < cdCount_; ++i)
		{
			if (pfo->is_on_CD(i)) 
			{
				diskIndex_[i].push_back(pobj);
				bOnCD = true;
			}
		}
		if (!bOnCD)
		{
			pfo->add_to_CD(0);
			diskIndex_[0].push_back(pobj);
			bOnCD = true;
		}
		return bOnCD;
	}
private:
	std::map<int, std::vector<qdNamedObject*> > &diskIndex_;
	int cdCount_;
};

// CDDiskManager dialog

IMPLEMENT_DYNAMIC(CDDiskManager, CDialog)
CDDiskManager::CDDiskManager(CWnd* pParent /*=NULL*/)
	: CDialog(CDDiskManager::IDD, pParent)
	, disksCount_(0)
{

}

int CDDiskManager::getDisksCount() const{
	return disksCount_;
}

void CDDiskManager::setDisksCount(int val){
	disksCount_ = val;
}

CDDiskManager::~CDDiskManager()
{
}

void CDDiskManager::setDispatcher(qdGameDispatcher* pdispatcher)
{
	gameDispatcher_ = pdispatcher;
}
void CDDiskManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM, tree_);
}


BEGIN_MESSAGE_MAP(CDDiskManager, CDialog)
	ON_COMMAND(ID_CD_DISK_MNGR_ADDCD, OnCdDiskMngrAddcd)
	ON_COMMAND(ID_CD_DISK_MNGR_REMOVE, OnCdDiskMngrRemove)
	ON_COMMAND(ID_CD_DISK_MNGR_REMOVECD, OnCdDiskMngrRemovecd)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CDDiskManager message handlers

BOOL CDDiskManager::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWaitCursor wait;
	CProgressDlg dlg;
	dlg.Create();
	CString str;
	str.LoadString(IDS_SCANE_FILES_PROGRESS_CAPTION);
	dlg.SetStatus(str);
	gameDispatcher_->scan_files(scaneFilesProgressCallback, (void*)&dlg);
	makeImageList();
	initTree();

	detectCDsCount();
	sortObjects();
	loadCDsTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CDDiskManager::initTree()
{
	tree_.SetStyle
		( 0
		| TLC_TREELIST
		| TLC_DOUBLECOLOR							// double color background

		| TLC_MULTIPLESELECT						// single or multiple select
		| TLC_SHOWSELACTIVE							// show active column of selected item
		| TLC_SHOWSELALWAYS							// show selected item always
		| TLC_SHOWSELFULLROWS						// show selected item in fullrow mode
		| TLC_TREELINE								// show tree line
		| TLC_ROOTLINE								// show root line
		| TLC_BUTTON								// show expand/collapse button [+]

		| TLC_HEADER								// show header
		| TLC_HGRID									// show horizonal grid lines
		| TLC_TGRID									// show tree horizonal grid lines ( when HGRID & VGRID )
		| TLC_HGRID_EXT								// show extention horizonal grid lines
		| TLC_VGRID_EXT								// show extention vertical grid lines
		| TLC_HGRID_FULL							// show full horizonal grid lines
		| TLC_READONLY								// read only

		| TLC_IMAGE									// show image
		| TLC_HOTTRACK								// show hover text 

		| TLC_DROP									// drop support
		| TLC_DRAG									// drop support
		| TLC_DROPTHIS								// drop self support

		);
	tree_.ModifyStyleEx(0, WS_EX_STATICEDGE);
	tree_.SetImageList(&imageList_);
	CRect rc;
	tree_.GetClientRect(rc);
	CString str;
	str.LoadString(IDS_DISK_CONTENT_COLUMN_CAPTION);
	tree_.InsertColumn(str, TLF_DEFAULT_LEFT, rc.Width()-10);

	tree_.setItemDragdropNotifyListener(this);
	tree_.setGeneralNotifyListener(this);
	tree_.setItemChangeNotifyListener(this);

	return true;
}

template<class Container >
bool CDDiskManager::sortObjects(Container const& container)
{
	std::vector<qdNamedObject*> forSort;
	forSort.reserve(container.size());
	std::copy(container.begin(), container.end(), std::back_inserter(forSort));
	std::sort(forSort.begin(), forSort.end());
	std::for_each(forSort.begin(), forSort.end(), 
		SortToDisk(getDisksCount(), diskIndex_));
	return true;
}
bool CDDiskManager::sortObjects()
{
	sortObjects(gameDispatcher_->scene_list());
	sortObjects(gameDispatcher_->video_list());
	return true;
}

bool CDDiskManager::makeImageList()
{
	imageList_.Create(16, 16, ILC_MASK|ILC_COLOR32, 55, 4);

	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_SCENE_NODE);
	imageList_.Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_VIDEOS_NODE);
	imageList_.Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_CD);
	imageList_.Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_SCENE_NOT_IN_CD);
	imageList_.Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_VIDEO_NOT_IN_CD);
	imageList_.Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	return true;
}

bool CDDiskManager::canDrop(CTreeListItem* target, CTreeListItem* pItem, 
							CTreeListCtrl* sourceCtrl) 
{
	if (target)
		return !isInChildList(target, pItem->GetData());

	return false;
}

bool CDDiskManager::isCdNode(CTreeListItem* pitem){
	return (pitem->GetData() < MAX_CDS_COUNT + 1);
}

void CDDiskManager::OnCdDiskMngrAddcd()
{
	int const iDiskNum = getDisksCount();
	CTreeListItem* pCD = addCD(iDiskNum);
	if (pCD) 
	{
		gameDispatcher_->add_to_CD(iDiskNum);
		setDisksCount(iDiskNum + 1);
		tree_.Expand(pCD, TLE_EXPAND);
	}
}

void CDDiskManager::OnCdDiskMngrRemove()
{
	POSITION pos = tree_.GetFirstSelectedItemPosition();
	std::list<CTreeListItem*> lst;
	while (pos) {
		CTreeListItem* const pitem = tree_.GetNextSelectedItem(pos);
		if (pitem)
			lst.push_back(pitem);
	}
	boost::rtl::crange<std::list<CTreeListItem*> const> r(lst);
	for(; r; ++r)
	{
		CTreeListItem* const pitem = *r;
		CTreeListItem* pParent = tree_.GetParentItem(pitem);
		if (isObjectOnDisks(pitem->GetData(), pParent))
		{
			removeCDContent(pParent, pitem);
			tree_.DeleteItem(pitem);
		}
	}
}

void CDDiskManager::OnCdDiskMngrRemovecd()
{
	while (CTreeListItem* pitem = tree_.GetSelectedItem()) {
		if (getDisksCount() == 1)
			break;
		copyNoOwnerContentToZeroDisk(pitem);
		int const iDiskNum = getCDNumber(pitem);
		compressDiskIndex(iDiskNum);
		compressDiskSizes(iDiskNum);
		tree_.DeleteItem(pitem);
		setDisksCount(getDisksCount()-1);
	}
	updateDisks();
}

CTreeListItem* CDDiskManager::getDiskItem(int number) const
{
	CTreeListItem * disk = tree_.GetChildItem(TLI_ROOT);
	while (disk) {
		if (getCDNumber(disk) == number)
			return disk;
		disk = tree_.GetNextSiblingItem(disk);
	}
	return NULL;
}

void CDDiskManager::copyNoOwnerContentToZeroDisk(CTreeListItem const* diskItem)
{
	int const iDiskNum = getCDNumber(diskItem);
	CTreeListItem* targetDisk = iDiskNum ? getDiskItem(0):getDiskItem(1);
	CTreeListItem* childItem = tree_.GetChildItem(diskItem);
	while (childItem)
	{
		if (!isObjectOnDisks(childItem->GetData(), diskItem))
			addCDContent(targetDisk, childItem);
		childItem = tree_.GetNextSiblingItem(childItem);
	}
}

void CDDiskManager::insertObjectToTree(CTreeListItem* pParent, qdNamedObject const* pno)
{
	CString str;
	str.Format(IDS_DISK_CONTENT_FORMAT, pno->name(), ::getSize(pno));
	CTreeListItem* pItem = tree_.InsertItem(str, pParent);
	if (pItem)
	{
		tree_.SetItemData(pItem, reinterpret_cast<DWORD_PTR>(pno));
		int const iImage = getImage(pno, true);
		tree_.SetItemImage(pItem, iImage, iImage, iImage, iImage);
	}
	if (!tree_.GetItemState(pParent, TLIS_EXPANDED))
		tree_.Expand(pParent, TLE_EXPAND);
}

void CDDiskManager::removeFromDiskIndex(int iDiskNumber, qdNamedObject* pno)
{
	std::vector<qdNamedObject*>&v = diskIndex_[iDiskNumber];
	std::vector<qdNamedObject*>::iterator res = std::find(v.begin(), v.end(), pno);
	if (res != v.end()) 
		v.erase(res);
	else
		ASSERT(0);
}

void CDDiskManager::addToDiskIndex(int iDiskNumber, qdNamedObject* pno)
{
	std::vector<qdNamedObject*>&v = diskIndex_[iDiskNumber];
	v.push_back(pno);
}

void CDDiskManager::removeCDContent(CTreeListItem* pParent, CTreeListItem const* pitem)
{
	int const iDiskNumber = getCDNumber(pParent);
	qdNamedObject * pobj = reinterpret_cast<qdNamedObject *>(pitem->GetData());
	qdFileOwner * pfo = dynamic_cast<qdFileOwner*>(pobj);

//	pfo->remove_from_CD(iDiskNumber);
	diskSizes_[iDiskNumber] -= pfo->files_size();
	removeFromDiskIndex(iDiskNumber, pobj);
	updateSizeInfo(pParent);
}

void CDDiskManager::addCDContent(CTreeListItem* pParent, CTreeListItem const* pSource)
{
	int diskNum = getCDNumber(pParent);
	if (!isInChildList(pParent, pSource->GetData())) 
	{
		qdNamedObject * pobj = reinterpret_cast<qdNamedObject *>(pSource->GetData());
		qdFileOwner * pfo = dynamic_cast<qdFileOwner*>(pobj);
		diskSizes_[diskNum] += pfo->files_size();
//		pfo->add_to_CD(diskNum);
		addToDiskIndex(diskNum, pobj);
		updateSizeInfo(pParent);
		insertObjectToTree(pParent, pobj);
	}
}

void CDDiskManager::removeItemsFromCD()
{
	while (	POSITION pos = tree_.GetFirstSelectedItemPosition())
	{
		CTreeListItem* subjectToRemove = tree_.GetNextSelectedItem(pos);
		removeCDContent(tree_.GetParentItem(subjectToRemove), subjectToRemove);
		tree_.DeleteItem(subjectToRemove);
	}
}
void CDDiskManager::addItemsToCD(CTreeListItem* pParent, CTreeListCtrl* pSource)
{
	POSITION pos = pSource->GetFirstSelectedItemPosition();
	while (	CTreeListItem* subjectToAdd = pSource->GetNextSelectedItem(pos))
	{
		addCDContent(pParent, subjectToAdd);
	}
}

bool CDDiskManager::isObjectOnDisks(DWORD_PTR obj,
					 CTreeListItem const* diskExclude) const
{
	CTreeListItem *disk = tree_.GetChildItem(TLI_ROOT);
	while (disk) {
		if (disk != diskExclude)
			if (isInChildList(disk, obj))
				return true;
		disk = tree_.GetNextSiblingItem(disk);
	}
	return false;
}

bool CDDiskManager::isInChildList(CTreeListItem const* pParent, DWORD_PTR dwData) const
{
	CTreeListItem* pChild = tree_.GetChildItem(pParent);
	while (pChild) {
		if (dwData == pChild->GetData()) 
			return true;
		pChild = tree_.GetNextSiblingItem(pChild);
	}
	return false;
}

void CDDiskManager::updateSizeInfo(CTreeListItem* pCDItem)
{
	int iDiskNum = getCDNumber(pCDItem);
	CString str;
	str.Format(IDS_CD_DISK_NAME_FORMAT, iDiskNum+1, getSize(iDiskNum));
	tree_.SetItemText(pCDItem, 0, str);
}

int CDDiskManager::getCDNumber(CTreeListItem const* pCDItem) const{
	return static_cast<int>(pCDItem->GetData());
}

void CDDiskManager::detectCDsCount()
{
	for(int i = 0; i < MAX_CDS_COUNT; ++i)
	{
		if (gameDispatcher_->is_on_CD(i)) 
			disksCount_++;
		else
			break;
	}
}

void CDDiskManager::loadCDsTree()
{
	for(int i = 0; i < disksCount_; ++i)
	{
		CTreeListItem* pCD = addCD(i);
		if (pCD)
			loadCDContent(pCD);
	}
}

void CDDiskManager::loadCDContent(CTreeListItem* pParent)
{
	int const iDiskNumber = getCDNumber(pParent);
	std::vector<qdNamedObject*> const& v = diskIndex_[iDiskNumber];
	if (v.empty())
		return;
	std::vector<qdNamedObject*>::const_iterator i = v.begin(), e = v.end();
	for(; i!=e; ++i)
	{
		insertObjectToTree(pParent, *i);
		diskSizes_[iDiskNumber] += dynamic_cast<qdFileOwner*>(*i)->files_size();
	}
	updateSizeInfo(pParent);
	tree_.Expand(pParent, TLE_EXPAND);
}

CTreeListItem* CDDiskManager::addCD(int iDiskNum)
{
	CString str;

	str.Format(IDS_CD_DISK_NAME_FORMAT, iDiskNum + 1, 
		::getSize(static_cast<qdFileOwner const*>(gameDispatcher_)));
	CTreeListItem* pItem = tree_.InsertItem(str);
	if (pItem)
	{
		tree_.SetItemData(pItem, iDiskNum);
		tree_.SetItemImage(pItem, CD_DISK_IMAGE);
		diskSizes_[iDiskNum] += gameDispatcher_->files_size();
	}
	return pItem;
}

void CDDiskManager::compressDiskIndex(int iDiskForRemove)
{
	for(int i = iDiskForRemove; i < getDisksCount()-1; ++i)
	{
		std::swap(diskIndex_[i], diskIndex_[i+1]);
	}
	diskIndex_.erase(getDisksCount()-1);
}

void CDDiskManager::compressDiskSizes(int iDiskForRemove)
{
	for(int i = iDiskForRemove; i < getDisksCount()-1; ++i)
	{
		diskSizes_[i] = diskSizes_[i+1];
	}
	diskSizes_.erase(getDisksCount()-1);
}

void CDDiskManager::updateDisks()
{
	CTreeListItem* pItem = tree_.GetChildItem(TLI_ROOT);
	int iNum = 0;
	while (pItem) {
		tree_.SetItemData(pItem, iNum);
		updateSizeInfo(pItem);
		++iNum;
		pItem = tree_.GetNextSiblingItem(pItem);
	}
	ASSERT(iNum == getDisksCount());
}

float CDDiskManager::getSize(int iDiskNum)
{
	return (diskSizes_[iDiskNum]/(1024.f*1024.f));
}

void CDDiskManager::saveDiskInfoToProject()
{
	CTreeListItem* pItem = tree_.GetChildItem(TLI_ROOT);
	while (pItem) {
		int const iDiskNum = getCDNumber(pItem);
		gameDispatcher_->add_to_CD(iDiskNum);
		diskIndexEntryToProject(iDiskNum, diskIndex_[iDiskNum]);
		pItem = tree_.GetNextSiblingItem(pItem);
	}
}

void CDDiskManager::diskIndexEntryToProject(int iDiskNum,
											std::vector<qdNamedObject*> const& v)
{
	std::vector<qdNamedObject*>::const_iterator i = v.begin(), e = v.end();
	for(; i != e; ++i)
	{
		dynamic_cast<qdFileOwner*>(*i)->add_to_CD(iDiskNum);
	}
}

void CDDiskManager::OnCancel()
{
	__super::OnCancel();
}

void CDDiskManager::OnOK()
{
	clearProject();
	saveDiskInfoToProject();

	__super::OnOK();
}

void CDDiskManager::clearProject()
{
	gameDispatcher_->clear_cd_info();
	qdGameSceneList const& lst = gameDispatcher_->scene_list();
	std::for_each(lst.begin(), lst.end(), std::mem_fun(qdFileOwner::clear_cd_info));
	qdVideoList const& vl = gameDispatcher_->video_list();
	std::for_each(vl.begin(), vl.end(), std::mem_fun(qdFileOwner::clear_cd_info));
}

void CDDiskManager::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	adjustControlsLayout();
}

void CDDiskManager::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 500;
	lpMMI->ptMinTrackSize.y = 350;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CDDiskManager::adjustControlsLayout()
{
	int const BOTTOM_OFFSET = 7;
	int const LEFT_OFFSET = 7;
	int const RIGHT_OFFSET = 7;
	int const TOP_OFFSET = 7;
	int const HORZ_CONTROL_DIST = 2;
	int const VERT_CONTROL_DIST = 2;

	CRect rcClient;
	int iTreeBottomOffset = BOTTOM_OFFSET;
	GetClientRect(rcClient);

	CWnd* pwnd = GetDlgItem(IDCANCEL);
	if (pwnd)
	{
		CRect rcBtn;
		pwnd->GetClientRect(&rcBtn);
		rcBtn = getCancelRect(rcClient, rcBtn, BOTTOM_OFFSET, RIGHT_OFFSET);
		pwnd->MoveWindow(&rcBtn);
		iTreeBottomOffset += rcBtn.Height()+VERT_CONTROL_DIST;
	}
	if (pwnd = GetDlgItem(IDOK))
	{
		CRect rcBtn;
		pwnd->GetClientRect(&rcBtn);
		rcBtn = getOkRect(rcClient, rcBtn, BOTTOM_OFFSET, 
			RIGHT_OFFSET + HORZ_CONTROL_DIST);
		pwnd->MoveWindow(&rcBtn);
		if (iTreeBottomOffset == BOTTOM_OFFSET) 
			iTreeBottomOffset += rcBtn.Height()+VERT_CONTROL_DIST;
		pwnd->Invalidate();
	}
	if (tree_.m_hWnd) 
	{
		CRect rc(getTreeRect(rcClient,LEFT_OFFSET, 
			RIGHT_OFFSET, iTreeBottomOffset, TOP_OFFSET));
		tree_.MoveWindow(rc);
	}
}

//! Начинаем тянуть выделенные элементы левой кнопкой мыши
bool CDDiskManager::onBeginDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd)
{
	if (tree_.GetSelectedItem())
		return !isCdNode(tree_.GetSelectedItem());
	return false;
}
//! Уведомляет о том, что тянем курсор над окном
bool CDDiskManager::onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd)
{
	CTreeListItem *dropTarget = ptldd->hoverItem;
	if (!isCdNode(dropTarget))
		dropTarget = tree_.GetParentItem(dropTarget);
	if (canDrop(dropTarget, source.GetSelectedItem(), ptldd->pSource )) 
	{
		tree_.SetTargetItem(dropTarget);
		if (GetAsyncKeyState(VK_CONTROL)>>15 != 0)
			ptldd->hCursor = AfxGetApp()->LoadCursor(IDC_POINTER_COPY);
		else
			ptldd->hCursor = AfxGetApp()->LoadCursor(IDC_POINTER);
	}
	else
	{
		ptldd->hCursor = AfxGetApp()->LoadCursor(IDC_NODROP);
		tree_.SetTargetItem(NULL);
		return false;
	}
	return true;
}

//! Уведомляет о том, что отжали кнопку мыши
bool CDDiskManager::onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd)
{
	ASSERT(ptldd->pSource->GetSelectedCount() != 0);
	if (tree_.GetDropTargetItem()) 
		addItemsToCD(tree_.GetDropTargetItem(), ptldd->pSource);

	if (GetAsyncKeyState(VK_CONTROL)>>15 == 0)
		removeItemsFromCD();
	return true;
}

void CDDiskManager::onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info)
{
	CMenu menu;
	if (!info->item||isCdNode(info->item))
		menu.LoadMenu(IDR_CD_MNGR_TREE_MENU);
	else
		menu.LoadMenu(IDR_CD_MNGR_TREE_MENU1);
	CMenu *pm = menu.GetSubMenu(0);
	if (!info->item)
		pm->EnableMenuItem(ID_CD_DISK_MNGR_REMOVECD, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	CPoint p;
	GetCursorPos(&p);
	pm->TrackPopupMenu(TPM_LEFTBUTTON , p.x, p.y, this);
}
//! Перед изменением состояния
bool CDDiskManager::onItemStateChanging(CTreeListCtrl& tree, 
										ITEM_CHANGE_INFO* pci)
{
	if (pci->uNewState&TLIS_SELECTED)
	{
		if (!tree_.GetSelectedItem())
			return true;

	if (isCdNode(pci->item) != isCdNode(tree_.GetSelectedItem())) 
		return false;
	}
	return true;
}

