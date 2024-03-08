#pragma once

#include "MinigameParams/IMGParamsChangerFactory.h"

/*!
    ����� ����������� ��������� ������� ��������-��������.
*/
class MGParamsChangerFactory : public IMGParamChangerFactory
{
	MGParamsChangerFactory(void);
	~MGParamsChangerFactory(void);

	std::auto_ptr<MGParamChanger> createParamChanger(
		qdMinigameConfigParameter const& parameter,
		qdGameDispatcher* gameDisp);
public:

	static IMGParamChangerFactory& instance();
};
