#pragma	once

#include "qd_interface_element_state.h"
#include "InterfaceEventRepresBase.h"

#include <boost/shared_ptr.hpp>

/**
	����� ��������� ������� ��������� ����������� �������
 */
class InterfEventRep
{
public:
	typedef std::vector<boost::shared_ptr<IInterfaceEventRepres> > Events;
private:
	/// ������� ������� �� ��� ��������������
	boost::shared_ptr<IInterfaceEventRepres> find(qdInterfaceEvent::event_t type);
	/// ������� ������� �� ��� ��������������(����������� �������)
	boost::shared_ptr<IInterfaceEventRepres> find(qdInterfaceEvent::event_t type) const;
public:
	InterfEventRep(void);
	~InterfEventRep(void);

	/// ���������� ���������� �������
	size_t events_count() const;
	/// ���������� ������
	Events const& events() const;
	/// ���������� ������� �� �������
	IInterfaceEventRepres const* operator[](int i) const;
	/// ���������� ������� �� ��������������
	IInterfaceEventRepres const* operator[](const qdInterfaceEvent::event_t& type) const;
private:
	/// ������ �������
	Events events_;
};
