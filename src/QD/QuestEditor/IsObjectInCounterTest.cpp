#include "stdafx.h"
#include ".\isobjectincountertest.h"
#include "qd_named_object.h"
#include "qd_counter.h"
#include "qd_game_object_state.h"

#include "obj_type_detectors.h"

#include <typeinfo>
#include <boost/rtl.hpp>


IsObjectInCounterTest::IsObjectInCounterTest(qdNamedObject const* testObject):
testObject_(testObject)
{
}

IsObjectInCounterTest::~IsObjectInCounterTest(void)
{
}

bool IsObjectInCounterTest::operator() (qdCounter const* counter) const{
	return run(counter, testObject_);
}

bool IsObjectInCounterTest::run(qdCounter const* counter, qdNamedObject const* obj){
	if (ptree_helpers::is_obj_state(obj))
	{	
		return (boost::rtl::rng::find(counter->elements(), 
					static_cast<qdGameObjectState const*>(obj)));
	}
	return false;
}

