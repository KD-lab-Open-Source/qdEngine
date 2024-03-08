/********************************************************************
	created:	2002/05/31
	created:	31:5:2002   12:55
	filename: 	D:\Projects\Quest\QuestEditor\state_guard.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	state_guard
	file ext:	h
	author:		Илюха
	
	purpose:	принимает анимированный объект и при своём разрушении
				или при установке нового объекта устанавливает нулевое 
				состояние
*********************************************************************/

#ifndef __STATE_GUARD_HEADER__
#define __STATE_GUARD_HEADER__
#pragma once

class qdGameObjectAnimated;
class qdGameObjectMoving;

/**
принимает анимированный объект и при своём разрушении
или при установке нового объекта устанавливает нулевое 
состояние
 */
class state_guard
{
	/// Объект чье состояние охраняется
	qdGameObjectAnimated* m_ptrObj;
public:
	state_guard():m_ptrObj(NULL){}
	state_guard(qdGameObjectAnimated* ptrGOA, qdGameObjectState* ptrState):
		m_ptrObj(ptrGOA){
			if(ptrState)
					m_ptrObj->set_state(ptrState);
	}
	~state_guard(){
		release();
	}
	void operator = (qdGameObjectAnimated* ptrGOA){
		release();
		m_ptrObj = ptrGOA;
	}

	operator qdGameObjectAnimated*(){
		return m_ptrObj;
	}

	qdGameObjectAnimated* operator ->(){
		return m_ptrObj;
	}

	void set(qdGameObjectAnimated* ptrGOA){
		m_ptrObj = ptrGOA;
	}
private:

	/// Устанавливает, если возоможно для хранимого объекта состояние по умолчанию
	void release(){
		if(m_ptrObj)
		{
			m_ptrObj->set_default_state();
		}
		m_ptrObj = NULL;
	}
	state_guard(const state_guard& s){}
	void operator = (const state_guard& s){}
};

#endif