#pragma once

#include <TreeListCtrl/Null Listeners/NullChangeItemLabelNotifyListener.h>
#include "qd_minigame_config.h"
/*!
	Инетерфейс, который реализуется объектом-ченжером.
	Объект-ченжер производит изменение параметра
*/
class MGParamChanger : public NullChangeItemLabelNotifyListener
{
public:
	virtual bool isValid() const = 0;
	virtual qdMinigameConfigParameter const& getParameter() const = 0;
	virtual LPCTSTR getValueString() const = 0;
	virtual LPCTSTR getCommentString() const = 0;
	virtual LPCTSTR getParameterName() const = 0;
};