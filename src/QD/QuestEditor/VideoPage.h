#pragma once


#include "basepage.h"
#include <memory>
// VideoPage dialog
class qdGameDispatcher;
class qdVideo;

/**
	Страница свойств видео
 */
class VideoPage : public CBasePage
{
	DECLARE_DYNAMIC(VideoPage)

	/// Копия объекта для возможного восстановления
	std::auto_ptr<qdVideo> m_ptrVideoCopy;
	/// Исходный объект
	qdVideo* m_ptrSource;
	/// Диспетчер проекта
	qdGameDispatcher* m_ptrGameDisp;
public:
	VideoPage();
	virtual ~VideoPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_VIDEO };

	/// Назначает диспетчер и редактируемое видео
	void set_data(qdGameDispatcher* ptrDisp, qdVideo* ptrVideo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnApply();

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnSelectBfile();
	afx_msg void OnBnClickedBtnSelectVfile();
	afx_msg void OnEnKillfocusEditVideoFile();
	afx_msg void OnEnKillfocusEditBackFile();
	afx_msg void OnBnClickedCheckToCenter();
	afx_msg void OnBnClickedCheckFullScreen();
	/// Вызывается для всех чекбоксов в диалоге
	afx_msg void OnClickInCheck();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void OnEnterInPos();
	void OnEnterInVideoFile();
	void OnEnterInBackFile();

	/// Параметры из объекта в контролы диалога
	void video2wnd();
	/// Данные из контролов в объект
	void wnd2video();

	/// Управляет доступностью контролов редактирования положения на экране
	void EnableLayout(bool benable);
private:
	/// Положение левой границы
	int m_iX;
	/// Положение верхней границы
	int m_iY;
	/// Файл бакграунда
	CString m_strBackFileName;
	/// Файл видео
	CString m_strVideoFileName;

    //! Расположить в центре
	BOOL m_bToCenter;
	//! На весь экран
	BOOL m_bFullScreen;
	//! Нельзя прервать
	BOOL m_bNoInterrupt;
	//! Будет ли видео проигрываться перед началом игры
	BOOL m_bIntroVideo;
	/// Разрешает музыку
	BOOL m_bEnableMusic;
};
