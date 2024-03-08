#pragma once

#include "BasePage.h"
#include "afxwin.h"
#include "interfsliderpurposes.h"
// InterfSldrProp dialog

/**
	—войства интерфейсного слайдера
 */
class InterfSldrProp : public CBasePage
{
	DECLARE_DYNAMIC(InterfSldrProp)

public:
	InterfSldrProp();
	virtual ~InterfSldrProp();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_SLIDER };

	/// —лайдер, с которым будем работать
	void set_data(class qdInterfaceSlider* pslider);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedOriHorz();
	afx_msg void OnBnClickedOriVert();

	afx_msg void OnDeltaposSpinX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinY(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinD(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboTarget();

	afx_msg void OnDeltaposSpinArSx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinArSy(NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	/// «агружает список возможных применений слайдера
	void LoadPurposes();

	DECLARE_MESSAGE_MAP()

	/// восстанавливает данные из резервной копии
	void bak2real();
	/// ƒелает резервную копии дл€ последующего восстановлени€
	void real2bak();

	/// ƒанные из контролов диалога в слайдер
	void wnd2slider();
	/// обновл€ет позицию слайдера на экране
	void update_pos();
	//! обновл€ет активный пр€моугольник
	void update_ar();

	/// данные из слайдера в переменные св€занные с контролами диалога
	void slider2wnd();

	/// ќтображает заданное дл€ слайдера применение в комбобоксе применений
	void purpose2wnd();
	/// —охран€ет в слайдере выбранное в комбобоксе применение
	void wnd2purpose();
private:
	/// х
	int m_iX;
	/// у
	int m_iY;
	/// √лубина
	int m_iD;
	/// Ўирина активного пр€моугольника
	int m_iARSX;
	/// ¬ысота активного пр€моугольника
	int m_iARSY;

	//! √оризонтальна€ (0), вертикальна€ (1) ориентаци€ слайдера
	BOOL m_iOrient;
	BOOL inverseDirection_;
	//! что регулирует слайдер
	CCJFlatComboBox m_wndTarget;

	//мы не можем просто скопировать объкт слайдер.
	//потому что он еще используетс€ на других страничках

	/// резервный х
	int m_iXBak;
	/// резервный у
	int m_iYBak;
	/// резервна€ глубина
	int m_iDBak;
	/// резервна€ ориентаци€
	BOOL m_iOrientBak;
	BOOL inverseDirectionBak_;
	/// резервный ширина активного пр€моугольника
	int m_iARSXBak;
	/// резервный высота активного пр€моугольника
	int m_iARSYBak;
	/// нужна здесь исключительно дл€ того, чтобы можно было откатить изменени€
	qdInterfaceElement::option_ID_t m_eOptionIdBak;

	/// —лайдер
	class qdInterfaceSlider* m_pSlider;
	/// —писок применений
	InterfSliderPurposes m_Purposes;
};
