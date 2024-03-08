#pragma once
/********************************************************************
	created:	2003/05/01
	created:	1:5:2003   18:35
	filename: 	d:\Projects\Quest\QuestEditor\ConvertTGAtoQDAInInterface.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	ConvertTGAtoQDAInInterface
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	Конвертация тга в qda для интерфейса
*********************************************************************/

/// \brief Класс-утилита. Предоставляет набор функций для конвертации изображений, привязанных
/// к элементам интерфейса в анимации
class ConvertTGAtoQDAInInterface
{
public:
	ConvertTGAtoQDAInInterface(void);
	~ConvertTGAtoQDAInInterface(void);
	/// Входная функция. Вызывает остальные в зависимости от типа объекта
	void convert(qdNamedObjectBase* pobj) const;
private:
	void convertTGAtoQDAInInterfaceScreen(class qdInterfaceScreen* pobj) const;
	void convertTGAtoQDAInInterfaceButton(class qdInterfaceButton* pobj) const;
	void convertTGAtoQDAInInterfaceSlider(class qdInterfaceSlider* pobj) const;
	void convertTGAtoQDAInInterfaceSave(class qdInterfaceSave* pobj) const;
	void convertTGAtoQDAInInterfaceElementState(
									class qdInterfaceElementState* pobj) const;
};
