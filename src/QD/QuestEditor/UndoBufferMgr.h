// UndoBufferMgr.h: interface for the UndoBufferMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNDOBUFFERMGR_H__ECB83938_0C1E_4CC4_B3DC_1FD16AEC1AFE__INCLUDED_)
#define AFX_UNDOBUFFERMGR_H__ECB83938_0C1E_4CC4_B3DC_1FD16AEC1AFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ubentry.h"
#include <deque>

/**
	Undo buffer для редактора сцены.
 */
class UndoBufferMgr  
{
	typedef std::deque<ubentry*> buffer_t;

	/// Список действий для отмены
	buffer_t m_undo_buffer;
	/// Список действий для повтора
	buffer_t m_redo_buffer;

	/// Очищает буффер
	VOID clear_buff(buffer_t& buffer);
public:
	UndoBufferMgr();
	~UndoBufferMgr();

	/// Добавляет запись в undo буффер
	bool add(ubentry* pentry);
	/// Возвращает запись undo. 
	/**
		Возвращает запись undo и удаляет ее из списка действий отмены
		Конвертирует запись в undo и кладет в undo буффер
	*/	
	ubentry* get_undo();
	/// Возвращает запись redo. 
	/**
		Возвращает запись redo и удаляет ее из списка действий повтора
		Конвертирует запись в undo и кладет в undo буффер
	 */ 
	ubentry* get_redo();
	
	/// Очищает все
	VOID clear();
	/// Очищает undo буффер
	VOID clear_undo();
	/// Очищает redo буффер
	VOID clear_redo();

	/// @return true если undo буффер пустой
	bool undo_empty() const{
		return m_undo_buffer.empty();
	}

	/// @return true если redo буффер пустой
	bool redo_empty() const{
		return m_redo_buffer.empty();
	}
};

#endif //!defined(AFX_UNDOBUFFERMGR_H__ECB83938_0C1E_4CC4_B3DC_1FD16AEC1AFE__INCLUDED_)
