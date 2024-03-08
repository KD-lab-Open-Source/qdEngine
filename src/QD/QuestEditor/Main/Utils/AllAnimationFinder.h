#pragma once

#include <map>

class qdGameDispatcher;

typedef std::map<qdNamedObject*, std::vector<LPCTSTR> > AnimIndex_t;

///	������� ��� �������� � �������, ������������ ���� � ��� �� ����.

/*@
	������ � AnimIndex_t �������� �����
*/
class AllAnimationFinder
{
public:
	AllAnimationFinder(void);
	~AllAnimationFinder(void);
	void find(qdGameDispatcher& gameDisp, const CString& strFileName, AnimIndex_t& index);
};
