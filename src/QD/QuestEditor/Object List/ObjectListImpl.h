#pragma once

#include "IObjectList.h"
#include <TreeListCtrl/Null Listeners/NullGeneralNotifyListener.h>
#include <TreeListCtrl/Null Listeners/NullItemOperationNotifyListener.h>

class QETreeListCtrl;
class qdNamedObjectReference;

/// Объект, реализующий работу со списком. Реально выполняет все действия со списокм
class ObjectListImpl : public NullGeneralNotifyListener,
					   public NullItemOperationNotifyListener
{
public:
	ObjectListImpl(QETreeListCtrl* listWindow);
	~ObjectListImpl(void);

	/// Назначает контрол, содержащий список
	void setListWindow(QETreeListCtrl* listWindow);
	/// Возвращает контрол, содержащий список
	QETreeListCtrl* getListWindow() const;

	/// Показывает объект в списке
	bool showObjectInList(qdNamedObjectReference const* );
	/// Добавляет объект к списку
	HITEM addObjectToList(qdNamedObjectReference const*, HITEM hItemRoot);
	/// Удаляет объект из списка
	bool removeObjectFromList(qdNamedObjectReference const* );
	/// Очищает список
	void clearList();

	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	/// Вставляет в listWindow_ столбец и выставляет его ширину
	bool init();
protected:
	//! Получаем строку,которая будет фигурировать в списке
	static CString getObjectListLabel(qdNamedObjectReference const& objRef);

	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	virtual void onDeleteItem(CTreeListCtrl& tree, ITEM_OPERATION_INFO* info);

private:
	/// Контрол, отображащий список
	QETreeListCtrl* listWindow_;
};
