/********************************************************************
	created:	2003/06/17
	created:	17:6:2003   15:31
	filename: 	d:\Projects\Quest\QuestEditor\InterfaceEventRepresBase.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	InterfaceEventRepresBase
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	Базовые классы для представления событий интерфейса в редакторе
*********************************************************************/
#pragma once

#include "qd_interface_element_state.h"

/**
	Определяет набор методов для всех представлений событий интерфейса в редакторе
 */
interface IInterfaceEventRepres
{
	/// Возвращает значение по умолчанию
	virtual LPCTSTR			getDefaultValue() const = 0;
	/// Описание события
	virtual CString const	description() const = 0;
	/// Нужны ли дополнительные данные
	virtual bool			hasData() const = 0;
	/// Идентификатор события
	virtual qdInterfaceEvent::event_t const eventId() const = 0;
	/// Загружает данные в окно
	virtual void			loadData(CWnd* pwnd) const = 0;
	virtual ~IInterfaceEventRepres() = 0;
};

inline bool operator == (IInterfaceEventRepres const& lhs, qdInterfaceEvent::event_t id){
	return (lhs.eventId() == id);
}
inline bool operator == (IInterfaceEventRepres const& lhs, 
						 IInterfaceEventRepres const& rhs){
	return (lhs.eventId() == rhs.eventId());
}

/**
	Базовый класс для всех представлений событий интерфейса в редакторе
 */
class InterfaceEventRepresBase : public IInterfaceEventRepres
{
public:
	InterfaceEventRepresBase(qdInterfaceEvent::event_t id, UINT_PTR descr_id);
	virtual ~InterfaceEventRepresBase(void);

	qdInterfaceEvent::event_t const eventId() const{
		return m_eEventId;
	}
	CString const description() const{
		return m_strDescription;
	}
private:
	/// Идентификатор представляемого события
	qdInterfaceEvent::event_t const m_eEventId;
	/// Описание события
	CString const m_strDescription;
};

/**
	Представляет все события, которые не требуют задавать дополнительные данные
 */
class InterfaceEventWithoutDataRepres : public InterfaceEventRepresBase
{
public:
	InterfaceEventWithoutDataRepres(qdInterfaceEvent::event_t id, UINT_PTR descr_id);
	virtual bool hasData() const{
		return false;
	}
	/// пуст
	virtual LPCTSTR	getDefaultValue() const;
	/// пуст
	virtual void loadData(CWnd* pwnd) const;
};

/**
	Базовый класс для представлений, требующих указания дополнительных параметров
 */
class InterfaceEventWithDataRepres : public InterfaceEventRepresBase
{
public:
	InterfaceEventWithDataRepres(qdInterfaceEvent::event_t id, UINT_PTR descr_id);
	virtual bool hasData() const{
		return true;
	}
protected:
	/**
		Функтор загружает имя объекта в указанный комбобокс
	 */
	class PutObjectNameToCombo
	{
	public:
		PutObjectNameToCombo(CCJFlatComboBox& box):m_box(box){
		}
		void operator()(qdNamedObjectBase* p){
			m_box.AddString(p->name());
		}
	private:
		/// Комбобокс
		CCJFlatComboBox& m_box;
	};

	/**
		Функтор загружает имя объекта и имя его владельца в указанный комбобокс
	 */
	class PutObjectNameWithOwnerToCombo
	{
	public:
		PutObjectNameWithOwnerToCombo(CCJFlatComboBox& box):m_box(box){
		}
		void operator()(const qdNamedObjectBase* owner, const qdNamedObjectBase* obj){
			if(owner->name() && obj->name()){
				std::string str = owner->name();
				str += "::";
				str += obj->name();

				m_box.AddString(str.c_str());
			}
		}
	private:
		/// Комбобокс
		CCJFlatComboBox& m_box;
	};
};