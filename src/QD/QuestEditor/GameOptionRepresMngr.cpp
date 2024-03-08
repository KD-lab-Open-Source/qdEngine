#include "stdafx.h"
#include "resource.h"

#include "gameoptionrepresmngr.h"
#include "GameOptBaseRepres.h"

#include <vector>
#include <boost/iterator/indirect_iterator.hpp>

/// ѕодозреваю, что это уже не нужно, если используетс€ VC7.1
#include <Additional/itr_traits_spec.hpp>
/// ѕодозреваю, что и это уже не нужно, если используетс€ VC7.1
BROKEN_COMPILER_PTR_ITER_TRAITS_SPECIALIZATION( IGameOptionRepresPtr )

GameOptionRepresMngr::GameOptionRepresMngr(void)
{
	typedef boost::shared_ptr<IGameOptionRepres> Ptr;
	m_options.reserve(10);
	m_options.push_back(
		Ptr(new GameOptionBaseRepres
			(qdInterfaceElement::OPTION_NONE, IDS_OPTION_NONE)));
	m_options.push_back(
		Ptr(new GameOptionBaseRepres
			(qdInterfaceElement::OPTION_SOUND, IDS_OPTION_SOUND)));
	m_options.push_back(
		Ptr(new GameOptionBaseRepres
			(qdInterfaceElement::OPTION_SOUND_VOLUME, IDS_OPTION_SOUND_VOLUME)));
	m_options.push_back(
		Ptr(new GameOptionBaseRepres
				(qdInterfaceElement::OPTION_MUSIC, IDS_OPTION_MUSIC)));
	m_options.push_back(
		Ptr(new GameOptionBaseRepres
				(qdInterfaceElement::OPTION_MUSIC_VOLUME, IDS_OPTION_MUSIC_VOLUME)));
}

GameOptionRepresMngr::~GameOptionRepresMngr(void)
{
}

boost::shared_ptr<IGameOptionRepres> GameOptionRepresMngr::
							find(qdInterfaceElement::option_ID_t type)
{
	typedef boost::indirect_iterator<Options::iterator> Iter;

	Iter res = std::find(Iter(m_options.begin()), Iter(m_options.end()), type);
	ASSERT(res != Iter(m_options.end()));
	return *res.base();
}

boost::shared_ptr<IGameOptionRepres> GameOptionRepresMngr::
							find(qdInterfaceElement::option_ID_t  type) const
{
	typedef boost::indirect_iterator<Options::const_iterator> Iter;
	Iter res = std::find(Iter(m_options.begin()), 
		Iter(m_options.end()), type);
	ASSERT(res != Iter(m_options.end()));
	return *res.base();
}

IGameOptionRepres* GameOptionRepresMngr::operator[](int i) const{
	return m_options[i].get();
}

IGameOptionRepres* GameOptionRepresMngr::operator[](
						qdInterfaceElement::option_ID_t id) const
{
	return find(id).get();
}
