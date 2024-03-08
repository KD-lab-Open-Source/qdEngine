#pragma once
#include "mgparamchanger.h"

#include <boost/scoped_ptr.hpp>

/*!
    Класс, предоставляющий реализацию по умолчанию, для классов-ченжеров, работающих
	поверх других классов-ченжеров. (EditMGParamChanger, FileMGParamChanger)
*/
class MGParamChangerCtrlStube : public MGParamChanger
{
public:
	MGParamChangerCtrlStube(MGParamChanger* paramChanger);
	~MGParamChangerCtrlStube(void);
	//! Уведомляет о том, что редактирование закончено
	virtual bool onEndLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
	//! Вызывается перед началам редактирования текста.
	virtual bool onBeginLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);

	virtual bool isValid() const;
	virtual qdMinigameConfigParameter const& getParameter() const;
	virtual LPCTSTR getValueString() const;
	virtual LPCTSTR getCommentString() const;
	virtual LPCTSTR getParameterName() const;
protected:
	void setMGParamChanger(MGParamChanger* paramChanger);
	MGParamChanger* getMGParamChanger() const;
private:
	boost::scoped_ptr<MGParamChanger> paramChanger_;
};
