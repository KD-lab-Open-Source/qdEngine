#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "boost/scoped_ptr.hpp"

class TrgProfLegend;
// TriggerDbgDlg dialog

class TriggerDbgDlg : public CDialog
{
	DECLARE_DYNAMIC(TriggerDbgDlg)

public:
	TriggerDbgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TriggerDbgDlg();

// Dialog Data
	enum { IDD = IDD_DLG_TRIG_DBG };

	/// Назначает редактор триггеров
	bool SetTriggerView(interface ITriggerView* ptrITView);

	/// Загружает qdTriggerProfiler записями, а потом загружает записи в список
	bool Load(const char* fname = NULL);
	bool LoadFromFile();
	/// Передвигает выбор на предыдущую запись
	bool Prev() const;
	/// Передвигает выбор на следующуй запись
	bool Next() const;

	/// Берет текущую запись и вычисляет для нее состояние триггера
	bool Activate() const;
	/// ничего не делает
	bool Deactivate() const;

	/// ничего не делает
	void OnShow();
	/// Скрывает, если было видимо окно легенды
	void OnHide();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	/// Выставляет положение сроллера. Пересчитывает состояния. Перерисовывает редактор
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	/// расставляет элементы по диалогу, чтобы они влазили в новый размер
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	/// На F5 перезагружает записи
	afx_msg void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);
	/// Загружает записи
	afx_msg void OnLoadDbg();
	afx_msg void OnLoadDbgFromFile();
	/// Очищает список записей
	afx_msg void OnClearDbgList();
	/// В ответ на сдвиг скроллера меняет выбранный элемент
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	/// Отображает диалог легенды
	afx_msg void OnBnClickedBtnShowLegend();
	/// Записывает записи в файл
	afx_msg void OnSavetofileProfilerData();


	DECLARE_MESSAGE_MAP()

	/// Передвигает выбор в списке записей вперед/назад на указанное количество записей
	/**
		Направление сдвига выбирается знаком
	 */
	bool MoveSelection(int count) const;
	/// Выставляет параметры скроллера. Дизейблит его, если он не нужен
	void SetupScroller();
	/// Выбирает указанный элемент списка
	bool SelectItem(int iItem) const;
private:
	//! Список строк
	mutable CListCtrl m_list;
	/// Редактор триггеров
	ITriggerView* m_ptrITView;

	/// Скроллбар для навигации по записям
	CScrollBar m_scrlr;
	/// Диалога легенды
	boost::scoped_ptr<TrgProfLegend> m_ptrTrgProfLegend;
};
