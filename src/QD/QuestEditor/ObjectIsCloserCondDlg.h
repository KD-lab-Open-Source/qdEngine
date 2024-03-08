#pragma once

#include "CondDlgBase.h"
#include "afxwin.h"

// ObjectIsCloserCondDlg dialog

/**
	������ ����� � ������� ��������, ��� �� �������
 */
class ObjectIsCloserCondDlg : public CondDlgBase
{
	DECLARE_DYNAMIC(ObjectIsCloserCondDlg)

public:
	ObjectIsCloserCondDlg(DWORD id);   // standard constructor
	virtual ~ObjectIsCloserCondDlg();

// Dialog Data
	enum { IDD = IDD_DLG_COND_OBJECT_IS_CLOSER };

	bool IsCondDataComplete();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	/// ��������� ������� ��������
	afx_msg void OnCbnSelchangeComboScenes();
	/// ������ ����� � �������
	afx_msg void OnCbnSelchangeComboObject();
	/// ������ ������ � �������
	afx_msg void OnCbnSelchangeComboObject2();
	/// ������ ������ � �������
	afx_msg void OnCbnSelchangeComboObject3();

	DECLARE_MESSAGE_MAP();
	/// ���������� ��������� �����
	class qdGameScene* get_scene();

	/// ��������� �������, �������� �� �������� ���������
	void loadObjects(CCJFlatComboBox& combo, 
					qdNamedObject const* excl, 
					qdNamedObject const* excl2);
	/// �������� ��������� � ����� ������
	qdNamedObject* get_obj(CCJFlatComboBox& combo);
private:

	CCJFlatComboBox scenes_;
	CCJFlatComboBox firstObjects_;
	CCJFlatComboBox candidateObjects1_;
	CCJFlatComboBox candidateObjects2_;
};
