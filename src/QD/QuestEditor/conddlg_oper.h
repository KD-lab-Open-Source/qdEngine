#pragma once
/********************************************************************
	created:	2003/04/16
	created:	16:4:2003   12:13
	filename: 	d:\Projects\Quest\QuestEditor\conddlg_oper.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	conddlg_oper
	file ext:	h
	author:		»люха
	
	purpose:	операции используемые в диалоге условий
*********************************************************************/

/**
	‘унктор, добавл€ющий условие объекту
 */
class add_cond{
	qdConditionalObject* object_;
public:
	add_cond(qdConditionalObject* pobj):object_(pobj){}
	void operator()(const boost::shared_ptr<qdCondition>& pcd){
		object_->add_condition(pcd.get());
	}
};

/**
	‘унктор, добавл€ющий группу объекту
 */
class add_group{
	qdConditionalObject* object_;
public:
	add_group(qdConditionalObject* pobj):object_(pobj){}
	void operator()(const std::pair<HTREEITEM, boost::shared_ptr<qdConditionGroup> >& pcg){
		object_->add_condition_group(pcg.second.get());
	}
};
