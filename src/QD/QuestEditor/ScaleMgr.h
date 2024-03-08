#pragma once
#include "scaleinterfaces.h"
#include <memory>

class CScaleBar;

/**
	Управляет тулбаром с контролами для масштабирования
 */
class ScaleMgr : public IScaleMgr
{
public:
	/// Назначаем того, кого будем масштабировать
	void SetScalable(IScalable* pscalable);
	/// обновляем информацию о масштабе
	void UpdateScaleInfo();

	ScaleMgr(void);
	~ScaleMgr(void);

	/// Создает тулбар
	bool Create(CFrameWnd* pParent, DWORD barid);
	/// Показать
	void Show() const;
	/// Скрыть
	void Hide() const;
	/// Видим ли?
	bool IsVisible() const;

	/// возвращает указатель на окно тулбара
	CWnd* GetView();
private:
	/// Указатель на тулбар
	std::auto_ptr<CScaleBar> m_ptrScaleBar;
	/// Окно, в котором тулбар живет
	CFrameWnd* m_pFrameWnd;
};
