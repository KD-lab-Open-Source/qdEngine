#pragma once
#include "IGameOptBaseRepres.h"
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<IGameOptionRepres> IGameOptionRepresPtr;

/**
	Создает список опций игры. В конструкторе объекта создается список, к которому
	потом можно получить доступ через методы класса
 */
class GameOptionRepresMngr
{
	typedef std::vector<IGameOptionRepresPtr> Options;
public:
	GameOptionRepresMngr();
	~GameOptionRepresMngr();

	/// Возвращает опцию по индексу
	IGameOptionRepres* operator[](int i) const;
	/// Возвращает опцию по идентификатору
	IGameOptionRepres* operator[](qdInterfaceElement::option_ID_t id) const;
	/// Количиство опций
	size_t size() const{
		return m_options.size();
	}
private:
	/// Находит опцию по типу
	boost::shared_ptr<IGameOptionRepres> find(qdInterfaceElement::option_ID_t type);
	/// Находит опцию по типу(константный вариант)
	boost::shared_ptr<IGameOptionRepres> find(qdInterfaceElement::option_ID_t  type) const;
private:
	/// опции
	Options m_options;
};
