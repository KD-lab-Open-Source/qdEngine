#pragma once

class CListCtrl;
/// Интерфейс класс загрузчика в CListCtrl.
/**
	Испльзуется совместно с 
	NamedObjectSelectorDlg. В доке к NamedObjectSelectorDlg изложенно более подробно, 
	как использовать этот класс. Смотреть в ProjectJoiner::selectSceneForJoin как можно 
	использовать
 */
struct INamedObjListReader
{
	virtual bool read(CListCtrl& lst) = 0;
};