#pragma once
/********************************************************************
	created:	2003/05/01
	created:	1:5:2003   15:38
	filename: 	d:\Projects\Quest\QuestEditor\AnimByCadr.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	AnimByCadr
	file ext:	h
	Powerd by:	�����
	
	purpose:	�������� �������� �� �����
*********************************************************************/
#include <boost/shared_ptr.hpp>
class qdAnimation;
/// �������� �������� �� �����.
class AnimByCadr
{
public:
	AnimByCadr(void);
	~AnimByCadr(void);
	/// ������� �������� � ��������� ���� ���� ���� �� �����
	boost::shared_ptr<qdAnimation> make(LPCTSTR lpszFileName, float fCadrTime);
};
