#pragma	 once

#include "ScaleInterfaces.h"
/**
	Класс тулбара, в котором расположены контролы управления масштабом
 */
class CScaleBar :  public CToolBar
{
public:
	CScaleBar();
	/// Создает тулбар и размещает на нем комбо
	bool Create(CWnd* pparent, DWORD id);
	/// Назначает жертву
	void SetScalable(IScalable* psclbl);
	/// Обновляет цифру в комбе
	void UpdateScaleInfo();

private:
	/// Создает комбо и размещает его. Назначает правильный фонт
	void AddCombo();

	/// Назачает жертве, выбранный из списка масштаб
	void OnComboSelChange();
	/// Уменьшает масштаб на заданную величину
	void OnScaleMinus();
	/// Увеличвает масштаб на заданную величину
	void OnScalePlus();
	DECLARE_MESSAGE_MAP();
	/// Перехватывает ввод в эдит-бокс комбы
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	/// Добавляет предопределенные масштабы
	void InitCombo();

	/// Управляет доступностью кнопки уменьшения
	void OnUpdateScaleMinus(CCmdUI* pCmdUI);
	/// Управляет доступностью кнопки увеличения
	void OnUpdateScalePlus(CCmdUI* pCmdUI);

	/// Считывает строчку из эдита комбы и назначает его жертвы
	void ReadAndSetScale();
private:
	/// Комба с масштабами
	CCJFlatComboBox m_wndCombo;
	/// Фонт, котором пишем в комбе
	CFont m_font;

	/// Объект, в котором регулируем масштаб(ЖЕРТВА)
	IScalable* m_pScalable;
};
