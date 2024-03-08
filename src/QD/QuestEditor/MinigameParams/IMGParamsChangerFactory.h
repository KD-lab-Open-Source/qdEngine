

#pragma once

#include "ParamChanger/MGParamChanger.h"
#include <memory>

/*!
    ��������� ������� ��������-��������. 
	�� ��������� ������� �������������� ������-������
*/
interface IMGParamChangerFactory
{
	virtual std::auto_ptr<MGParamChanger> createParamChanger(
						qdMinigameConfigParameter const& parameter,
						qdGameDispatcher* gameDisp) = 0;
};