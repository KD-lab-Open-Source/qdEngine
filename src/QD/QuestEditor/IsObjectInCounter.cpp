#include "stdafx.h"
#include ".\isobjectincounter.h"
#include "IsObjectInCounterTest.h"
#include "qd_counter.h"

#include <boost/rtl.hpp>

IsObjectInCounter::IsObjectInCounter(void)
{
}

IsObjectInCounter::~IsObjectInCounter(void)
{
}

bool IsObjectInCounter::run(qdGameDispatcher const* disp, 
					   qdNamedObject const* obj, 
					   NamedObjectList& objectList)
{
	bool res = false;
	boost::rtl::filtered_range<qdCounterList const,
							IsObjectInCounterTest> counters(disp->counter_list(),
														IsObjectInCounterTest(obj));
	while (counters) {
		objectList.push_back(*counters);
		++counters;
		res = true;
	}
	return res;
}