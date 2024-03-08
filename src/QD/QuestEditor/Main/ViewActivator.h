#pragma once

//! Переключает между собой окна редактора триггеров и редактора сцены
/*!
	Хранит информацию о видимости окон редактора триггеров и редакотора сцены, 
	и предоставляет к ней доступ.
	Реализует механизм переключения между окнами
*/
class Project;

class ViewActivator
{
	//! Видимо ли окно редактора сцены
	/*!
		если true, то на экране редактор сцены
		false - редактор триггеров
	*/
	bool isSceneViewActive_;
	//! указатель на дерево проекта
	Project* project_;

	const ViewActivator& operator = (const ViewActivator&);
	ViewActivator(const ViewActivator&);
public:
	ViewActivator(Project* p):isSceneViewActive_(true), project_(p){}

	bool getSceneViewActive() const;
	//! присваиваем новое значение и в соответствии с ним меняем видимость окон
	void operator = (bool b);
};

