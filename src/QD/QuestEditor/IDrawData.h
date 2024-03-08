#pragma once
#include <utility>

interface IDrawData
{
	virtual std::pair<class qdTriggerChain const* const*, int> getDrawedChains() const = 0;
};