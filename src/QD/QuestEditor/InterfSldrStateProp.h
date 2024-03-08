#pragma once

#include "BasePage.h"
#include <boost/scoped_ptr.hpp>
// InterfSldrStateProp dialog

/**
	Возможные состояния слайдера
 */
enum eStateChoose{
	SC_BACK, ///< Состояние бекграунда
	SC_SLIDER///< Состяние полузунка
};
/**
	Свойства состояния слайдера
 */
class InterfSldrStateProp : public CBasePage
{
	DECLARE_DYNAMIC(InterfSldrStateProp)

public:

	InterfSldrStateProp(eStateChoose stc, UINT idCap);
	virtual ~InterfSldrStateProp();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_SLIDER_STATE };

	/// Назначает слайдер, для работы
	void set_data(class qdInterfaceSlider* pslider);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedBtnSelAnim();
	afx_msg void OnBnClickedBtnSelSound();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnDeltaposSpinOffsX(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinOffsY(NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	/// Состояние в контролы диалога 
	void state2wnd();
	/// Данные из контролов диалога в состояние
	void wnd2state();

	/// Инвертирует указанный флаг
	void invert_flag(int flag);
	/// Устанавливает файл анимации
	void reset_anim();
	/// Устанавливает звуковой файл
	void reset_sound();
	/// Сохраняет измененное состояние в слайдере
	void update_state(const class qdInterfaceElementState & pstate);
private:
	/// Файл анимации
	CString m_strAnimFile;
	/// Звуковой файл
	CString m_strSoundFile;
	/// С каким состоянием слайдера работаем
	eStateChoose const m_eStateChoose;

	/// смещение подложки
	Vect2i backgroundOffset_;
	/// резервная копия смещения подложки
	Vect2i backgroundOffsetBack_;

	/// Резервная копия
	boost::scoped_ptr<class qdInterfaceElementState> m_ptrBack;
	/// Состояние, в котором проводим все изменения
	/**
		Это отдельная копия, потому что слайдер не дает возоможности 
		редактировать свои состояния напрямую
	 */
	boost::scoped_ptr<class qdInterfaceElementState> m_ptrForChange;
	/// Слайдер
	qdInterfaceSlider* m_ptrSlider;
};
