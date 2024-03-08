#pragma once
#include "afxcmn.h"

#include "qd_textdb.h"
// TextIDSelectorDLG dialog

/**
	����� ��������� ������� ������������� ������ �� ��������� ���� ������.
	��������� �������������� ���������� ����� ������ TextIdSelector::run
 */
class TextIDSelectorDLG : public CDialog
{
	DECLARE_DYNAMIC(TextIDSelectorDLG)

public:
	TextIDSelectorDLG(CWnd* pParent = NULL);   // standard constructor
	virtual ~TextIDSelectorDLG();

// Dialog Data
	enum { IDD = IDD_DLG_SELECT_TEXT_ID };

	/// ��������� ��������� ���� ������
	void setTextDB(qdTextDB const* ptrTextDB);
	/// ������������� ������� ������������� ������
	void setCurrentId(CString const& textId);
	CString const getSelectedId() const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnGetdispinfoTree(NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	/// ��������� ��� �������� �������������� �����
	bool loadRootIds();
	/// ��������� ������ �������������� � ������ ��� ���������� ��������
	bool loadIds(qdTextDB::IdList const& idList, HTREEITEM hParent);
	/// ���������� ����� ��������������
	/**
		���������� ������, � ������� ����� '.' ����������� ��� ���� � ����� 
		���� ��� ��������� � ������ �� ������� �� ������ ���� � ����� ������
	 */
	CString const getIDMask(HTREEITEM hItem) const;

	/// \brief ������������� ������ �� ��� ��� ���� �� ����� ����� ����, 
	///  ����� ������������� currentId_
	void showCurrentId() ;
	/// ������� ����� �������� ����� ����, ����� �������� ����� text
	HTREEITEM findItemByText(CString const& text, HTREEITEM hParent) const;
	/// ��������� ������������ ����, ��������������� ��������� ������
	void insertNOTEXTItem();
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	/// �������� �����, ��������������� ���������� ��������������
	/**
		������� �� ��������� MFC �� �������� ������� ������ ������
	 */
	CString textPreview_;
	/// ������ ���������������
	CTreeCtrl tree_;

	/// ��������� ����
	qdTextDB const* ptrTextDB_;
	//! �������������, ������� ������
	CString currentId_;
};
