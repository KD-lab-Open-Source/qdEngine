#pragma once
#include "treenodestube.h"

/// Базовый класс узла папки объектов
/**
	Классами этой ветви обслуживаются узлы, которым не соответствует объекта
	движка. Это те узлы, которые были привнесены искуственно. Они не отражают
	внутреннюю иерархию объектов движка.
	Класс папок несут минимальную функциональнсть. Обычно все ограничавается
	показом всплывающего меню.
 */
class FolderTreeNode : public TreeNodeStube
{
public:
	FolderTreeNode(DWORD folderId);
	~FolderTreeNode();
	/// Возвращает id папки.
	DWORD getFolderId() const;
	ContentClassificator getContentClassificator() const;
	virtual void onDeleteItem(TreeLogic& logic);
	NodeContent getContent() const;
protected:
private:
	/// Идентификатор папки
	/**
		Обычно идентификатор равен id тех объектов, которые содержатся в каталоге.
		Для папок состояний персонажа предусмотрены отдельные идентификаторы
	 */
	DWORD folderId_;
};
