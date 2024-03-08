#pragma	once

#include "qd_interface_element_state.h"
#include "InterfaceEventRepresBase.h"

#include <boost/shared_ptr.hpp>

/**
	Класс управляет списком возможных интефейсных событий
 */
class InterfEventRep
{
public:
	typedef std::vector<boost::shared_ptr<IInterfaceEventRepres> > Events;
private:
	/// Находит событие по его идентификатору
	boost::shared_ptr<IInterfaceEventRepres> find(qdInterfaceEvent::event_t type);
	/// Находит событие по его идентификатору(константный вариант)
	boost::shared_ptr<IInterfaceEventRepres> find(qdInterfaceEvent::event_t type) const;
public:
	InterfEventRep(void);
	~InterfEventRep(void);

	/// Возвращает количество событий
	size_t events_count() const;
	/// Возвращает список
	Events const& events() const;
	/// Возвращает событие по индексу
	IInterfaceEventRepres const* operator[](int i) const;
	/// Возвращает событие по идентификатору
	IInterfaceEventRepres const* operator[](const qdInterfaceEvent::event_t& type) const;
private:
	/// Список событий
	Events events_;
};
