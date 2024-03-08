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
	Undo buffer ��� ��������� �����.
 */
class UndoBufferMgr  
{
	typedef std::deque<ubentry*> buffer_t;

	/// ������ �������� ��� ������
	buffer_t m_undo_buffer;
	/// ������ �������� ��� �������
	buffer_t m_redo_buffer;

	/// ������� ������
	VOID clear_buff(buffer_t& buffer);
public:
	UndoBufferMgr();
	~UndoBufferMgr();

	/// ��������� ������ � undo ������
	bool add(ubentry* pentry);
	/// ���������� ������ undo. 
	/**
		���������� ������ undo � ������� �� �� ������ �������� ������
		������������ ������ � undo � ������ � undo ������
	*/	
	ubentry* get_undo();
	/// ���������� ������ redo. 
	/**
		���������� ������ redo � ������� �� �� ������ �������� �������
		������������ ������ � undo � ������ � undo ������
	 */ 
	ubentry* get_redo();
	
	/// ������� ���
	VOID clear();
	/// ������� undo ������
	VOID clear_undo();
	/// ������� redo ������
	VOID clear_redo();

	/// @return true ���� undo ������ ������
	bool undo_empty() const{
		return m_undo_buffer.empty();
	}

	/// @return true ���� redo ������ ������
	bool redo_empty() const{
		return m_redo_buffer.empty();
	}
};

#endif //!defined(AFX_UNDOBUFFERMGR_H__ECB83938_0C1E_4CC4_B3DC_1FD16AEC1AFE__INCLUDED_)
