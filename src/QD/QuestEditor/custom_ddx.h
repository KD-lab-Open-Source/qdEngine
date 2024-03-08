/********************************************************************
	created:	2002/11/25
	created:	25:11:2002   17:12
	file ext:	h
	Powered by:	�����
	
	purpose:	���� ���������� ���������� �� ��������� � ��������
*********************************************************************/
#ifndef _CUSTOM_DDX_
#define _CUSTOM_DDX_
#pragma once

namespace custom_ddx{

/// ������ �� �������� ������������
struct null_preproc{
	//���������� true, ���� ��������� ������ ���� ����������
	//false - � ��������� ������
	bool operator()(CDataExchange*pDX, int nIDC){
		return true;
	}
};

/// ���������� ��� �����
struct int_preproc {
	bool operator()(CDataExchange* pDX, int nIDC){
		if (!pDX->m_bSaveAndValidate) 
			return true;

		HWND hw = pDX->PrepareEditCtrl(nIDC);
		TCHAR szT[64];
		GetWindowText(hw, szT, sizeof(szT)/sizeof(TCHAR));
		if (*szT == _T('\0')||*szT==_T('-')) 
			return false;
		return true;
	}
};

/// ������������ ��� ����� ����� ������ �� ����������� "int"
template<class _Data>
class ddx_preprocessor : public null_preproc{
};

/// ������������ ��� �����
template<>
class ddx_preprocessor<int> : public int_preproc{
};

/// ������� ���������� ��� ����� ����� ������
/** 
	�������� ��������� ������������ �� ����.
	��� ��� ���� ��� � ���� � ���� ����������
	������������� ddx_preprocessor
*/
template<class _Data>
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, _Data& d){

	if (!ddx_preprocessor<_Data>()(pDX, nIDC))
	{
		pDX->Fail();
		//��������������, ��� d ��� ����� �����-�� ��������
		return;
	}
	::DDX_Text(pDX, nIDC, d);
}

/// ������� ���������� ��� ����� ����� ������
/**
	��������� ���� ����������
 */
template<class _Data, class _PreProc>
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, _Data& d,_PreProc& pp){

	if (!pp(pDX, nIDC))
	{
		pDX->Fail();
		//��������������, ��� d ��� ����� �����-�� ��������
		return;
	}
	::DDX_Text(pDX, nIDC, d);
}

}//end of custom_ddx
#endif