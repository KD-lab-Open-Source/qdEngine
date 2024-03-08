#pragma once

#include "InterfElePosPage.h"
#include <memory>
// InterfBtnProp dialog

#include "GameOptionRepresMngr.h"
#include "afxwin.h"

class qdInterfaceButton;
class qdInterfaceDispatcher;

/// �������� ������ ����������
class InterfBtnProp : public InterfElePosPage
{
	DECLARE_DYNAMIC(InterfBtnProp)

public:
	InterfBtnProp();
	virtual ~InterfBtnProp();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_INTRF_BTN_PROP };

	/// ��������� ������ � ��������� ���������
	void set_data(qdInterfaceDispatcher* pd, qdInterfaceButton* pbtn);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnCbnSelchangeComboOptions();

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	/// ��������� ���������
	virtual BOOL OnApply();
	/// �������� ���������
	virtual void OnCancel();
	/// ��������� ������ ����� ����
	bool LoadOptions();
	/// �������� � ��������� ������ ����� ����
	GameOptionRepresMngr const &getGameOptions() const{
		return m_gameOptions;
	}
	/// �������������� ��������� � ���������� ����� ����
	qdInterfaceElement::option_ID_t getOptionId() const;
	
	/// ��������� ������ � �������� �������
	void btn2wnd();
	/// ������ �� ��������� ������� � ��������� ������
	void wnd2btn();
private:
	/// �������� ������ ����� ����
	GameOptionRepresMngr const m_gameOptions;
	/// ������� ���������� ������ ����� ����
	CCJFlatComboBox m_wndOptions;

	/// �������� ������
	qdInterfaceButton* m_pSourceBtn;
	/// ��������� �����
	std::auto_ptr<qdInterfaceButton> m_pBackBtn;
};
