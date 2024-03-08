#pragma once

class qdNamedObjectReference;
///  \brief »нтерфейс предоставл€ет дл€ реализации методы, позвол€ющие получать 
/// уведомлени€ о событи€х произошедших в списке объектов
class IObjectListEventListener
{
	IObjectListEventListener();
public:
	/// ”ведомление о двойном клике
	virtual void onObjectDblClick(qdNamedObjectReference const* obj)
};
