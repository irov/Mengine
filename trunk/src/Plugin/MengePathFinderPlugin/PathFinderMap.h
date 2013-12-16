#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "Core/Polygon.h"

#	include "poly2tri.h"

namespace Menge
{
	typedef std::vector<Poly2Tri::Point *> TVectorPathPoint;

	class PathFinderMap
	{
	public:
		PathFinderMap();
		~PathFinderMap();

	public:
		void setMap( const Polygon & _polygon );
		void setUnitSize( float _size );

	public:
		bool addHole( size_t _index, const Polygon & _polygon );
		
	public:
		bool generateMap();

		bool findPath( const mt::vec2f & _from, const mt::vec2f & _to );

	protected:
		bool testHoles_( size_t _index, const Polygon & _polygon ) const;

	protected:
		Polygon m_mapPolygon;
		float m_unitSize;

		typedef std::map<size_t, Polygon> THoles;
		THoles m_holes;
		THoles m_bigHoles;

		Poly2Tri::Point m_cachePoint[1024];
		size_t m_cachePointUse;

		Poly2Tri::SweepContext m_sweepContext;
	};
}