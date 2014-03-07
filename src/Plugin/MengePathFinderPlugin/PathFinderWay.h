#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Math/vec2.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef std::vector<mt::vec2f> TVectorWayPoint;
	//////////////////////////////////////////////////////////////////////////
	class PathFinderWay
	{
	public:
		PathFinderWay( ServiceProviderInterface * _serviceProvider );
		~PathFinderWay();

	public:
		void initialize( const mt::vec2f & _from, const mt::vec2f & _to, const TVectorWayPoint & _way );

	public:
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera );

	public:
		inline size_t getWayPointCount() const;
		inline const mt::vec2f & getWayPoint( size_t _index ) const;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		mt::vec2f m_from;
		mt::vec2f m_to;

		TVectorWayPoint m_way;
	};
	//////////////////////////////////////////////////////////////////////////
	inline size_t PathFinderWay::getWayPointCount() const
	{
		size_t size = m_way.size();

		return size;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::vec2f & PathFinderWay::getWayPoint( size_t _index ) const
	{
		const mt::vec2f & p = m_way[_index];

		return p;
	}

}

