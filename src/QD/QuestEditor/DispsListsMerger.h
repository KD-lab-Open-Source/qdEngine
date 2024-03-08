// DispsListsMerger.h: interface for the DispsListsMerger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPSLISTSMERGER_H__DA1909D8_613A_497C_BB04_8BF692D6BEC9__INCLUDED_)
#define AFX_DISPSLISTSMERGER_H__DA1909D8_613A_497C_BB04_8BF692D6BEC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>

typedef std::set<CString> CStringSet_t;

/**
	�������� ����� �������� �� ���������� al � ptrSet. ��������� �� ��, ���
	� std::set ������ ������� ����������� ������ ���� ���
 */
template<class __X>
inline void copy_animation(const __X& al, CStringSet_t* ptrSet)
{
	__X::const_iterator _itr = al.begin(), _end = al.end();
	for(; _itr != _end; ++_itr)
		ptrSet->insert((*_itr)->name());
}

class qdGameDispatcherBase;

/**
	���� ������� ���������� ������, ��������, �������� ����� � ������ ��������
	�� ��������� �������. �������� ��������� ������������� �������, ������� ��� �� 
	����������
 */
template<class __X>
std::auto_ptr<CStringSet_t> merge_lists(qdGameDispatcherBase* ptrDispBase1st, 
						qdGameDispatcherBase* ptrDispBaseLast,
						__X (qdGameDispatcherBase::*f)() const)
{
	std::auto_ptr<CStringSet_t> ptr(new CStringSet_t);

	if (ptrDispBaseLast) {
		copy_animation((ptrDispBaseLast->*f)(), ptr.get());
		
		if(ptrDispBaseLast == ptrDispBase1st)
			return ptr;
		
		ptrDispBaseLast = static_cast<qdGameDispatcherBase*>(ptrDispBaseLast->owner());
		
		copy_animation((ptrDispBaseLast->*f)(), ptr.get());
		
		if(ptrDispBaseLast == ptrDispBase1st)
			return ptr;
	}
	
	copy_animation((ptrDispBase1st->*f)(), ptr.get());
	
	return ptr;
}

#endif //!defined(AFX_DISPSLISTSMERGER_H__DA1909D8_613A_497C_BB04_8BF692D6BEC9__INCLUDED_)
