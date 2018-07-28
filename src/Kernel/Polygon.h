#pragma once

#include "Config/Typedef.h"
#include "Config/Vector.h"

#include "math/vec2.h"
#include "math/mat4.h"
#include "math/box2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
	typedef Vector<mt::vec2f> VectorPoints;
	typedef Vector<uint32_t> VectorIndices;
    //////////////////////////////////////////////////////////////////////////
	typedef Vector<class Polygon> VectorPolygon;
	//////////////////////////////////////////////////////////////////////////
	class Polygon
	{
	public:
		Polygon();
		~Polygon();

		Polygon( const Polygon & _polygon );
        Polygon( Polygon && _polygon );
		Polygon( const void * _impl );

	public:
		typedef uint32_t size_type;

	public:
		void operator = (const Polygon & _polygon);
		
	public:
		void clear();

		void append( const mt::vec2f & _v );
		void append_inner( const Polygon & _polygon );

		void correct();

	public:
		bool triangulate( VectorPoints & _result ) const;
		bool triangulate_indices( VectorIndices & _result ) const;

		void mul_wm( Polygon & _out, const mt::mat4f & _wm ) const;
		void mul_wm_and_transpose( Polygon & _out, const mt::mat4f & _wm, const mt::vec2f & _pos ) const;
		void transpose( Polygon & _out, const mt::vec2f & _pos ) const;
		void transpose_and_scale( Polygon & _out, const mt::vec2f & _pos, const mt::vec2f & _scale ) const;

		void centroid( mt::vec2f & _center ) const;

		bool to_box2f( mt::box2f & _box2f ) const;
		bool empty() const;

        size_type num_points() const;

        size_type outer_count() const;
		const mt::vec2f * outer_points() const;
		mt::vec2f * outer_points();
		const mt::vec2f & outer_point( size_type _index ) const;
		mt::vec2f & outer_point( size_type _index );

        size_type inners_count() const;
        size_type inner_count( size_type _index ) const;
		const mt::vec2f * inner_points( size_type _index ) const;
		mt::vec2f * inner_points( size_type _index );
		const mt::vec2f & inner_point( size_type _index, size_type _v ) const;
		mt::vec2f & inner_point( size_type _index, size_type _v );
		
	public:
		bool intersects( const Polygon & _polygon ) const;
		bool intersects( const mt::vec2f & _point ) const;
		bool intersects( const mt::vec2f & _p0, const mt::vec2f & _p1 ) const;
		bool intersects( const mt::box2f & _box ) const;

	public:
		bool intersection( const Polygon & _polygon, VectorPolygon & _out ) const;

	public:
		bool difference( const Polygon & _polygon, VectorPolygon & _out ) const;

	public:
		void * m_impl;
	};
	//////////////////////////////////////////////////////////////////////////	
}