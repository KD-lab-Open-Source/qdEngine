#include "stdafx.h"
#include ".\mgparamschangerfactory.h"

#include "ParamChanger/SimpleMGParamChanger.h"
#include "ParamChanger/EditMGParamChanger.h"
#include "ParamChanger/FileMGParamChanger.h"
#include "ParamChanger/ObjectMGParamChanger.h"

MGParamsChangerFactory::MGParamsChangerFactory(void)
{
}

MGParamsChangerFactory::~MGParamsChangerFactory(void)
{
}

std::auto_ptr<MGParamChanger> MGParamsChangerFactory::createParamChanger(
	qdMinigameConfigParameter const& parameter,
	qdGameDispatcher* gameDisp)
{
	switch(parameter.data_type()) {
	case qdMinigameConfigParameter::PRM_DATA_STRING:
		{
			std::auto_ptr<MGParamChanger> first(new SimpleMGParamChanger(parameter));
			if (!first.get())
				return first;
			std::auto_ptr<MGParamChanger> second(new EditMGParamChanger(first.get()));
			if (!second.get())
				return second;
			first.release();
			return second;
		}
		break;
	case qdMinigameConfigParameter::PRM_DATA_FILE:
		{
			std::auto_ptr<MGParamChanger> first(new SimpleMGParamChanger(parameter));
			if (!first.get())
				return first;
			std::auto_ptr<MGParamChanger> second(new FileMGParamChanger(first.get()));
			if (!second.get())
				return second;
			first.release();
			return second;

		}
		break;
	case qdMinigameConfigParameter::PRM_DATA_OBJECT:
		{
			std::auto_ptr<MGParamChanger> first(
				new ObjectMGParamChanger(parameter, gameDisp));
			return first;

		}
		break;
	}
	return std::auto_ptr<MGParamChanger>();
}

IMGParamChangerFactory& MGParamsChangerFactory::instance(){
	static MGParamsChangerFactory instance;
	return instance;
}