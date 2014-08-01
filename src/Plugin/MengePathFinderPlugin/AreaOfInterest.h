#	pragma once

#	include "Math/vec2.h"

#	include <vector>

namespace Menge
{
	typedef std::vector<class AOIActor *> TVectorAOIActors;

	class AOIActor
	{
	public:
		AOIActor();		

	public:
		void setRadius( float _radius );

	public:
		void setPosition( const mt::vec2f & _pos );

	public:
		void addActorNeighbor( AOIActor * _actor );
		void removeActorNeighbor( AOIActor * _actor );
		bool isActorNeighbor( AOIActor * _actor ) const;

	public:
		void update();
		void dead();

	protected:
		virtual void onActorEnter( AOIActor * _actor ) = 0;
		virtual void onActorLeave( AOIActor * _actor ) = 0;

	protected:
		float m_radius;
		mt::vec2f m_position;
				
		TVectorAOIActors m_neighbours;

		bool m_dead;
	};

	class AreaOfInterest
	{
	public:
		void addActor( AOIActor * _actor );
		void removeActor( AOIActor * _actor );

	public:
		void update();

	protected:
		typedef std::vector<AOIActor *> TVectorAOIActors;
		TVectorAOIActors m_actors;
	};
}