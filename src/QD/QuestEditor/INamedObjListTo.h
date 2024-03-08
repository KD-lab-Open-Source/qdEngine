#pragma once

class CListCtrl;
/**
	Интерфейс класс загрузчика в CListCtrl. Испльзуется совместно с 
	NamedObjectSelectorDlg. В доке к NamedObjectSelectorDlg изложенно более подробно, 
	как использовать этот класс. Смотреть в ProjectJoiner::selectSceneForJoin как можно 
	использовать
 */
interface INamedObjListToList
{
	virtual bool load(CListCtrl& lst) const = 0;
};