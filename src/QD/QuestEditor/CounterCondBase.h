#pragma once
#include "conddlgbase.h"

class qdGameDispatcher;

/**
	������� ����� ��� �������� ������� ��������� �� ����������
 */
class CounterCondBase :
	public CondDlgBase
{
	DECLARE_DYNAMIC(CounterCondBase);
protected:
	DECLARE_MESSAGE_MAP();
public:
	CounterCondBase(UINT nIDTemplate, DWORD condID, UINT counterComboId);
	~CounterCondBase(void);
protected:

	UINT getCounterComboId()const{
		return m_uiCounterComboId;
	}
	void setGameDispatcher(qdGameDispatcher* ptrGameDisp){
		m_ptrGameDisp = ptrGameDisp;
	}
	qdGameDispatcher* getGameDispatcher() const{
		return m_ptrGameDisp;
	}

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	/// ���������, ����� �������� ������� CBN_SELCHANGE � m_wndCounters
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	/// ��������� ������ ���������
	virtual bool loadCounters(CCJFlatComboBox& box);
	/// ����������� ������� � �������
	virtual void onSelChangeInCounterCombo();

	/// ������� ��������� ������� �� ����������
	qdCounter* getCounterFromCombo(CCJFlatComboBox& box);
protected:
	/**
		�������, ����������� ������� � ���������
	 */
	class AddCounterToCombo
	{
		CCJFlatComboBox& m_wndCombo;
	public:
		AddCounterToCombo(CCJFlatComboBox& box);
		void operator()(qdCounter* pc);
	};
private:
	/// ������������� ����� � ��������� �������. 
	/**
		�����, ����� ��������� m_wndCounters � ���� �������
	 */
	UINT m_uiCounterComboId;
	CCJFlatComboBox m_wndCounters;
	qdGameDispatcher* m_ptrGameDisp;
};
