// UndoBufferMgr.cpp: implementation of the UndoBufferMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "questeditor.h"
#include "UndoBufferMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UndoBufferMgr::UndoBufferMgr()
{

}

UndoBufferMgr::~UndoBufferMgr()
{
	clear();
}

bool UndoBufferMgr::add(ubentry* pentry){
	m_undo_buffer.push_back(pentry);
	clear_redo();
	return true;
}

ubentry* UndoBufferMgr::get_undo(){
	if (m_undo_buffer.empty()) 
		return NULL;

	ubentry* res = m_undo_buffer.back();
	m_undo_buffer.pop_back();

	m_redo_buffer.push_back(res->to_redo());

	return res;
}

ubentry* UndoBufferMgr::get_redo(){
	if (m_redo_buffer.empty()) 
		return NULL;

	ubentry* res = m_redo_buffer.back();
	m_redo_buffer.pop_back();

	m_undo_buffer.push_back(res->to_redo());
	return res;
}

VOID UndoBufferMgr::clear(){
	clear_undo();
	clear_redo();
}

VOID UndoBufferMgr::clear_buff(buffer_t& buffer){
	buffer_t::iterator _itr = buffer.begin(), _end = buffer.end();
	for(; _itr != _end; ++_itr)
		delete *_itr;
	buffer.clear();
}

VOID UndoBufferMgr::clear_undo(){
	clear_buff(m_undo_buffer);
}

VOID UndoBufferMgr::clear_redo(){
	clear_buff(m_redo_buffer);
}