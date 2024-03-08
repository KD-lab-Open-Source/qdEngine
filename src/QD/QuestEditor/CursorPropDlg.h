#pragma once


#include "RqstngStateEdit.h"
#include <memory>
#include "afxwin.h"
// CursorPropDlg dialog
class qdGameDispatcher;
class qdGameObjectStateStatic;
class qdGameObjectMouse;

/// Диалог позволяющий редактировать свойства курсора
class CursorPropDlg : public CDialog, public IRequestingStateEdit
{
	DECLARE_DYNAMIC(CursorPropDlg)

public:
	CursorPropDlg(HWND hOwnedWnd = NULL);   // standard constructor
	virtual ~CursorPropDlg();

// Dialog Data
	enum { IDD = IDD_DLG_CURSOR_PROP };

	/// Назначает диспетчер и собственно курсор, с которым работаем
	bool set_data(qdGameDispatcher* ptrDisp, qdGameObjectStateStatic* ptrCursor);

	// Реализация интерфейса IRequestingStateEdit


	/// Возвращает исходный курсор(состояние, которое его описывает)
	qdGameObjectState* GetState();
	/// Прекращает стыковку курсора
	void EndStackState();
	/// Прекращает редактирование размера(Не поддерживается)
	void EndEditStateDims();
	/// Прекращает редактирование КА для кусора(Не поддерживается)
	void EndEditCoordAnimation();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	/// Восстанавливает из резервной копии 
	virtual void OnCancel();
	/// Заносит изменеия в курсор
	virtual void OnOK();
	afx_msg void OnDeltaposSpinMoveSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMoveRadius(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnStackState();
	DECLARE_MESSAGE_MAP()

	/// переносит параметры из курсора в контролы диалога
	void cursor2wnd();
	/// из контролов диалога переносит данные в курсор
	void wnd2cursor();

	/// Загружает анимацию из глобального каталога анимации
	void LoadAnimations();
private:
	/// Диспетчер проекта
	qdGameDispatcher* m_ptrGameDisp;
	/// Исходный курсор
	qdGameObjectStateStatic* m_ptrSourceCursor;
	/// Резервная копия. 
	std::auto_ptr<qdGameObjectStateStatic> m_ptrCopyCursor;

	/// Окно, которое инициировала создание диалога
	HWND m_hOwnedWnd;

	/// Список доступной анимации
	CCJFlatComboBox m_wndAnim;
	/// Зацикливать анимацию
	BOOL m_bLoop;
	/// Инвертировать горизонтально
	BOOL m_bFlipHorz;
	/// Инвертировать вертикально
	BOOL m_bFlipVert;
	/// Курсор для динамических объектов
	BOOL m_bForDynObj;
	/// Курсор для зон
	BOOL m_bForZones;
	/// Курсор для главного меню
	BOOL m_bForMainMenu;
	/// Курсор для внутриигрового интерфейса
	BOOL m_bForIngameInterf;
	/// Это есть m_ptrSourceCursor->rnd_move_radius()
	float moveRadius_;
	/// Это есть m_ptrSourceCursor->rnd_move_speed()
	float moveSpeed_;
	/// Для объектов в инвентори
	BOOL forObjectsInInventory_;
};
