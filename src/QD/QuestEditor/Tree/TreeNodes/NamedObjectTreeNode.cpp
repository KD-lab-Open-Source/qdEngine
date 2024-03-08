#include "stdafx.h"
#include "ptree_consts.h"
#include ".\namedobjecttreenode.h"

#include "Stat Dependencies/StatObjectDependencies.h"
#include "Object List/IObjectList.h"

#include "qd_trigger_chain.h"
#include "te_helpers.h"
#include "Main/ObjectTraits.h"
#include "ITriggerView.h"


NamedObjectTreeNode::NamedObjectTreeNode(qdNamedObject* object):
  object_(object)
{
}

NamedObjectTreeNode::~NamedObjectTreeNode(void)
{
}

void NamedObjectTreeNode::setObject(qdNamedObject* object){
	object_ = object;
}

qdNamedObject* NamedObjectTreeNode::getObject() const{
	return object_;
}

ContentClassificator NamedObjectTreeNode::getContentClassificator() const{
	return CC_NAMED_OBJECT;
}

NodeContent NamedObjectTreeNode::getContent() const{
	NodeContent res;
	res.gameObject = getObject();
	return res;
}

void NamedObjectTreeNode::setINTriggerFlag(TreeLogic& logic, 
										   eTriggerIconCommand value)
{
	logic.setInTriggerIcon(this, value);
}

void NamedObjectTreeNode::onDeleteItem(TreeLogic& logic)
{
	removeIndexEntry(logic, reinterpret_cast<DWORD_PTR>(getObject()));
}

bool NamedObjectTreeNode::preDelete(TreeLogic& logic, bool ask, bool statDepends)
{
	if (ask&&!askUserForDelete(getObject()->name()))
		return false;
	
	if (statDepends&&statObjectDependencies(logic, getObject()))
		return false;

	if (!ObjectTraits::is_obj_state(getObject())&&getObject()->is_in_triggers()) 
	{
		removeObjectFromTriggers(getObject(), logic.getGameDispatcher());
		logic.getTriggerView()->clearUndobuffer();
	}

	return true;
}

bool NamedObjectTreeNode::statObjectDependencies(TreeLogic& logic, 
												 qdNamedObject const* object)
{
	std::map<qdNamedObject const*, NamedObjectList> statistic;
	StatObjectDependencies::run(logic.getGameDispatcher(), object, &statistic);
	if (!statistic.empty())
	{
		if (filterDepedenciesBound(logic, object, statistic) 
			== FilterObjectDependencies::ACROSS_OBJECT)
		{
			fillObjectList(logic, statistic);
			logic.getObjectList()->showObjectInList(&qdNamedObjectReference(object));
			logic.getObjectList()->show();
			return true;
		}
	}
	return false;
}

//! Проверяет выходят ли зависимости объектов сцены за пределы этой сцены
FilterObjectDependencies::eDependenciesBound 
	NamedObjectTreeNode::filterDepedenciesBound(TreeLogic& logic, 
												qdNamedObject const* object,
												DependsIndex & statistic)
{
	return FilterObjectDependencies::run(object, &statistic)
											? FilterObjectDependencies::ACROSS_OBJECT 
											: FilterObjectDependencies::IN_OBJECT;
}


void NamedObjectTreeNode::fillObjectList(TreeLogic& logic,
										 DependsIndex const& statistic)
{
	logic.getObjectList()->clearList();
	boost::rtl::crange< DependsIndex const> stats(statistic);
	for(; stats; ++stats)
	{
		boost::rtl::crange<NamedObjectList const> list((*stats).second);
		if (list)
		{
			qdNamedObjectReference objRef((*stats).first);
			HITEM objNode = logic.getObjectList()->addObjectToList(&objRef);

			for(; list; ++list)
			{
				qdNamedObjectReference ref(*list);
				logic.getObjectList()->addObjectToList(&ref, objNode);
			}
		}
	}
}

/*!
 	Пробегает по всем цепочкам. Усли находит элемент, который содержит \a pno, удаляет
	его
*/
void NamedObjectTreeNode::removeObjectFromTriggers(qdNamedObject const* pno, 
												   qdGameDispatcher* disp)
{
	boost::rtl::crange<qdTriggerChainList const> r(disp->trigger_chain_list());
	for(; r; ++r)
	{
		qdTriggerChain* const pchain = *r;
		while (qdTriggerElementPtr const& ptrEle = pchain->search_element(pno)) {
			te_h::RemoveLinks(pchain, ptrEle);
			pchain->remove_element(ptrEle);
		}
	}
}

bool NamedObjectTreeNode::canRename(TreeLogic& logic){
	return true;
}
