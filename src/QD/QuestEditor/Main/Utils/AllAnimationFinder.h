#pragma once

#include <map>

class qdGameDispatcher;

typedef std::map<qdNamedObject*, std::vector<LPCTSTR> > AnimIndex_t;

///	Находит всю анимацию в проекте, использующую один и тот же файл.

/*@
	Ключем в AnimIndex_t являются сцены
*/
class AllAnimationFinder
{
public:
	AllAnimationFinder(void);
	~AllAnimationFinder(void);
	void find(qdGameDispatcher& gameDisp, const CString& strFileName, AnimIndex_t& index);
};
