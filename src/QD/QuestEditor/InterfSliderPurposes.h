#pragma once

#include <vector>
#include "qd_interface_element.h"

/// Менеджер списка применений для слайдера
class InterfSliderPurposes
{
	/// Описывает одно конкретное применение
	class interfsldr_purpose{
		/// Идентификатор строки в ресурсах
		UINT descr_id_;
		/// Загруженная из ресурсов строка
		mutable CString descr_;
		/// Идентификатор применения
		qdInterfaceElement::option_ID_t option_id_;
	public:
		interfsldr_purpose(UINT descr_id, qdInterfaceElement::option_ID_t oid);
		/// возвращает описание
		const CString& get_descr() const;

		/// возвращает идентификатор
		const qdInterfaceElement::option_ID_t opt_id() const;

		bool operator== (qdInterfaceElement::option_ID_t id) const{
			return (opt_id() == id);
		}
	};
public:
	/**
		Инициализирует список применений
	 */
	InterfSliderPurposes(void);
	~InterfSliderPurposes(void);

	/// Количество применений в списке
	size_t count() const;
	/// Идентификатор i-го применения
	const qdInterfaceElement::option_ID_t get_opt_id(int i) const;
	/// Описание i-го применения
	CString get_descr(int i) const;
	/// Описание по идентификатору применения
	CString get_descr(const qdInterfaceElement::option_ID_t id) const;
private:
	/// Находит применение в списке по идентификатору
	interfsldr_purpose const* find(const qdInterfaceElement::option_ID_t id) const;
private:
	/// Список применений
	std::vector<interfsldr_purpose>	purposes_;
};
