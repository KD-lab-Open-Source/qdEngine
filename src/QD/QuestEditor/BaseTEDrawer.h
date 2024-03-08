/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   15:58
	filename: 	d:\Projects\Quest\QuestEditor\BaseTEDrawer.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	BaseTEDrawer
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Реализация базовых потребностей. Отрисовка элементов, связей, сетки.
*********************************************************************/

#pragma once
#include "TEDrawerBase.h"
#include "IDrawData.h"
#include "qd_trigger_element.h"

class TEWnd;
class qdTriggerChain;

/**
	Реализация базовых потребностей. Отрисовка элементов, связей, сетки.
 */
class BaseTEDrawer : public TEDrawerBase
{
public:
	BaseTEDrawer(void);
	virtual ~BaseTEDrawer(void);
	/// Вызывает отрисовывку сетки
	void draw(CDC* pdc, TEWnd* pwnd, IDrawData* pdd, CRect const& rcPaint) const;
	void drawDraggedRect(HDC dc, CRect const&r, HPEN hpen) const;
	/// Отрисовывает сетку
	void drawNet(CDC* pdc, TEWnd* pwnd, CRect const& rcPaint) const;
protected:
	/// По элементу определяет цвет фона под текстом
	virtual HBRUSH	SelectEleBackBrush(qdTriggerElementConstPtr const& pele) const;
	/// По линку определяет кисть для закраски треугольника
	virtual HBRUSH	SelectLinkBrColor(qdTriggerLink const* plink) const;
	/// По линку определяет карандаш для отрисовки линка
	virtual HPEN	SelectLinkPenColor(qdTriggerLink const* plink) const;

protected://геттеры и сеттеры
	void setTriggerOffset(CSize const& sz) const;
	CSize const& getTriggerOffset() const{
		return m_szTriggerOffset;
	}
	BOOL setNetPen(int nPenStyle, int nWidth, COLORREF crColor);
	BOOL setNetPen(HPEN hPen);
	HPEN getNetPen() const{
		return m_NetPen;
	}
	BOOL setAutoRestartPen(int nPenStyle, int nWidth, COLORREF crColor);
	BOOL setAutoRestartPen(HPEN hPen);
	HPEN getAutoRestart() const{
		return m_penAutoRestart;
	}
	void setCurrentDrawedChain(qdTriggerChain const* pchain) const;
	qdTriggerChain const* getCurrentDrawedChain() const{
		return m_ptrCurrentDrawedChain;
	}

	BOOL setNormalEleBrush(HBRUSH hBrush);
	BOOL setNormalEleBrush(COLORREF crColor);
	HBRUSH getNormalEleBrush() const{
		return m_brNormalEleBrush;
	}
	BOOL setSelEleBrush(HBRUSH hBrush);
	BOOL setSelEleBrush(COLORREF crColor);
	HBRUSH getSeldEleBrush() const{
		return m_brNormalEleBrush;
	}

	void setDrawedLinkOwner(qdTriggerElement const* pele) const;
	qdTriggerElement const* getDrawedLinkOwner() const{
		return m_ptrDrawedLinkOwner;
	}
	void setPaintRect(CRect const& rc) const;
	CRect const& getPaintRect() const;

protected:
	/// Отрисовывает элемент
	void DrawElement(TEWnd* pwnd, CDC* pdc, 
		qdTriggerElementConstPtr const& ptrEle) const;
	/// отрисовывает линк
	void DrawLink(TEWnd* pwnd, HDC dc, CPoint pt_from, 
				CPoint pt_to, qdTriggerLink const* pl) const;
	/// отрисовывает линки в цепочке
	void DrawLinks(TEWnd* pwnd, CDC* pdc, qdTriggerChain const* ptrChain) const;
	/// отрисовывает линки элемента
	void DrawElementLinks(TEWnd* pwnd, CDC* pdc, 
		const RECT& rcEle, qdTriggerElementConstPtr const& ptrEle) const;
	/// Отрисовывает набор элементов
	void DrawElements(TEWnd* pwnd, CDC* pdc, 
		const CRect& rcPaint, qdTriggerChain const* ptrChain) const;
	/// Отрисовывает цепочку
	void DrawChain(TEWnd* pwnd, CDC* pdc, const CRect& rcPaint, 
		qdTriggerChain const* ptrChain) const;

private:
	static void DrawLink(HDC dc, const CPoint& pt_from, const CPoint& pt_to, 
		CPoint* ptrg, HPEN pen, HGDIOBJ brush);
private:
	/// Карандаш, которым рисуется сетка
	CPen m_NetPen;
	/// Карандаш для отрисовки ликов с признаком "авторестарт"
	CPen m_penAutoRestart;
	/// Цепочка отрисовываемая в текущий момент
	mutable qdTriggerChain const* m_ptrCurrentDrawedChain;
	/// Текст для рисуемого элемента. для оптимизации.
	mutable CString m_strDrawedEleTitle;

	/// \brief Смещение текущего триггера от его обычноного положения.
	/// используется при отрисовке всех цепочек сразу.
	mutable CSize m_szTriggerOffset;

	/// Кисть, которой рисуется рамка обычного элемента
	CBrush m_brNormalEleBrush;
	/// Кисть, которой рисуется рамка выделеннго элемента
	CBrush m_brSelEleBrush;

	/// Владелец рисуемой связи
	mutable qdTriggerElement const *m_ptrDrawedLinkOwner;

	//! \brief Прямоугольник текущей прорисовки
	//! Устанавливается в начале прорисовки и недействителен после неё
	mutable CRect paintRect_;
};
