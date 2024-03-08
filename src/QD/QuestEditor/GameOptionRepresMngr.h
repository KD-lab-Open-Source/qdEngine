#pragma once
#include "IGameOptBaseRepres.h"
#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<IGameOptionRepres> IGameOptionRepresPtr;

/**
	������� ������ ����� ����. � ������������ ������� ��������� ������, � ��������
	����� ����� �������� ������ ����� ������ ������
 */
class GameOptionRepresMngr
{
	typedef std::vector<IGameOptionRepresPtr> Options;
public:
	GameOptionRepresMngr();
	~GameOptionRepresMngr();

	/// ���������� ����� �� �������
	IGameOptionRepres* operator[](int i) const;
	/// ���������� ����� �� ��������������
	IGameOptionRepres* operator[](qdInterfaceElement::option_ID_t id) const;
	/// ���������� �����
	size_t size() const{
		return m_options.size();
	}
private:
	/// ������� ����� �� ����
	boost::shared_ptr<IGameOptionRepres> find(qdInterfaceElement::option_ID_t type);
	/// ������� ����� �� ����(����������� �������)
	boost::shared_ptr<IGameOptionRepres> find(qdInterfaceElement::option_ID_t  type) const;
private:
	/// �����
	Options m_options;
};
