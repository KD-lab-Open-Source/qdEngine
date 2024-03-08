/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   12:23
	filename: 	d:\Projects\Quest\QuestEditor\TESingleChainWorkMode.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TESingleChainWorkMode
	file ext:	h
	Powerd by:	�����  ��� ������� ����������� � ��������� ��������� of Scmorr
	
	purpose:	������� ������ ��� �������, ������� �������� � ����� ��������
*********************************************************************/
#pragma once
#include "teworkmodebase.h"
#include "IDrawData.h"
#include "qd_trigger_element.h"

#include "SelectionManager.h"

class TEWnd;

/**
	�� ��� ������ � ����� ��������
 */
class TESingleChainWorkMode :public TEWorkModeBase
{
protected:
	/**
		�������� ������ ��� �����������
	 */
	class DataKeeper : public IDrawData
	{
		/// ������� ��� ���������
		class qdTriggerChain const* m_ptrChain;
	public:
		DataKeeper(qdTriggerChain const* pchain) : m_ptrChain(pchain)
		{}
		/// ��������� �������
		void setTriggerChain(qdTriggerChain const* pchain){
			m_ptrChain = pchain;
		}
		std::pair<qdTriggerChain const* const*, int> getDrawedChains() const{
			return std::make_pair(&m_ptrChain, 1);
		}
	};
protected:
	TESingleChainWorkMode(SelectionManager& selectionManger);
	~TESingleChainWorkMode(void);

public:
	/**
		������������ ����� 2 �������
		IDC_TE_SHOW_CONDITION � IDC_TE_PROPETIES. � ��������� ������� ���������� FALSE
	 */
	BOOL OnCommand(TEWnd* pwnd, WPARAM wParam, LPARAM lParam);
	/**
		���� ���� ������������, �� ��������� ������, ���� ����������, 
		�� ���������� ������
	 */
	void OnShowWindow(TEWnd* pwnd, BOOL bShow, UINT nStatus);

	/// ��������� �������
	virtual void setChain(qdTriggerChain* pchain);
	/// ���������� �������
	virtual qdTriggerChain* getChain() const{
		return m_ptrChain;
	}
protected:

	/**
		���������� �������� �������, ����������� � ��������, ��������� ���������
		���� ������ ������������, �� ����������� �������, �� ������� ��������
	 */
	void OnTeProperties(TEWnd* pwnd);
	/**
		���������� ������� ��� �������, ����������� � ��������, ��������� ���������
		���� ������ ������������, �� ����������� �������, �� ������� ��������
	 */
	void OnTeShowCondition(TEWnd* pwnd);
	/**
		������� ������� ��� �����. ���� �� �� �������� � ���������� ���������,
		�� ������ ����� �������.
		���������� ������ � ����� point
	 */
	virtual void trackToolTip(TEWnd* pwnd, CPoint point);
	/// ���������� selectionManager_
	SelectionManager& getSelectionManager();
	/// ���������� ����������� selectionManager_
	SelectionManager const& getSelectionManager() const;

	/// ��������� ������� �������
	void setBaseElement(qdTriggerElementPtr const& element);
	/// ���������� ������� �������
	qdTriggerElementPtr const& getBaseElement() const;

protected:
	/**
		��� ����� ���������� ���������� ������� ������� � NULL.
		�����, ����� �� �������� ��� ������ ������
	 */
	class BaseElementGuard{
		TESingleChainWorkMode* workMode_;
	public:
		BaseElementGuard(TESingleChainWorkMode* workMode):workMode_(workMode){}
		~BaseElementGuard(){
			workMode_->setBaseElement(NULL);
		}
	};
private:
	/// �������, � ������� ����������� ������
	qdTriggerChain *m_ptrChain;
	/// ������ ����������� ������������ ���������
	SelectionManager& selectionManager_;
	//! �������, �� ������� �������� ������
	qdTriggerElementPtr baseElement_;
};
