#pragma once


// TrgProfLegend dialog

/**
	Диалог отображает цвета элементов и линков, и что они означают
 */
class TrgProfLegend : public CDialog
{
	DECLARE_DYNAMIC(TrgProfLegend)

public:
	TrgProfLegend(CWnd* pParent = NULL);   // standard constructor
	virtual ~TrgProfLegend();

// Dialog Data
	enum { IDD = IDD_TRG_PROFILER_LEGEND };
	void initColors(COLORREF doned_link, COLORREF actve_link, 
					COLORREF inactive_link, COLORREF doned_ele, 
					COLORREF active_ele, COLORREF inactive_ele, COLORREF waiting_ele);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	/// по id контрола возвращает нужную кисть
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
private:
	/// Отобаотавший линк
	CBrush m_brDonedLink;
	/// Активный линк
	CBrush m_brActiveLink;
	/// Неактивный линк
	CBrush m_brInactiveLink;

	/// Отработавший элемент
	CBrush m_brDonedEle;
	/// Активный элемент
	CBrush m_brActiveEle;
	/// Неактивный элемент
	CBrush m_brInactiveEle;
	/// Ожидающий элемент
	CBrush m_brWaitingEle;
};
