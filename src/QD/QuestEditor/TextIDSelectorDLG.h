#pragma once
#include "afxcmn.h"

#include "qd_textdb.h"
// TextIDSelectorDLG dialog

/**
	Класс позволяет выбрать идентификатор строки из текстовой базы данных.
	Получение идентификатора происходит путем вызова TextIdSelector::run
 */
class TextIDSelectorDLG : public CDialog
{
	DECLARE_DYNAMIC(TextIDSelectorDLG)

public:
	TextIDSelectorDLG(CWnd* pParent = NULL);   // standard constructor
	virtual ~TextIDSelectorDLG();

// Dialog Data
	enum { IDD = IDD_DLG_SELECT_TEXT_ID };

	/// Назначает текстовую базу данных
	void setTextDB(qdTextDB const* ptrTextDB);
	/// Устанавливает текущий идентификатор строки
	void setCurrentId(CString const& textId);
	CString const getSelectedId() const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnGetdispinfoTree(NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	/// Загружает все корневые идентификаторы строк
	bool loadRootIds();
	/// Загружает список идентификторов в дерево под указанного родителя
	bool loadIds(qdTextDB::IdList const& idList, HTREEITEM hParent);
	/// Возвращает маску идентификатора
	/**
		Составляет строку, в которой через '.' перечислены имя узла и имена 
		всех его родителей в дереве по порядку от самого узла к корню дерева
	 */
	CString const getIDMask(HTREEITEM hItem) const;

	/// \brief Разворачивает дерево до тех пор пока не будет виден узел, 
	///  маска соответствует currentId_
	void showCurrentId() ;
	/// Находит среди дочерних узлов узел, текст которого равен text
	HTREEITEM findItemByText(CString const& text, HTREEITEM hParent) const;
	/// Вставляет вспециальный узел, символизирующий отсутсвие текста
	void insertNOTEXTItem();
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	/// Содержит текст, соответствующий выбранному идентификатору
	/**
		Связана по средством MFC со статиком лежащим поверх дерева
	 */
	CString textPreview_;
	/// Дерево идентификаторов
	CTreeCtrl tree_;

	/// Текстовая база
	qdTextDB const* ptrTextDB_;
	//! идентификатор, который выбран
	CString currentId_;
};
