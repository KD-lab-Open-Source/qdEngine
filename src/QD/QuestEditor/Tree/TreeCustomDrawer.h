#pragma once

#include <TreeListCtrl/Null Listeners/NullCustomDrawNotifyListener.h>

/// Отрисовывает узлы дерева собственными цветами и шрифтом
class TreeCustomDrawer : public NullCustomDrawNotifyListener
{
public:
	TreeCustomDrawer(void);
	~TreeCustomDrawer(void);

	/// Определяет цвет и шрифт, которым будет рисоваться узел
	DWORD onItemPrepaint(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi);
	/// Возвращает в DC предыдущий фонт
	DWORD onItemPostpaint(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi);
protected:
	/// \brief Определяет каким цветом будет закрашено пространство под текстом узла
	/// и задает шрифт
	void selectItemBack(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi);
	/// \brief Определяет каким цветом будет закрашено пространство под текстом узла
	/// для узлов состояний
	void selectStateItemBack(qdGameObjectState* pstate, 
									ITEM_CUSTOM_DRAW_INFO* pcdi);
private:
	/// Фонт для отрисовки шрифта
	CFont font_;
	/// Предыдущий фонт, который надо восстановить
	HGDIOBJ oldFont_;
};
