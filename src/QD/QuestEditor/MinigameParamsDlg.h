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
    ����� �������, �������������� ��������� ��������
	����� ������� �������������� ������� editMinigameParams
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

	//! �������� ������ ���������� � ������ ��������, ������� ��� ��������� ����� ������
	void copyParamsToChangers();
	//! ������� ������ ���������� �� ������ ��������
	void copyChangersToParams();

	//! ��������� ������ � ������ � �������
	void loadToList();
	void initList();

	//! ����� ��������� ���������
	virtual void onItemStateChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* pci);
	/*!
	������������ �������� ����� ����� ������ ��� ������
	\retval true - ��� ����, ����� ��������� ��������� �� ���������
	\retval false - ��� ����, ����� ��������� ��������� �� ���������
	*/
	virtual bool onDBLClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
private:
	QETreeListCtrl treeList_;
	qdGameDispatcher* gameDisp_;
	qdMiniGame* minigame_;
	//! ������ ��������
	ParamChangers paramChangers_;
};
