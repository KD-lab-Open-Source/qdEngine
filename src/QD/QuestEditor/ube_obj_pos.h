/********************************************************************
	created:	2002/08/17
	created:	17:8:2002   12:54
	filename: 	D:\Projects\Quest\QuestEditor\ube_obj_pos.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	ube_obj_pos
	file ext:	h
	author:		Илюха
	
	purpose:	данные для возврата позиции объекта
*********************************************************************/

#ifndef __UBE_OBJ_POS_H__
#define __UBE_OBJ_POS_H__

#include "ubentry.h"

/**
	Запись для восстановления позиции объекта
 */
class ubeObjPos: public ubentry
{
	/// имя объекта, пложение которого будем менять
	std::string m_obj_name;
	/// Старая позиция
	Vect3f m_vPosOld;
	/// Новая позиция
	Vect3f m_vPosNew;
public:
	ubeObjPos():ubentry(UBE_OBJ_POS){}
	ubeObjPos(const ubeObjPos& u):ubentry(u), m_obj_name(u.m_obj_name), 
								m_vPosOld(u.m_vPosOld), 
								m_vPosNew(u.m_vPosNew)
	{}
	const ubeObjPos& operator = (const ubeObjPos& u)
	{
		ubentry::operator = (u);
		m_obj_name = u.m_obj_name;
		m_vPosOld = u.m_vPosOld;
		m_vPosNew = u.m_vPosNew;
		return *this;
	}
	void set_old_pos(const Vect3f& vPosOld) {m_vPosOld = vPosOld;}
	void set_new_pos(const Vect3f& vPosNew) {m_vPosNew = vPosNew;}
	void set_name(LPCTSTR lpszObjName) { m_obj_name = lpszObjName; }

	/// возвращаем противодействие
	ubentry* to_redo(){
		ubeObjPos * p = new ubeObjPos();
		if(!p)
			return NULL;
		p->set_new_pos(m_vPosOld);
		p->set_old_pos(m_vPosNew);
		p->set_name(m_obj_name.c_str());

		return p;
	}

	LPCTSTR obj_name() const {
		return m_obj_name.c_str();
	}
	const Vect3f& old_pos() const{
		return m_vPosOld;
	}
	const Vect3f& new_pos() const{
		return m_vPosNew;
	}
	bool empty() const{
		return !strlen(m_obj_name.c_str());
	}
};

#endif //
