#pragma once

#include "InterfElePosPage.h"
#include <memory>
// InterfBtnProp dialog

#include "GameOptionRepresMngr.h"
#include "afxwin.h"

class qdInterfaceButton;
class qdInterfaceDispatcher;

/// Свойства кнопки интерфейса
class InterfBtnProp : public InterfElePosPage
{
	DECLARE_DYNAMIC(InterfBtnProp)

public:
	InterfBtnProp();
	virtual ~InterfBtnProp();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_BTN_PROP };

	/// Назначает кнопку и диспетчер интефейса
	void set_data(qdInterfaceDispatcher* pd, qdInterfaceButton* pbtn);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnCbnSelchangeComboOptions();

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	/// Применяет изменения
	virtual BOOL OnApply();
	/// Отменяет изменения
	virtual void OnCancel();
	/// Загружает список опций игры
	bool LoadOptions();
	/// Акссесор к мендежеру списка опций игры
	GameOptionRepresMngr const &getGameOptions() const{
		return m_gameOptions;
	}
	/// Индентификатор выбранной в комбобоксе опции игры
	qdInterfaceElement::option_ID_t getOptionId() const;
	
	/// Параметры кнопки в контролы диалога
	void btn2wnd();
	/// Данные из контролов диалога в параметры кнопки
	void wnd2btn();
private:
	/// Менеджер списка опций игры
	GameOptionRepresMngr const m_gameOptions;
	/// Контрол содержащий список опций игры
	CCJFlatComboBox m_wndOptions;

	/// Исходная кнопка
	qdInterfaceButton* m_pSourceBtn;
	/// Резервная копия
	std::auto_ptr<qdInterfaceButton> m_pBackBtn;
};
