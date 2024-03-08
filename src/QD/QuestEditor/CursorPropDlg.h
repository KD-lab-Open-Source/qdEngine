#pragma once


#include "RqstngStateEdit.h"
#include <memory>
#include "afxwin.h"
// CursorPropDlg dialog
class qdGameDispatcher;
class qdGameObjectStateStatic;
class qdGameObjectMouse;

/// ������ ����������� ������������� �������� �������
class CursorPropDlg : public CDialog, public IRequestingStateEdit
{
	DECLARE_DYNAMIC(CursorPropDlg)

public:
	CursorPropDlg(HWND hOwnedWnd = NULL);   // standard constructor
	virtual ~CursorPropDlg();

// Dialog Data
	enum { IDD = IDD_DLG_CURSOR_PROP };

	/// ��������� ��������� � ���������� ������, � ������� ��������
	bool set_data(qdGameDispatcher* ptrDisp, qdGameObjectStateStatic* ptrCursor);

	// ���������� ���������� IRequestingStateEdit


	/// ���������� �������� ������(���������, ������� ��� ���������)
	qdGameObjectState* GetState();
	/// ���������� �������� �������
	void EndStackState();
	/// ���������� �������������� �������(�� ��������������)
	void EndEditStateDims();
	/// ���������� �������������� �� ��� ������(�� ��������������)
	void EndEditCoordAnimation();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	/// ��������������� �� ��������� ����� 
	virtual void OnCancel();
	/// ������� �������� � ������
	virtual void OnOK();
	afx_msg void OnDeltaposSpinMoveSpeed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMoveRadius(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnStackState();
	DECLARE_MESSAGE_MAP()

	/// ��������� ��������� �� ������� � �������� �������
	void cursor2wnd();
	/// �� ��������� ������� ��������� ������ � ������
	void wnd2cursor();

	/// ��������� �������� �� ����������� �������� ��������
	void LoadAnimations();
private:
	/// ��������� �������
	qdGameDispatcher* m_ptrGameDisp;
	/// �������� ������
	qdGameObjectStateStatic* m_ptrSourceCursor;
	/// ��������� �����. 
	std::auto_ptr<qdGameObjectStateStatic> m_ptrCopyCursor;

	/// ����, ������� ������������ �������� �������
	HWND m_hOwnedWnd;

	/// ������ ��������� ��������
	CCJFlatComboBox m_wndAnim;
	/// ����������� ��������
	BOOL m_bLoop;
	/// ������������� �������������
	BOOL m_bFlipHorz;
	/// ������������� �����������
	BOOL m_bFlipVert;
	/// ������ ��� ������������ ��������
	BOOL m_bForDynObj;
	/// ������ ��� ���
	BOOL m_bForZones;
	/// ������ ��� �������� ����
	BOOL m_bForMainMenu;
	/// ������ ��� �������������� ����������
	BOOL m_bForIngameInterf;
	/// ��� ���� m_ptrSourceCursor->rnd_move_radius()
	float moveRadius_;
	/// ��� ���� m_ptrSourceCursor->rnd_move_speed()
	float moveSpeed_;
	/// ��� �������� � ���������
	BOOL forObjectsInInventory_;
};
