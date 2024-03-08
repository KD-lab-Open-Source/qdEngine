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

/// Класс диалога, раскладывающий весь материал по дискам
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

	/// Назначает диспетчер игры
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

	//! Начинаем тянуть выделенные элементы левой кнопкой мыши
	virtual bool onBeginDrag(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Уведомляет о том, что тянем курсор над окном
	virtual bool onDragOver(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);
	//! Уведомляет о том, что отжали кнопку мыши
	virtual bool onDrop(CTreeListCtrl& source, TREELISTDRAGDROP* ptldd);

	virtual void onRClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	//! Перед изменением состояния
	virtual bool onItemStateChanging(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);

	/// Добавляет диск
	afx_msg void OnCdDiskMngrAddcd();
	/// Удаляет объекты с диска
	afx_msg void OnCdDiskMngrRemove();
	/// Удаляет диск
	afx_msg void OnCdDiskMngrRemovecd();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	DECLARE_MESSAGE_MAP()
protected:
	/// Делает настройки CTreeListCtrl
	bool initTree();

	/// Раскладывает все объекты и видео по дискам в diskIndex_
	bool sortObjects();
	/// Раскладывает все объекты из контейнера по дискам в diskIndex_
	template<class Container>
	bool sortObjects(Container const& container);

	/// Формирует список изображений
	bool makeImageList();

	/// Добавляет все выбранные элементы pSource в диск описанный pParent
	void addItemsToCD(CTreeListItem* pParent, CTreeListCtrl* pSource);
	/// Добавляет объект(pSource) на диск (pParent)
	void addCDContent(CTreeListItem* pParent, CTreeListItem const* pSource);
	/// Удаляет объект(pSource) с диска (pParent)
	void removeCDContent(CTreeListItem* pParent, CTreeListItem const* item);
	/// Удаляет объекты выбранных элементов с дисков. А элементы из контрола
	void removeItemsFromCD();

	/// Является ли элемент элементом диска
	bool isCdNode(CTreeListItem* pParent);
	/// Есть ли среди чайлдов pParent элемент с ItemData == dwData
	bool isInChildList(CTreeListItem const* pParent, DWORD_PTR dwData) const;

	/// Вставляет объект в дерево под указанный элемент
	void insertObjectToTree(CTreeListItem* pParent, qdNamedObject const* pno);

	/// Возвращает количество дисков
	int getDisksCount() const;
	/// Назначает количество дисков
	void setDisksCount(int val);

	/// Обновляет для элемента диска информацию о размере места занимаемом данными
	void updateSizeInfo(CTreeListItem* pCDItem);
	/// Возвращет по элемнту номер диска
	int getCDNumber(CTreeListItem const* pCDItem) const;
	/// Опеределяет сколько вообще дисков
	void detectCDsCount();
	/// Загружает дерево с дисками и объектами на них
	void loadCDsTree();
	/// Загружает в дерево содержимое диска(pParent)
	void loadCDContent(CTreeListItem* pParent);
	/// Добавляет в дерево элемент для диска с номером num
	CTreeListItem* addCD(int num);

	/// Удаляет объект pno из diskIndex_ для диска iDiskNumber
	void removeFromDiskIndex(int iDiskNumber, qdNamedObject* pno);
	/// Добавляет объект pno в diskIndex_ для диска iDiskNumber
	void addToDiskIndex(int iDiskNumber, qdNamedObject* pno);
	/// Убирает дырку в списке идексов
	/**
		Меняя местами список объектов для ненужного диска со списком для следующего
		диска, постепенно сдвигает ненужный к концу. А затем удаляет
	 */
	void compressDiskIndex(int iDiskForRemove);
	/// Убирает дырку в списке размеров для дисков
	/**
		Алгоритм такой же как void compressDiskIndex(int iDiskForRemove)
	 */
	void compressDiskSizes(int iDiskForRemove);
	/// Для всех дисков обновляет инфу о размере данных, на них размещенных
	void updateDisks();

	/// Возвращает размер данных на диске в мегабайтах
	float getSize(int iDiskNum);

	/// По структуре дерева записывает инфу о размещении в диспетчер проекта
	void saveDiskInfoToProject();
	/// Заносит в диспетчер объекта информацию о конкретном диске
	void diskIndexEntryToProject(int iDiskNum, std::vector<qdNamedObject*> const& v);
	/// Стирает в проекте всю инфу о размещении объектов по дискам
	void clearProject();
	/// Выравнивает положение контролов при изменении размеров диалога
	void adjustControlsLayout();

	/// Можем ли бросить перетягиваемый элемент
	bool canDrop(CTreeListItem* target, CTreeListItem* pItem, 
		CTreeListCtrl* sourceCtrl) ;
	//! Копируем на нулевой диск все ресурсы, которые находились только на этом диске
	void copyNoOwnerContentToZeroDisk(CTreeListItem const* diskItem);

	//! По номеру диска возвращем его узел в дереве
	CTreeListItem* getDiskItem(int number) const;

	/// Проверяет находится ли объект на каком-нибуд другом диске, кроме указанного
	bool isObjectOnDisks(DWORD_PTR obj, CTreeListItem const* diskExclude) const;
protected:
	virtual void OnCancel();

private:
	//! Дерево с дисками
	QETreeListCtrl tree_;
	/// Диспетчер игры
	qdGameDispatcher* gameDispatcher_;
	/// Изображения для узлов в дереве
	CImageList imageList_;
	/// По номеру диска содержит его размер
	std::map<int, int> diskSizes_;
	/// Количество дисков
	int disksCount_;
	/// По номеру диска содержит объекты, которые на нем находятся
	std::map<int, std::vector<qdNamedObject*> > diskIndex_;
};
