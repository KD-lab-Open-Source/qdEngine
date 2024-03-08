#pragma once

#include <vector>
#include "qd_interface_element.h"

/// �������� ������ ���������� ��� ��������
class InterfSliderPurposes
{
	/// ��������� ���� ���������� ����������
	class interfsldr_purpose{
		/// ������������� ������ � ��������
		UINT descr_id_;
		/// ����������� �� �������� ������
		mutable CString descr_;
		/// ������������� ����������
		qdInterfaceElement::option_ID_t option_id_;
	public:
		interfsldr_purpose(UINT descr_id, qdInterfaceElement::option_ID_t oid);
		/// ���������� ��������
		const CString& get_descr() const;

		/// ���������� �������������
		const qdInterfaceElement::option_ID_t opt_id() const;

		bool operator== (qdInterfaceElement::option_ID_t id) const{
			return (opt_id() == id);
		}
	};
public:
	/**
		�������������� ������ ����������
	 */
	InterfSliderPurposes(void);
	~InterfSliderPurposes(void);

	/// ���������� ���������� � ������
	size_t count() const;
	/// ������������� i-�� ����������
	const qdInterfaceElement::option_ID_t get_opt_id(int i) const;
	/// �������� i-�� ����������
	CString get_descr(int i) const;
	/// �������� �� �������������� ����������
	CString get_descr(const qdInterfaceElement::option_ID_t id) const;
private:
	/// ������� ���������� � ������ �� ��������������
	interfsldr_purpose const* find(const qdInterfaceElement::option_ID_t id) const;
private:
	/// ������ ����������
	std::vector<interfsldr_purpose>	purposes_;
};
