#include "stdafx.h"
#include ".\mgparamchangerstube.h"

MGParamChangerStube::MGParamChangerStube(qdMinigameConfigParameter const& parameter)
: parameter_(parameter)

{
}

MGParamChangerStube::~MGParamChangerStube(void)
{
}

bool MGParamChangerStube::isValid() const{
	const_cast<qdMinigameConfigParameter&>(parameter_).validate_data();

	return parameter_.is_data_valid();
}

qdMinigameConfigParameter const& MGParamChangerStube::getParameter() const{
	return parameter_;
}

qdMinigameConfigParameter& MGParamChangerStube::getParameter(){
	return parameter_;
}

LPCTSTR MGParamChangerStube::getCommentString() const{
	return parameter_.comment();
}

LPCTSTR MGParamChangerStube::getValueString() const{
	return parameter_.data_string();
}

LPCTSTR MGParamChangerStube::getParameterName() const{
	return parameter_.name();
}
