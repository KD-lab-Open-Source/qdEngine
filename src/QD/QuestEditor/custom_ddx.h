/********************************************************************
	created:	2002/11/25
	created:	25:11:2002   17:12
	file ext:	h
	Powered by:	Илюха
	
	purpose:	Свой обработчки информации из контролов в диалогах
*********************************************************************/
#ifndef _CUSTOM_DDX_
#define _CUSTOM_DDX_
#pragma once

namespace custom_ddx{

/// Ничего не делающий препроцессор
struct null_preproc{
	//возвращаем true, если обработка должна быть продолжена
	//false - в противном случае
	bool operator()(CDataExchange*pDX, int nIDC){
		return true;
	}
};

/// Обработчик для интов
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

/// Препроцессор для любых типов данных за исключением "int"
template<class _Data>
class ddx_preprocessor : public null_preproc{
};

/// Препроцессор для интов
template<>
class ddx_preprocessor<int> : public int_preproc{
};

/// Функция обработчик для любых типов данных
/** 
	пытается вычислить препроцессор по типу.
	все что надо это у себя в коде определить
	специализацию ddx_preprocessor
*/
template<class _Data>
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, _Data& d){

	if (!ddx_preprocessor<_Data>()(pDX, nIDC))
	{
		pDX->Fail();
		//предполагается, что d уже имеет какое-то значение
		return;
	}
	::DDX_Text(pDX, nIDC, d);
}

/// Функция обработчик для любых типов данных
/**
	Принимает свой обработчик
 */
template<class _Data, class _PreProc>
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, _Data& d,_PreProc& pp){

	if (!pp(pDX, nIDC))
	{
		pDX->Fail();
		//предполагается, что d уже имеет какое-то значение
		return;
	}
	::DDX_Text(pDX, nIDC, d);
}

}//end of custom_ddx
#endif