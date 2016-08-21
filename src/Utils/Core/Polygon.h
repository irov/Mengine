#	pragma once

#   include "Config/Typedef.h"

#	include "Math/vec2.h"
#	include "Math/mat4.h"
#   include "Math/box2.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	typedef stdex::vector<mt::vec2f> TVectorPoints;
	typedef stdex::vector<uint32_t> TVectorIndices;

	typedef stdex::vector<class Polygon> TVectorPolygon;
	
	class Polygon
	{
	public:
		Polygon();
		~Polygon();

		Polygon( const Polygon & _polygon );
		Polygon( const void * _impl );

	public:
		typedef size_t size_type;

	public:
		void operator = (const Polygon & _polygon);
		
	public:
		void clear();

		void append( const mt::vec2f & _v );
		void append_inner( const Polygon & _polygon );

		void correct();

	public:
		bool triangulate( TVectorPoints & _result ) const;
		bool triangulate_indices( TVectorIndices & _result ) const;

		void mul_wm( Polygon & _out, const mt::mat4f & _wm ) const;
		void mul_wm_and_transpose( Polygon & _out, const mt::mat4f & _wm, const mt::vec2f & _pos ) const;
		void transpose( Polygon & _out, const mt::vec2f & _pos ) const;
		void transpose_and_scale( Polygon & _out, const mt::vec2f & _pos, const mt::vec2f & _scale ) const;

		void centroid( mt::vec2f & _center ) const;

		bool to_box2f( mt::box2f & _box2f ) const;
		bool empty() const;

		size_t num_points() const;

		size_t outer_count() const;
		const mt::vec2f * outer_points() const;
		mt::vec2f * outer_points();
		const mt::vec2f & outer_point( size_t _index ) const;
		mt::vec2f & outer_point( size_t _index );

		size_t inners_count() const;
		size_t inner_count( size_t _index ) const;
		const mt::vec2f * inner_points( size_t _index ) const;
		mt::vec2f * inner_points( size_t _index );
		const mt::vec2f & inner_point( size_t _index, size_t _v ) const;
		mt::vec2f & inner_point( size_t _index, size_t _v );
		
	public:
		bool intersects( const Polygon & _polygon ) const;
		bool intersects( const mt::vec2f & _point ) const;
		bool intersects( const mt::vec2f & _p0, const mt::vec2f & _p1 ) const;
		bool intersects( const mt::box2f & _box ) const;

	public:
		bool intersection( const Polygon & _polygon, TVectorPolygon & _out ) const;

	public:
		bool difference( const Polygon & _polygon, TVectorPolygon & _out ) const;

	public:
		void * impl;
	};
	//////////////////////////////////////////////////////////////////////////	
}