#pragma once


#include "basepage.h"
#include <memory>
// VideoPage dialog
class qdGameDispatcher;
class qdVideo;

/**
	�������� ������� �����
 */
class VideoPage : public CBasePage
{
	DECLARE_DYNAMIC(VideoPage)

	/// ����� ������� ��� ���������� ��������������
	std::auto_ptr<qdVideo> m_ptrVideoCopy;
	/// �������� ������
	qdVideo* m_ptrSource;
	/// ��������� �������
	qdGameDispatcher* m_ptrGameDisp;
public:
	VideoPage();
	virtual ~VideoPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_VIDEO };

	/// ��������� ��������� � ������������� �����
	void set_data(qdGameDispatcher* ptrDisp, qdVideo* ptrVideo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnApply();

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnSelectBfile();
	afx_msg void OnBnClickedBtnSelectVfile();
	afx_msg void OnEnKillfocusEditVideoFile();
	afx_msg void OnEnKillfocusEditBackFile();
	afx_msg void OnBnClickedCheckToCenter();
	afx_msg void OnBnClickedCheckFullScreen();
	/// ���������� ��� ���� ��������� � �������
	afx_msg void OnClickInCheck();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void OnEnterInPos();
	void OnEnterInVideoFile();
	void OnEnterInBackFile();

	/// ��������� �� ������� � �������� �������
	void video2wnd();
	/// ������ �� ��������� � ������
	void wnd2video();

	/// ��������� ������������ ��������� �������������� ��������� �� ������
	void EnableLayout(bool benable);
private:
	/// ��������� ����� �������
	int m_iX;
	/// ��������� ������� �������
	int m_iY;
	/// ���� ����������
	CString m_strBackFileName;
	/// ���� �����
	CString m_strVideoFileName;

    //! ����������� � ������
	BOOL m_bToCenter;
	//! �� ���� �����
	BOOL m_bFullScreen;
	//! ������ ��������
	BOOL m_bNoInterrupt;
	//! ����� �� ����� ������������� ����� ������� ����
	BOOL m_bIntroVideo;
	/// ��������� ������
	BOOL m_bEnableMusic;
};
