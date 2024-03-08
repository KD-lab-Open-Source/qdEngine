#pragma once

#include "Main/QETreeListCtrl.h"
#include <TreeListCtrl/Null Listeners/NullItemChangeNotifyListener.h>
#include <TreeListCtrl/Null Listeners/NullGeneralNotifyListener.h>
// MinigameParamsDlg dialog

class qdMiniGame;
class qdGameDispatcher;
class MGParamChanger;

#include "qd_minigame.h"
#include <vector>

/*!
    Класс диалога, редактирующего параметры миниигры
	Вызов диалога осуществляется вызовом editMinigameParams
*/
class MinigameParamsDlg : public CDialog
						, public NullItemChangeNotifyListener
						, public NullGeneralNotifyListener
{
	DECLARE_DYNAMIC(MinigameParamsDlg)

	typedef std::vector<MGParamChanger*> ParamChangers;
public:
	MinigameParamsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MinigameParamsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MINIGAME_PARAMS };

	void set_data(qdGameDispatcher* gameDisp, qdMiniGame* miniGame);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	
	DECLARE_MESSAGE_MAP();

	//! Копирует список параметров в список объектов, которые эти параметры будут менять
	void copyParamsToChangers();
	//! Создает список параметров из списка ченжеров
	void copyChangersToParams();

	//! загружает данные в список в диалоге
	void loadToList();
	void initList();

	//! После изменения состояния
	virtual void onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	/*!
	Возрващаемое значение имеет смысл только для дерева
	\retval true - для того, чтобы позволить обработку по умолчанию
	\retval false - для того, чтобы запретить обработку по умолчанию
	*/
	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
private:
	QETreeListCtrl treeList_;
	qdGameDispatcher* gameDisp_;
	qdMiniGame* minigame_;
	//! Список ченжеров
	ParamChangers paramChangers_;
};
