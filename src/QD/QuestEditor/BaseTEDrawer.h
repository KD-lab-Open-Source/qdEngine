/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   15:58
	filename: 	d:\Projects\Quest\QuestEditor\BaseTEDrawer.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	BaseTEDrawer
	file ext:	h
	Powerd by:	����� ��� ������� ����������� � ��������� ��������� of Scmorr
	
	purpose:	���������� ������� ������������. ��������� ���������, ������, �����.
*********************************************************************/

#pragma once
#include "TEDrawerBase.h"
#include "IDrawData.h"
#include "qd_trigger_element.h"

class TEWnd;
class qdTriggerChain;

/**
	���������� ������� ������������. ��������� ���������, ������, �����.
 */
class BaseTEDrawer : public TEDrawerBase
{
public:
	BaseTEDrawer(void);
	virtual ~BaseTEDrawer(void);
	/// �������� ����������� �����
	void draw(CDC* pdc, TEWnd* pwnd, IDrawData* pdd, CRect const& rcPaint) const;
	void drawDraggedRect(HDC dc, CRect const&r, HPEN hpen) const;
	/// ������������ �����
	void drawNet(CDC* pdc, TEWnd* pwnd, CRect const& rcPaint) const;
protected:
	/// �� �������� ���������� ���� ���� ��� �������
	virtual HBRUSH	SelectEleBackBrush(qdTriggerElementConstPtr const& pele) const;
	/// �� ����� ���������� ����� ��� �������� ������������
	virtual HBRUSH	SelectLinkBrColor(qdTriggerLink const* plink) const;
	/// �� ����� ���������� �������� ��� ��������� �����
	virtual HPEN	SelectLinkPenColor(qdTriggerLink const* plink) const;

protected://������� � �������
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
	/// ������������ �������
	void DrawElement(TEWnd* pwnd, CDC* pdc, 
		qdTriggerElementConstPtr const& ptrEle) const;
	/// ������������ ����
	void DrawLink(TEWnd* pwnd, HDC dc, CPoint pt_from, 
				CPoint pt_to, qdTriggerLink const* pl) const;
	/// ������������ ����� � �������
	void DrawLinks(TEWnd* pwnd, CDC* pdc, qdTriggerChain const* ptrChain) const;
	/// ������������ ����� ��������
	void DrawElementLinks(TEWnd* pwnd, CDC* pdc, 
		const RECT& rcEle, qdTriggerElementConstPtr const& ptrEle) const;
	/// ������������ ����� ���������
	void DrawElements(TEWnd* pwnd, CDC* pdc, 
		const CRect& rcPaint, qdTriggerChain const* ptrChain) const;
	/// ������������ �������
	void DrawChain(TEWnd* pwnd, CDC* pdc, const CRect& rcPaint, 
		qdTriggerChain const* ptrChain) const;

private:
	static void DrawLink(HDC dc, const CPoint& pt_from, const CPoint& pt_to, 
		CPoint* ptrg, HPEN pen, HGDIOBJ brush);
private:
	/// ��������, ������� �������� �����
	CPen m_NetPen;
	/// �������� ��� ��������� ����� � ��������� "�����������"
	CPen m_penAutoRestart;
	/// ������� �������������� � ������� ������
	mutable qdTriggerChain const* m_ptrCurrentDrawedChain;
	/// ����� ��� ��������� ��������. ��� �����������.
	mutable CString m_strDrawedEleTitle;

	/// \brief �������� �������� �������� �� ��� ���������� ���������.
	/// ������������ ��� ��������� ���� ������� �����.
	mutable CSize m_szTriggerOffset;

	/// �����, ������� �������� ����� �������� ��������
	CBrush m_brNormalEleBrush;
	/// �����, ������� �������� ����� ���������� ��������
	CBrush m_brSelEleBrush;

	/// �������� �������� �����
	mutable qdTriggerElement const *m_ptrDrawedLinkOwner;

	//! \brief ������������� ������� ����������
	//! ��������������� � ������ ���������� � �������������� ����� ��
	mutable CRect paintRect_;
};
