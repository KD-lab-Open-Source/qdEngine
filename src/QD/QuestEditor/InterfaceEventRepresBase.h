/********************************************************************
	created:	2003/06/17
	created:	17:6:2003   15:31
	filename: 	d:\Projects\Quest\QuestEditor\InterfaceEventRepresBase.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	InterfaceEventRepresBase
	file ext:	h
	Powerd by:	�����
	
	purpose:	������� ������ ��� ������������� ������� ���������� � ���������
*********************************************************************/
#pragma once

#include "qd_interface_element_state.h"

/**
	���������� ����� ������� ��� ���� ������������� ������� ���������� � ���������
 */
interface IInterfaceEventRepres
{
	/// ���������� �������� �� ���������
	virtual LPCTSTR			getDefaultValue() const = 0;
	/// �������� �������
	virtual CString const	description() const = 0;
	/// ����� �� �������������� ������
	virtual bool			hasData() const = 0;
	/// ������������� �������
	virtual qdInterfaceEvent::event_t const eventId() const = 0;
	/// ��������� ������ � ����
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
	������� ����� ��� ���� ������������� ������� ���������� � ���������
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
	/// ������������� ��������������� �������
	qdInterfaceEvent::event_t const m_eEventId;
	/// �������� �������
	CString const m_strDescription;
};

/**
	������������ ��� �������, ������� �� ������� �������� �������������� ������
 */
class InterfaceEventWithoutDataRepres : public InterfaceEventRepresBase
{
public:
	InterfaceEventWithoutDataRepres(qdInterfaceEvent::event_t id, UINT_PTR descr_id);
	virtual bool hasData() const{
		return false;
	}
	/// ����
	virtual LPCTSTR	getDefaultValue() const;
	/// ����
	virtual void loadData(CWnd* pwnd) const;
};

/**
	������� ����� ��� �������������, ��������� �������� �������������� ����������
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
		������� ��������� ��� ������� � ��������� ���������
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
		/// ���������
		CCJFlatComboBox& m_box;
	};

	/**
		������� ��������� ��� ������� � ��� ��� ��������� � ��������� ���������
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
		/// ���������
		CCJFlatComboBox& m_box;
	};
};