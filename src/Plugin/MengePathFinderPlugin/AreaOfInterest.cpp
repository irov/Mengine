#	include "AreaOfInterest.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AOIActor::AOIActor()
		: m_radius(0.f)
		, m_position(0.f, 0.f)
		, m_dead(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void AOIActor::setRadius( float _radius )
	{
		m_radius = _radius;
	}
	//////////////////////////////////////////////////////////////////////////
	void AOIActor::setPosition( const mt::vec2f & _pos )
	{
		m_position = _pos;
	}
	//////////////////////////////////////////////////////////////////////////	
	void AOIActor::addActorNeighbor( AOIActor * _actor )
	{
		m_neighbours.push_back( _actor );

		this->onActorEnter( _actor );
	}
	//////////////////////////////////////////////////////////////////////////
	void AOIActor::removeActorNeighbor( AOIActor * _actor )
	{
		TVectorAOIActors::iterator it_erase = std::find( m_neighbours.begin(), m_neighbours.end(), _actor );

		if( it_erase == m_neighbours.end() )
		{
			return;
		}

		m_neighbours.erase( it_erase );

		this->onActorLeave( _actor );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AOIActor::isActorNeighbor( AOIActor * _actor ) const
	{
		TVectorAOIActors::const_iterator it_erase = std::find( m_neighbours.begin(), m_neighbours.end(), _actor );

		if( it_erase == m_neighbours.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AOIActor::update()
	{
		if( m_dead == true )
		{
			for( TVectorAOIActors::iterator
				it = m_neighbours.begin(),
				it_end = m_neighbours.end();
			it != it_end; )
			{
				AOIActor * actor = *it;

				actor->removeActorNeighbor( this );
			}
		}
		else
		{
			float d_radius = m_radius * m_radius;

			for( TVectorAOIActors::iterator it = m_neighbours.begin(); it != m_neighbours.end(); )
			{
				AOIActor * actor = *it;

				if( mt::sqrlength_v2_v2( m_position, actor->m_position ) > d_radius )
				{
					this->onActorLeave( actor );

					*it = m_neighbours.back();
					m_neighbours.pop_back();
				}
				else
				{
					++it;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AOIActor::dead()
	{
		m_dead = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AreaOfInterest::addActor( AOIActor * _actor )
	{
		m_actors.push_back( _actor );
	}
	//////////////////////////////////////////////////////////////////////////
	void AreaOfInterest::removeActor( AOIActor * _actor )
	{
		_actor->dead();
	}
	//////////////////////////////////////////////////////////////////////////
	void AreaOfInterest::update()
	{

	}
}