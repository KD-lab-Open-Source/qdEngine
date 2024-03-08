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
	����� ������, � ������� �������������� ��� ������� �����.

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
	/// ��������� ������
	bool setDrawer(ITEDrawer* ptrDrawer);
	/// ���������� ������
	ITEDrawer* getDrawer() const{
		return m_ptrDrawer;
	}
	/// ��������� ��������� �������
	void setGameDisp(qdGameDispatcher* ptrDisp);
	/// ���������� ��������� �������
	qdGameDispatcher* getGameDisp() const{
		return m_ptrGameDisp;
	}
protected:
	/// ������ ������� ����� ����
	void setLTPos(CPoint const& p);
	/// ���������� ���
	CPoint const& getLTPos() const{
		return m_ptLTPos;
	}
	/// ��������� �������, � �������� �������� � ������� ������
	void setActiveChain(qdTriggerChain* pc);
	/// ���������� �������� �������
	qdTriggerChain* getActiveChain() const{
		return m_ptrActiveChain;
	}
	/// ��������� ��������, ������� �������� ��������������
	void setRectPen(HPEN hPen);
	/// ���������� ��������, ������� �������� ��������������
	HPEN getRectPen() const{
		return m_hRectPen;
	}
	void setWasMouseMoved(bool bval);
	bool getWasMouseMoved() const{
		return m_bWasMouseMoved;
	}
protected:
	/// �����, ���������� � ������ ������ ��� ���������
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
	/// ������
	ITEDrawer* m_ptrDrawer;

	/// ��������� ������ �������� ����
	CPoint m_ptLTPos;
	/// �������, � ������� � ������� ������ ��������
	qdTriggerChain* m_ptrActiveChain;
	/// ��������, ������� ������ �������������
	HPEN m_hRectPen;
	/// ��������, ������� ������ �������������, ����� ��� ������ ��������
	CPen m_redPen;
	/// ������ ���������� � ������ ������ ��� ���������
	std::auto_ptr<DrawDataKeeper> m_ptrDrawDataKeeper;

	qdGameDispatcher* m_ptrGameDisp;

	/// ���� �� �������� ����
	bool m_bWasMouseMoved;
};
