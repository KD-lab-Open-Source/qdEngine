#pragma once


#include <boost/scoped_ptr.hpp>
#include "BasePage.h"
// ScreenTextPropPage dialog

class qdScreenTextDispatcher;
class qdScreenTextSet;

/**
	Свойства экрана с текстами
 */
class ScreenTextPropPage : public CBasePage
{
	DECLARE_DYNAMIC(ScreenTextPropPage)

public:
	ScreenTextPropPage();
	virtual ~ScreenTextPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_SCREEN_TEXT };

	/// Назначает диспетчер текстовых экранов и обрабатываемый экран
	void set_data(qdScreenTextDispatcher* pstdisp, qdScreenTextSet* pset);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* );
	afx_msg void OnRadioBtnClick();

	DECLARE_MESSAGE_MAP();

	/// Данные из контролов в обрабатываемый объект
	void wnd2set();
	/// Параметы объекта в контролы дилаога
	void set2wnd();

	/// Ничего не делает. Похоже не нужна
	int getHorzAlignmentNum(int alignment);
	/// Ничего не делает. Похоже не нужна
	int getVertAlignmentNum(int alignment);
	/// Ничего не делает. Похоже не нужна
	void setAlignment();
private:
	/// Обрабатываемый набор текстов
	qdScreenTextSet*		pScreenTextSet_;
	/// Диспетчер экранов текста
	qdScreenTextDispatcher*	pScreenTextDisp_;

	/// Горизонтальное выравнивание
	BOOL iHorzAlign_;
	/// Вертикальное выранвивание
	BOOL iVertAlign_;
	/// Позиция левого края
	int iLeft_;
	/// Позиция верхнего края
	int iTop_;
	/// Ширина
	int iSX_;
	/// Высота
	int iSY_;
	/// резервная копия, что можно было отменить все
	boost::scoped_ptr<qdScreenTextSet> screenTextSetCopy_;
	//! Ориентация текста(по горизонтали или по вертикали)
	BOOL iOrientation_;
};
