#pragma once

#include "MinigameParams/IMGParamsChangerFactory.h"

/*!
    Класс реализующий интерфейс фабрики объектов-ченжеров.
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
