
#ifndef __OBJECT_LIST_HEADER
#define __OBJECT_LIST_HEADER

class qdNamedObjectReference;

DECLARE_HANDLE(HITEM);
HITEM const OL_ROOT = reinterpret_cast<HITEM>(0x0000ffff);

/// Интерфейс класса обображающего список объектов.
struct IObjectList
{
	/// Показать список
	virtual void show() = 0;
	/// Скрыть список
	virtual void hide() = 0;
	/// Показать объект в списке
	virtual bool showObjectInList(qdNamedObjectReference const* ) = 0;
	/// Добавить объект к списку.
	/**
		@param hRootItem - узел, под который будет добавлен объект
	 */
	virtual HITEM addObjectToList(qdNamedObjectReference const*, 
									HITEM hRootItem = OL_ROOT) = 0;
	/// Удалить из списка
	virtual bool removeObjectFromList(qdNamedObjectReference const* ) = 0;
	/// Очистить список
	virtual void clearList() = 0;
	/// Назначает заголовок для списка объектов
	virtual void setListCaption(CString const& str) = 0;
};
#endif//__OBJECT_LIST_HEADER