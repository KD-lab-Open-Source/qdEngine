#pragma once

/// Функтор. Назначает указанному объекту инвентори
class InventorySetter
{
public:
	InventorySetter();
	InventorySetter(LPCTSTR name);
	void operator()(qdGameObject* object);
private:
	/// Имя инвентори
	LPCTSTR name_;
};
