#pragma once

class qdGameDispatcher;
class qdGameDispatcherBase;

///	\brief  ласс скрывает в себе создание мультивизарда и прив€зки к нему
///	визарда создани€ анимации
class AddAnimMultiWizard
{
public:
	AddAnimMultiWizard(void);
	~AddAnimMultiWizard(void);
	/// «апускает визард
	/**
		\return true - если были сделаны изменени€
	 */
	bool show(CWnd * parent, class qdGameDispatcher* ptrDisp, 
			qdGameDispatcherBase* ptrGDB, UINT idCap = 0);
};
