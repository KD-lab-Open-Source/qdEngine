#pragma once
#include "shelltree\shelltree.h"


// SelectDirDlg dialog
//открывает текущую директорию

/**
	Использование этого класс следует заменить на вызов disk_op::GetDiskFolder
	Он был сделан, потому что по началу не удалось показать в шельном диалоге текущий
	каталог
 */
class SelectDirDlg : public CDialog
{
	DECLARE_DYNAMIC(SelectDirDlg)

public:
	SelectDirDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SelectDirDlg();

// Dialog Data
	enum { IDD = IDD_DLG_DIR_SELECT };

	/// Возвращет выбранный путь
	const CString& GetPath()const{
		return m_strFullPath;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
private:
	/// Выбранный путь
	CString m_strFullPath;

	CShellTree m_tree;

	/// Идет ли загрузка данных в дерево
	bool m_bTunneling;
	/// Нужна для CShellTree
	CString m_strRoot;
};
