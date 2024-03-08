#pragma once

#include "basepage.h"
#include <boost/scoped_ptr.hpp>
#include "afxwin.h"
#include "qd_interface_element_state.h"

// InterfSaveProp dialog

class qdInterfaceSave;

/**
	Свойства qdInterfaceSave
 */
class InterfSaveProp : public CBasePage
{
	DECLARE_DYNAMIC(InterfSaveProp)

public:
	InterfSaveProp();
	virtual ~InterfSaveProp();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_SAVE_PROP };

	/// Назначает сейв, с которым будем работать
	void set_data(qdInterfaceSave* psave);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedBtnSelSound();
	afx_msg void OnBnClickedBtnSelAnim();
	afx_msg void OnBnClickedCheckHorzInv();
	afx_msg void OnBnClickedCheckVertInv();
	afx_msg void OnBnClickedCheckLoop();
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboModes();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	/// данные из контролов в сейв
	void wnd2save();
	/// данные из сейва в контролы диалога
	void save2wnd();

	/// данные режима в контролы диалога
	void mode2wnd(qdInterfaceElementState::state_mode_t const md);
	/// данные из контролов диалога в режим
	void wnd2mode(qdInterfaceElementState::state_mode_t const md);
	/// Загружает возможные режимы
	void LoadModes();
	/// Переназначает звуковой файл и файл анимации для сейва
	void reset_files();
	/// вызываю на изменения позиции или размеров
	void OnChangePosSize();

	/// Возвращает текущий режим для сейва
	qdInterfaceElementState::state_mode_t GetCurMode();
	/// возвращает режим отличный от данного(выбор из 2х)
	qdInterfaceElementState::state_mode_t GetAnotherMode(
		qdInterfaceElementState::state_mode_t const m);
private:
	//! Файл анимации
	CString m_strAnimFile;
	//! Звуковой файл
	CString m_strSoundFile;
	/// х
	int m_iX;
	/// у
	int m_iY;
	/// глубина
	int m_iD;
	/// размер по горизонтали
	int m_iSX;
	/// размер по вертикали
	int m_iSY;
	//! смещение текста при отображении сэйва
	int m_iTextDx;
	int m_iTextDy;

	int isAutosaveSlot_;

	/// Сейв, с которым работаем
	qdInterfaceSave* m_ptrSourceSave;
	/// Резервная копия
	boost::scoped_ptr<qdInterfaceSave> m_ptrBack;
	//! Идентификатор сейва
	INT m_iSaveId;
	//! Описывают взаимоотношение мыши с элементом
	CCJFlatComboBox m_wndModes;
};
