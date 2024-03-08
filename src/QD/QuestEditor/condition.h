/********************************************************************
	created:	2002/10/22
	created:	22:10:2002   17:09
	filename: 	d:\projects\quest\questeditor\condition.h
	file path:	d:\projects\quest\questeditor
	file base:	condition
	file ext:	h
	author:		�����
	
	purpose:	��������� ��� CONDITION ����� ����������� ��������� �������
*********************************************************************/
#ifndef _CONDITION_HEADER_
#define _CONDITION_HEADER_

/*!
	���������, ����������� �������: ��� � ��������
*/
typedef struct tagCONDITION{
	LPCTSTR	strName;
	DWORD		id;
	qdCondition::ConditionType type() const{
		return static_cast<qdCondition::ConditionType>(LOWORD(id));
	}
	bool inverse() const {
		return static_cast<bool>(HIWORD(id));
	}
}CONDITION;

extern CONDITION g_arrConds[];
extern int g_iCondsSize;

//! ��������� ������� � ��������� ���
CString GetConditionText(const qdCondition* ptrCond);

//! ��������� ������������� ��� �������������� �������
#define MAKE_INVERS(xxx) MAKELONG((xxx), true)
//! ��������� ������������� ��� �������
#define MAKE_NORMAL(xxx) MAKELONG((xxx), false)

//! �� �������������� ������� ���������� ��� �������
inline qdCondition::ConditionType get_type(DWORD xxx) {
	return static_cast<qdCondition::ConditionType>(LOWORD(xxx));
}
//! �� �������������� ������� ���������� ������� ������������� ��� ��� ���
inline bool get_invers(DWORD xxx){
	return static_cast<bool>(HIWORD(xxx));
}

// ������������
class Permutation
{
public:
	Permutation(int size = 0);

	void load(const char* fname);
	void save(const char* fname) const;

	void swap(int i, int j);
	int get_idx(int i) const;
private:
	std::vector<int> idx_;
};

extern Permutation g_permutation;

// ��������� ������ ���������, �������� �� ������������ (g_permutation)
template<class List>
void fillConditionList(List& list)
{
	for(int i = 0; i < g_iCondsSize; ++i){
		int iPos = list.AddString(g_arrConds[g_permutation.get_idx(i)].strName);
		if (iPos != -1) 
			list.SetItemData(iPos, static_cast<DWORD>(g_arrConds[g_permutation.get_idx(i)].id));
	}
}
#endif//_CONDITION_HEADER_