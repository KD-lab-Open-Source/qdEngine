#include "stdafx.h"
#include ".\allanimationfinder.h"

#include <boost/rtl.hpp>

#include "qd_game_dispatcher.h"
#include "qd_animation.h"

/// Проверяет какой файл используется в анимации.
/*!
Проверяет какой файл используется в анимации. Если имя файла совпадает с
test_anim_file_name::strFileName_, то кладет имя анимации в список 
	test_anim_file_name::vec_
*/
class test_anim_file_name
{
	std::vector<LPCTSTR>& vec_;
	CString strFileName_;
public:
	test_anim_file_name(const CString& strFileName,
		std::vector<LPCTSTR>& vec):vec_(vec), strFileName_(strFileName){}
		void operator()(const qdAnimation* p){
			if (!strFileName_.CompareNoCase(p->qda_file()))
				vec_.push_back(p->name());
		}
};


AllAnimationFinder::AllAnimationFinder(void)
{
}

AllAnimationFinder::~AllAnimationFinder(void)
{
}

void AllAnimationFinder::find(qdGameDispatcher& gameDisp,
							const CString& fileName, 
							AnimIndex_t& index)
{
	using namespace boost::rtl;
	rng::for_each(gameDisp.animation_list(), 
		test_anim_file_name(fileName, index[&gameDisp]));

	const qdGameSceneList& lst = gameDisp.scene_list();
	crange<qdGameSceneList const> r(gameDisp.scene_list());
	for (; r; ++r)
		rng::for_each((*r)->animation_list(), test_anim_file_name(fileName, index[*r]));
}
