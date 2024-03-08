#pragma once

class qdGameObjectState;
class qdGameObjectAnimated;

///  \brief Восстанавливает положение изображения при копировании(переносе) состояния из
/// одного объекта в другой.
class RestoreImageLayout
{
public:
	RestoreImageLayout(void);
	~RestoreImageLayout(void);

	/*!
		Восстанавливает положение изображения при копировании(переносе) состояния из
		одного объекта в другой.
		Получает экранные координаты исходного объекта с установленным исходным состоянием.
		Получает экранные координаты второго объекта с установленным новым состоянием.
		Добавляет к смещению нового состояния разницу между положениями двух объектов.
		\remarks \a pnew является копией \a psource
		\param psource	- Исходное состояние
		\param po_src	- Исходный объект
		\param pmo		- Объект, в который вставляем состояние
		\param pnew		- Состояние, которое вставлено
	*/
	static void restore(qdGameObjectState* psource, qdGameObjectAnimated * po_src,
		qdGameObjectAnimated* pmo, qdGameObjectState* pnew);
	/*!
 		Получает владельца состояния копии(параметр \a pnew) и передает управление
		RestoreImageLayout(qdGameObjectState* psource, qdGameObjectAnimated * po_src,
		qdGameObjectAnimated* pmo, qdGameObjectState* pnew)
	*/

	static void restore(qdGameObjectState* psource, 
		qdGameObjectAnimated* pmo, qdGameObjectState* pnew);

	/*!
		Выравнивает положение изображений для состояний при копировании динамического 
		объекта в персонаж
		\remarks \a pobj - должен быть копией \a psrc
	*/
	static void restore(qdGameObjectAnimated * psrc, qdGameObjectMoving *pobj);
};
