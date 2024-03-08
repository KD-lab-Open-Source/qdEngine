/********************************************************************
	created:	2003/05/21
	created:	21:5:2003   17:12
	filename: 	d:\Projects\Quest\QuestEditor\TEBaseWorkMode.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEBaseWorkMode
	file ext:	h
	Powerd by:	����� ��� ������� ����������� � ��������� ��������� of Scmorr
	
	purpose:	���������� ��������� ������ ������ ��� ��������� ���������
*********************************************************************/
#pragma once

class qdTriggerChain;
class qdTriggerElement;
class qdTriggerLink;
class TEWnd;
class TEBaseWorkModeDrawerBase;

class MakeLinkActive;
class MoveTEElementPos;
class AddLink;

#include "tesinglechainworkmode.h"
#include "teundomanager.h"
#include <memory>
#include <map>

/**
	����� ��, � ������� ��������� ������������� �������. 
 */
class TEBaseWorkMode :
	public TESingleChainWorkMode
{
	/// ��� ������� undo ����������
	typedef std::map<qdTriggerChain*, TEUndoManager> UndoBuffers;
	typedef TESingleChainWorkMode Base;
	/// ���������� ���-����� ��� ������ �����
	enum eLinkAnchorTest {
		LAT_NONE,///< ������
		LAT_BEGIN,///< �������
		LAT_END///< ������
	};
	/// � ����� �� ��������, ���������� ����� ���������� ������
	enum eBetweenPlotsLayout{
		BPL_NOWERE,///< �� � �����
		BPL_VERT,///< � ������������(������ ��� ����� �� ��������)
		BPL_HORZ,///< � ��������������(����� ��� ������)
		BPL_VERT_HORZ,///< �� ��������� ����� ��� ����
	};
	friend class MakeLinkActive;
	friend class MoveTEElementPos;
	friend class AddLink;
public:
	TEBaseWorkMode(SelectionManager& selectionManger);
	virtual ~TEBaseWorkMode(void);

	void OnLButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point);
	void OnLButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point);
	void OnLButtonDblClk(TEWnd* pwnd, UINT nFlags, CPoint point);

	void OnRButtonDown(TEWnd* pwnd, UINT nFlags, CPoint point);
	void OnRButtonUp(TEWnd* pwnd, UINT nFlags, CPoint point);

	void OnMouseMove(TEWnd* pwnd, UINT nFlags, CPoint point);

	void OnKeyDown(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyUp(TEWnd* pwnd, UINT nChar, UINT nRepCnt, UINT nFlags);

	void OnPaint(TEWnd* pwnd, CDC* pdc, CRect const& rcPaint);

	BOOL OnCommand(TEWnd* pwnd, WPARAM wParam, LPARAM lParam);

	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	bool OnStart();
	bool OnEnd();

	BOOL OnDropData(TEWnd* pwnd, void* pData, const CPoint& point);
	bool CanDrop(TEWnd* pwnd, void* pData, const CPoint& point);

	virtual bool OnCommandUpdateUI(TEWnd *pwnd, CCmdUI* pCmdUI);
	virtual void OnSettingsChange();
	virtual void clearUndobuffer();

	bool setDrawer(TEBaseWorkModeDrawerBase* ptrDrawer);
	qdTriggerElementConstPtr const& getHilitedLinksElement() const{
		return m_ptrHilitedLinksElement;
	}
	virtual void setChain(qdTriggerChain* pchain);
protected:
	void setHilitedLinksElement(qdTriggerElementConstPtr pele);
	void setMouseMoved(bool bVal);
	bool getMouseMoved() const{
		return m_bMouseMoved;
	}
	void setLinkOwner(qdTriggerElementPtr pele);
	qdTriggerElementPtr getLinkOwner() const{
		return m_ptrLinkOwner;
	}
	void setActiveLink(qdTriggerLink* plink);
	qdTriggerLink* getActiveLink() const{
		return m_ptrActiveLink;
	}
	void setObjPos(CPoint const& point);
	CPoint const& getObjPos() const{
		return m_ptObjPos;
	}
	void setDrawedLinkType(int newType);
	int getDrawedLinkType() const{
		return m_iDrawedLinkType;
	}
	void setLATest(eLinkAnchorTest eVal);
	eLinkAnchorTest getLATest() const{
		return m_eLATest;
	}
	void setActiveLinkType(int iVal);
	int getActiveLinkType() const{
		return m_iActiveLinkType;
	}
	void setBetweenPlotsLayout(eBetweenPlotsLayout eVal);
	eBetweenPlotsLayout getBetweenPlotsLayout() const{
		return m_eBetweenPlotsLayout;
	}
	//��������� ������ �� ������ bound �� ����� ������������� �����
	CSize getLinkOffset(const CPoint& pFrom, const CPoint& ptTo, const RECT& bound);
	//��������� ������� �� ������ ����� ������������� �����
	void calcLinkOffsets(qdTriggerElementPtr parent, 
		qdTriggerElementPtr child, CPoint const& ptFrom, CPoint const& ptTo);

	eLinkAnchorTest find_link_anchor(qdTriggerElementPtr powner, 
		qdTriggerLink* plink, const CPoint& p);

	void activateLink(qdTriggerLink* plink, qdTriggerElementPtr pLinkOwner);
	void OnTeLinkDelete(TEWnd* pwnd);
	void OnTeTgrEleDelete(TEWnd* pwnd);
	void OnTeMakeActive(TEWnd* pwnd);
	void OnClearStarts(TEWnd* pwnd);
	void OnTeLinkType(UINT id, TEWnd* pwnd);
	void OnAutoRestart(TEWnd* pwnd);
	void OnInsertColumns(TEWnd* pwnd);
	void OnInsertRows(TEWnd* pwnd);
	void OnDeleteColumn(TEWnd* pwnd);
	void OnDeleteRow(TEWnd* pwnd);
	void OnTEHiliteLinks(TEWnd* pwnd);
	void OnTELinkSelectedTriggers(TEWnd* pwnd);

	bool CanPlugLink(qdTriggerElementPtr ptrTestEle, qdTriggerChain const* pchain) const;

	void ModifyMenu(CMenu* pmenu, UINT menuResId, TEWnd* pwnd);

	bool TestForBetweenPlots(TEWnd* pwnd, CPoint p);
	void clearActive(qdTriggerChain* pc);

	enum eUndoOperation {UO_NOT, UO_UNDO};
	void moveLinkEnd(CPoint const& point);
	virtual void removeLinks(qdTriggerChain* pc, 
							qdTriggerElementPtr pe, 
							TEUndoManager::Bunch& undoBunch,
							TEUndoManager::Bunch& redoBunch);
	bool removeLink(qdTriggerChain* pc, 
					qdTriggerElementPtr ptrLinkOwner, 
					qdTriggerLink* pLink, TEUndoManager::Action& aUndo,
					TEUndoManager::Action& aRedo);
	void OnEditUndo(TEWnd* pwnd);
	void OnEditRedo(TEWnd* pwnd);

	void moveSelection(TEWnd* pwnd, CPoint const& point);
	bool moveDraggedTitles(TEWnd* pwnd, CPoint const& point) const;
	void drawDraggedTitles(TEWnd* pwnd) const;

	void deleteTrigger(TEWnd* pwnd, qdTriggerChain* chain, 
							qdTriggerElementPtr const& element, 
							TEUndoManager::Bunch& undoBunch,
							TEUndoManager::Bunch& redoBunch);
	void deleteSelection(TEWnd* pwnd, TEUndoManager::Bunch& undoBunch,
		TEUndoManager::Bunch& redoBunch);
	void deleteSelection(TEWnd* pwnd);

private:
	std::pair<qdNamedObject*, qdNamedObject*> getDraggedData(void* pData) const;
	TEUndoManager& getUndoManager(qdTriggerChain* pchain);
private:
	/// ���� �� �������� ����
	bool m_bMouseMoved;
	/// �������, ��������� ��������� �����
	qdTriggerElementPtr m_ptrLinkOwner;
	/// �������� ����
	qdTriggerLink *m_ptrActiveLink;
	/// ������� �������
	CPoint m_ptObjPos;
	/// ��� �����, ������� �� �����. -1 - ������, ��� ���� ��������� ������
	int m_iDrawedLinkType;
	/// ���� ������ ��� ����� ����� �� ����
	eLinkAnchorTest m_eLATest;
	/// ��� ����������� �����
	int m_iActiveLinkType;
	/// �������, ������� �������� ������ ��� ����������
	std::auto_ptr<DataKeeper> m_ptrDataKeeper;

	/// ����� ������������ ���������
	TEBaseWorkModeDrawerBase *m_ptrDrawer;

	/// ���� �������� ������ ������� �� �� ��������, �� ����.
	eBetweenPlotsLayout m_eBetweenPlotsLayout;

	/// �������, ��� ����� � �������� � �� �������� ������ ����. ������
	qdTriggerElementConstPtr m_ptrHilitedLinksElement;
	/// undo-�������� ��� ������� ��������
	TEUndoManager::Action m_undo;
	/// ������ undo-�������.��� ������ ������� ����
	UndoBuffers m_undoBuffers;
	/// ���������� �� ����������� ����, ����� �������� ����
	bool m_bShowMenuOnLinkCreate;
};
