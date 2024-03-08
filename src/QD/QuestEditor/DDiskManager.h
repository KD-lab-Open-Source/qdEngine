#pragma once
#include "Main/QETreeListCtrl.h"
#include <TreeListCtrl/Null Listeners/NullItemDragdropNotifyListener.h>
#include <TreeListCtrl/Null Listeners/NullGeneralNotifyListener.h>
#include <TreeListCtrl/Null Listeners/NullItemChangeNotifyListener.h>

#include <vector>
#include <map>

class qdGameDispatcher;
class qdNamedObject;

// CDDiskManager dialog

/// ����� �������, �������������� ���� �������� �� ������
class CDDiskManager : public CDialog
					, public NullItemDragdropNotifyListener
					, public NullGeneralNotifyListener
					, public NullItemChangeNotifyListener
{		
	DECLARE_DYNAMIC(CDDiskManager)

	class SortToDisk;
public:
	CDDiskManager(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDDiskManager();

// Dialog Data
	enum { IDD = IDD_DLG_DISK_SORT };

	/// ��������� ��������� ����
	void setDispatcher(qdGameDispatcher* pdispatcher);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
/*
	void onRClickInCustom(NMHDR* phdr, LRESULT* pResult);
	void onItemChangingInCustom(NMHDR* phdr, LRESULT* pResult);
	void onDropInCustom(NMHDR* phdr, LRESULT* pResult);
	void onDragOverInCustom(NMHDR* phdr, LRESULT* pResult);
	void onDragOverInCustomSceneVideo(NMHDR* phdr, LRESULT* pResult);
	void onBeginDragInCustom(NMHDR* phdr, LRESULT* pResult);
*/
//	void onItemChangingInCustom(NMHDR* phdr, LRESULT* pResult);

	//! �������� ������ ���������� �������� ����� ������� ����
	virtual bool onBeginDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! ���������� � ���, ��� ����� ������ ��� �����
	virtual bool onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! ���������� � ���, ��� ������ ������ ����
	virtual bool onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);

	virtual void onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	//! ����� ���������� ���������
	virtual bool onItemStateChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);

	/// ��������� ����
	afx_msg void OnCdDiskMngrAddcd();
	/// ������� ������� � �����
	afx_msg void OnCdDiskMngrRemove();
	/// ������� ����
	afx_msg void OnCdDiskMngrRemovecd();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	DECLARE_MESSAGE_MAP()
protected:
	/// ������ ��������� CTreeListCtrl
	bool initTree();

	/// ������������ ��� ������� � ����� �� ������ � diskIndex_
	bool sortObjects();
	/// ������������ ��� ������� �� ���������� �� ������ � diskIndex_
	template<class Container>
	bool sortObjects(Container const& container);

	/// ��������� ������ �����������
	bool makeImageList();

	/// ��������� ��� ��������� �������� pSource � ���� ��������� pParent
	void addItemsToCD(CTreeListItem* pParent, CTreeListCtrl* pSource);
	/// ��������� ������(pSource) �� ���� (pParent)
	void addCDContent(CTreeListItem* pParent, CTreeListItem const* pSource);
	/// ������� ������(pSource) � ����� (pParent)
	void removeCDContent(CTreeListItem* pParent, CTreeListItem const* item);
	/// ������� ������� ��������� ��������� � ������. � �������� �� ��������
	void removeItemsFromCD();

	/// �������� �� ������� ��������� �����
	bool isCdNode(CTreeListItem* pParent);
	/// ���� �� ����� ������� pParent ������� � ItemData == dwData
	bool isInChildList(CTreeListItem const* pParent, DWORD_PTR dwData) const;

	/// ��������� ������ � ������ ��� ��������� �������
	void insertObjectToTree(CTreeListItem* pParent, qdNamedObject const* pno);

	/// ���������� ���������� ������
	int getDisksCount() const;
	/// ��������� ���������� ������
	void setDisksCount(int val);

	/// ��������� ��� �������� ����� ���������� � ������� ����� ���������� �������
	void updateSizeInfo(CTreeListItem* pCDItem);
	/// ��������� �� ������� ����� �����
	int getCDNumber(CTreeListItem const* pCDItem) const;
	/// ����������� ������� ������ ������
	void detectCDsCount();
	/// ��������� ������ � ������� � ��������� �� ���
	void loadCDsTree();
	/// ��������� � ������ ���������� �����(pParent)
	void loadCDContent(CTreeListItem* pParent);
	/// ��������� � ������ ������� ��� ����� � ������� num
	CTreeListItem* addCD(int num);

	/// ������� ������ pno �� diskIndex_ ��� ����� iDiskNumber
	void removeFromDiskIndex(int iDiskNumber, qdNamedObject* pno);
	/// ��������� ������ pno � diskIndex_ ��� ����� iDiskNumber
	void addToDiskIndex(int iDiskNumber, qdNamedObject* pno);
	/// ������� ����� � ������ �������
	/**
		����� ������� ������ �������� ��� ��������� ����� �� ������� ��� ����������
		�����, ���������� �������� �������� � �����. � ����� �������
	 */
	void compressDiskIndex(int iDiskForRemove);
	/// ������� ����� � ������ �������� ��� ������
	/**
		�������� ����� �� ��� void compressDiskIndex(int iDiskForRemove)
	 */
	void compressDiskSizes(int iDiskForRemove);
	/// ��� ���� ������ ��������� ���� � ������� ������, �� ��� �����������
	void updateDisks();

	/// ���������� ������ ������ �� ����� � ����������
	float getSize(int iDiskNum);

	/// �� ��������� ������ ���������� ���� � ���������� � ��������� �������
	void saveDiskInfoToProject();
	/// ������� � ��������� ������� ���������� � ���������� �����
	void diskIndexEntryToProject(int iDiskNum, std::vector<qdNamedObject*> const& v);
	/// ������� � ������� ��� ���� � ���������� �������� �� ������
	void clearProject();
	/// ����������� ��������� ��������� ��� ��������� �������� �������
	void adjustControlsLayout();

	/// ����� �� ������� �������������� �������
	bool canDrop(CTreeListItem* target, CTreeListItem* pItem, 
		CTreeListCtrl* sourceCtrl) ;
	//! �������� �� ������� ���� ��� �������, ������� ���������� ������ �� ���� �����
	void copyNoOwnerContentToZeroDisk(CTreeListItem const* diskItem);

	//! �� ������ ����� ��������� ��� ���� � ������
	CTreeListItem* getDiskItem(int number) const;

	/// ��������� ��������� �� ������ �� �����-����� ������ �����, ����� ����������
	bool isObjectOnDisks(DWORD_PTR obj, CTreeListItem const* diskExclude) const;
protected:
	virtual void OnCancel();

private:
	//! ������ � �������
	QETreeListCtrl tree_;
	/// ��������� ����
	qdGameDispatcher* gameDispatcher_;
	/// ����������� ��� ����� � ������
	CImageList imageList_;
	/// �� ������ ����� �������� ��� ������
	std::map<int, int> diskSizes_;
	/// ���������� ������
	int disksCount_;
	/// �� ������ ����� �������� �������, ������� �� ��� ���������
	std::map<int, std::vector<qdNamedObject*> > diskIndex_;
};
