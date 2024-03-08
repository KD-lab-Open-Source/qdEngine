#pragma once
#include "teworkmodebase.h"
#include "IDrawData.h"
#include <memory>
#include <vector>

class TEWnd;
class qdGameDispatcher;
class qdTriggerChain;

interface ITEDrawer;

/**
	Режим работы, в котором отрисовываются все цепочки разом.

 */
class TEAllChainWorkMode :
	public TEWorkModeBase
{
public:
	TEAllChainWorkMode(qdGameDispatcher* ptrDisp);
	~TEAllChainWorkMode(void);

	virtual void OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point);
	virtual void OnLButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point);
	virtual void OnMouseMove(TEWnd* pwnd, UINT nFlags, CPoint point);
	void OnPaint(TEWnd* pwnd, CDC* pdc, CRect const& rcPaint);
	/// Назначает дровер
	bool setDrawer(ITEDrawer* ptrDrawer);
	/// Возвращает дровер
	ITEDrawer* getDrawer() const{
		return m_ptrDrawer;
	}
	/// Назначает диспетчер проекта
	void setGameDisp(qdGameDispatcher* ptrDisp);
	/// Возвращает диспетчер проекта
	qdGameDispatcher* getGameDisp() const{
		return m_ptrGameDisp;
	}
protected:
	/// Задает верхний левый угол
	void setLTPos(CPoint const& p);
	/// Возвращает его
	CPoint const& getLTPos() const{
		return m_ptLTPos;
	}
	/// Назначает цепочку, с котророй работаем в текущий момент
	void setActiveChain(qdTriggerChain* pc);
	/// Возвращает активную цепочку
	qdTriggerChain* getActiveChain() const{
		return m_ptrActiveChain;
	}
	/// Назначает карандаш, которым рисуются прямоугольники
	void setRectPen(HPEN hPen);
	/// Возвращает карандаш, которым рисуются прямоугольники
	HPEN getRectPen() const{
		return m_hRectPen;
	}
	void setWasMouseMoved(bool bval);
	bool getWasMouseMoved() const{
		return m_bWasMouseMoved;
	}
protected:
	/// Класс, передающий в дровер данные для рисования
	class DrawDataKeeper : public IDrawData
	{
		std::vector<qdTriggerChain const*> m_data;
	public:
		DrawDataKeeper();
		~DrawDataKeeper();
		void setDrawedChains(qdTriggerChainList const* lst);
		std::pair<qdTriggerChain const* const*, int> getDrawedChains() const{
			return std::make_pair(&*m_data.begin(), m_data.size());
		}
	};
private:
	/// Дровер
	ITEDrawer* m_ptrDrawer;

	/// положение левого верхнего угла
	CPoint m_ptLTPos;
	/// цепочка, с которой в текущий момент работаем
	qdTriggerChain* m_ptrActiveChain;
	/// Карандаш, которым рисуем прямоугольник
	HPEN m_hRectPen;
	/// Карандаш, которым рисуем прямоугольник, когда его нельзя перенсти
	CPen m_redPen;
	/// Объект передающий в дровер данные для рисования
	std::auto_ptr<DrawDataKeeper> m_ptrDrawDataKeeper;

	qdGameDispatcher* m_ptrGameDisp;

	/// была ли сдвинута мышь
	bool m_bWasMouseMoved;
};
