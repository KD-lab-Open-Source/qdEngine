#pragma once
#include "minigameparams\paramchanger\mgparamchanger.h"
/*!
    �����, �������������� ���������� �� ��������� ��� �������-��������
*/
class MGParamChangerStube : public MGParamChanger
{
public:
	MGParamChangerStube(qdMinigameConfigParameter const& parameter);
	~MGParamChangerStube(void);
	virtual bool isValid() const;
	qdMinigameConfigParameter const& getParameter() const;
	virtual LPCTSTR getCommentString() const;
	virtual LPCTSTR getValueString() const;
	virtual LPCTSTR getParameterName() const;
protected:
	qdMinigameConfigParameter& getParameter() ;

private:
	qdMinigameConfigParameter parameter_;
};
