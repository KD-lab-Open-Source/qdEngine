#pragma once


#include <boost/scoped_ptr.hpp>
#include "BasePage.h"
// ScreenTextPropPage dialog

class qdScreenTextDispatcher;
class qdScreenTextSet;

/**
	�������� ������ � ��������
 */
class ScreenTextPropPage : public CBasePage
{
	DECLARE_DYNAMIC(ScreenTextPropPage)

public:
	ScreenTextPropPage();
	virtual ~ScreenTextPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_SCREEN_TEXT };

	/// ��������� ��������� ��������� ������� � �������������� �����
	void set_data(qdScreenTextDispatcher* pstdisp, qdScreenTextSet* pset);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* );
	afx_msg void OnRadioBtnClick();

	DECLARE_MESSAGE_MAP();

	/// ������ �� ��������� � �������������� ������
	void wnd2set();
	/// �������� ������� � �������� �������
	void set2wnd();

	/// ������ �� ������. ������ �� �����
	int getHorzAlignmentNum(int alignment);
	/// ������ �� ������. ������ �� �����
	int getVertAlignmentNum(int alignment);
	/// ������ �� ������. ������ �� �����
	void setAlignment();
private:
	/// �������������� ����� �������
	qdScreenTextSet*		pScreenTextSet_;
	/// ��������� ������� ������
	qdScreenTextDispatcher*	pScreenTextDisp_;

	/// �������������� ������������
	BOOL iHorzAlign_;
	/// ������������ ������������
	BOOL iVertAlign_;
	/// ������� ������ ����
	int iLeft_;
	/// ������� �������� ����
	int iTop_;
	/// ������
	int iSX_;
	/// ������
	int iSY_;
	/// ��������� �����, ��� ����� ���� �������� ���
	boost::scoped_ptr<qdScreenTextSet> screenTextSetCopy_;
	//! ���������� ������(�� ����������� ��� �� ���������)
	BOOL iOrientation_;
};
