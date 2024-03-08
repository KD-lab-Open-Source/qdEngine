#pragma once


// ObjOnPersonageWayCond dialog
#include "CondDlgBase.h"
#include "afxwin.h"

class qdGameScene;
class qdNamedObject;

/**
	������ �� ���� ���������
 */
class ObjOnPersonageWayCond : public CondDlgBase
{
	DECLARE_DYNAMIC(ObjOnPersonageWayCond)

public:
	ObjOnPersonageWayCond(DWORD condId, CWnd* pParent = NULL);   // standard constructor
	virtual ~ObjOnPersonageWayCond();

// Dialog Data
	enum { IDD = IDD_DLG_COND_OBJ_ON_PERSONAGE_WAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	/// ��������� ������������� �������
	void LoadObjects();
	/// ��������� ���������
	void LoadPersonages();
	/// ��������� �����
	void LoadScenes();
	/// ���������� ��������� �����
	qdGameScene* getScene();
	/// ���������� ���������� �����
	qdNamedObject* getPersonage();
	/// ���������� ��������� ���� ������
	qdNamedObject* getObject();
	/// ���������� ��������� � ����� ������
	template<class ObjType>
		ObjType* getObjFromCombo(CCJFlatComboBox& box);

	/// ������������ ������� � ����������
	afx_msg void OnCbnSelchangeComboScenes();
	/// ������ �������� � �������
	/**
		���������� ��������� ������� �� ������ ����. ��������. � ����������� ����������
		���������
	 */
	afx_msg void OnCbnSelchangeComboObject();
	afx_msg void OnCbnSelchangeComboObject2();

	virtual bool IsCondDataComplete();

	/// ��������� ����������� ���������� ���������
	void setPrevPersonage(qdNamedObject* p);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
private:
	/**
		��������� ������� � ����� ��������� ������
	 */
	template< class Filter>
	class ObjTypeToCombo
	{
		CCJFlatComboBox& m_box;
		Filter const& m_filter;
	public:
		ObjTypeToCombo(CCJFlatComboBox& box, Filter const& f);
		void operator()(qdNamedObject* pobj);
	};
private:
	CCJFlatComboBox m_wndScenes;
	//! ������ ����������
	CCJFlatComboBox m_wndPers;
	/// ������ ������������� ��������, �� ����������� ����� ���������
	CCJFlatComboBox m_wndObjs;
	
	/// ���������� ��������� ��������
	qdNamedObject* m_pPrevPers;
	//! ������������ ���������� ����� ���������
	float m_fMaxDist;
public:
	afx_msg void OnDeltaposSpinMaxDist(NMHDR *pNMHDR, LRESULT *pResult);
};
