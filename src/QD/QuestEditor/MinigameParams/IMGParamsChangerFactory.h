

#pragma once

#include "ParamChanger/MGParamChanger.h"
#include <memory>

/*!
    Интерфейс фабрики объектов-ченжеров. 
	По параметру создает соответсвующий объект-ченжер
*/
interface IMGParamChangerFactory
{
	virtual std::auto_ptr<MGParamChanger> createParamChanger(
						qdMinigameConfigParameter const& parameter,
						qdGameDispatcher* gameDisp) = 0;
};