/********************************************************************
	created:	2002/08/17
	created:	17:8:2002   12:22
	filename: 	D:\Projects\Quest\QuestEditor\UBEntry.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	UBEntry
	file ext:	h
	author:		Илюха
	
	purpose:	базовый класс для данных, входящих в undo buffer
*********************************************************************/
#ifndef __UNDO_BUFFER_ENTRY__
#define __UNDO_BUFFER_ENTRY__


/// Типы записей в буффере
enum UBE_TYPE{UBE_FIRST,///< Не используется
	UBE_OBJ_POS///< Действие, изменяющее позицию объекта
};

/**
	Реализует интерфейс для записей в undo буффер
 */
class ubentry
{
	/// Тип записи
	UBE_TYPE m_type;
public:
	ubentry(){};
	ubentry(UBE_TYPE t):m_type(t){}

	virtual ~ubentry(){};
	UBE_TYPE type() const{return m_type;}

	ubentry(const ubentry& ue):m_type(ue.m_type){};

	virtual const ubentry& operator = (const ubentry& ue)
	{m_type = ue.m_type; return *this;}
	/// Ковертирует текущую запись в противодействие (undo->redo, redo->undo)
	virtual ubentry* to_redo() = 0;

};
#endif //__UNDO_BUFFER_ENTRY__