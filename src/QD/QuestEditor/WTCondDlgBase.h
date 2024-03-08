#pragma once


// WTCondDlgBase
#include "CondDlgBase.h"

/**
	������� ������ ��� ������ � "�������� ����������" ���������
 */
class WTCondDlgBase : public CondDlgBase
{
	DECLARE_DYNAMIC(WTCondDlgBase)
public:
	WTCondDlgBase(UINT nIDTemplate, DWORD id,
		UINT uiStatesComboId, UINT uiStatesScenesComboId, UINT uiSceneStaticId);
	virtual ~WTCondDlgBase();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	/// ��������� ��������� � �������
	afx_msg virtual void OnCbnSelchangeComboStates();
	/// ������������� ������ ����
	afx_msg virtual void OnCbnSelchangeComboStateScene();

	DECLARE_MESSAGE_MAP()

	/// �������� ������ ����
	virtual bool LoadScenes();
	virtual BOOL OnInitDialog();

	CCJFlatComboBox& getStateCombo(){
		return m_wndStatesCombo;
	}
	CCJFlatComboBox& getStatesSceneCombo(){
		return m_wndStatesSceneCombo;
	}

	/// ���������� ��������� �����
	qdGameScene* getScene();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	/// ����� ��� ���������
	CCJFlatComboBox m_wndStatesCombo;
	/// ����� ��� ����
	CCJFlatComboBox m_wndStatesSceneCombo;
	/// �� ����� ��� ��������� � ��������� �������
	/**
		�����, ����� ������� m_wndStatesSceneCombo � ��������� � �������
	 */
	UINT m_uiStatesComboId;
	/// �� ����� ���� � ��������� �������
	/**
		�����, ����� ������� m_wndStatesSceneCombo � ��������� � �������
	 */
	UINT m_uiStatesScenesComboId;
	/// �� �������� ������ ����� � ����� ��� ����
	UINT m_uiSceneStaticId;
};


