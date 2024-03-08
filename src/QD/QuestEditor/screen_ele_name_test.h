#pragma once
#include "name_interf.h"

/**
	Реализация INameTester получающая уникальное имя для элемента интерфейса
 */
class screen_ele_name_test : public INameTester
{
	/// Экран, содержащий элемент
	class qdInterfaceScreen* m_pScreen;
public:
	explicit screen_ele_name_test(qdInterfaceScreen* pScreen);
	~screen_ele_name_test(void);
	/// Получает уникальное имя, опираясь на имена элементов экрана
	virtual const CString get_unique_name(const CString& str) const;
};
