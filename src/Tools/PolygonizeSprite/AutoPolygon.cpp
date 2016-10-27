#   include "AutoPolygon.h"

#   include "Math/utils.h"

#   include "poly2tri/poly2tri.h"
#   include "clipper.hpp"

#   include <algorithm>
#   include <math.h>

#	include "Math/line2.h"

const static uint32_t PRECISION = 100;
//////////////////////////////////////////////////////////////////////////
AutoPolygon::AutoPolygon( const uint8_t * _data, uint32_t _width, uint32_t _height )
	: m_data( _data )
	, m_width( _width )
	, m_height( _height )
{
}
//////////////////////////////////////////////////////////////////////////
AutoPolygon::~AutoPolygon()
{
}
//////////////////////////////////////////////////////////////////////////
Pointfss AutoPolygon::test2( const Rect & rect, float epsilon, uint8_t threshold ) const
{	
	ClipperLib::Paths total_paths;

	for( uint32_t y = rect.y; y < rect.y + rect.height; ++y )
	{
		ClipperLib::Clipper clipper;
		clipper.StrictlySimple( true );

		uint32_t count = 0;

		int32_t prev_x = -1;

		for( uint32_t x = rect.x; x < rect.x + rect.width; ++x )
		{
			Point p( x, y );

			uint8_t alpha = this->getAlphaByPos( p );

			if( alpha <= threshold )
			{
				if( prev_x >= 0 )
				{
					ClipperLib::Path subj;

					ClipperLib::cInt p_prev_x = (ClipperLib::cInt)prev_x;
					ClipperLib::cInt p_x = (ClipperLib::cInt)x;
					ClipperLib::cInt p_y = (ClipperLib::cInt)y;

					subj << ClipperLib::IntPoint( (p_prev_x - epsilon * 0.5) * PRECISION, (p_y - epsilon * 0.5) * PRECISION );
					subj << ClipperLib::IntPoint( (p_x + epsilon * 0.5) * PRECISION, (p_y - epsilon * 0.5) * PRECISION );
					subj << ClipperLib::IntPoint( (p_x + epsilon * 0.5) * PRECISION, (p_y + epsilon * 0.5) * PRECISION );
					subj << ClipperLib::IntPoint( (p_prev_x - epsilon * 0.5) * PRECISION, (p_y + epsilon * 0.5) * PRECISION );

					clipper.AddPath( subj, ClipperLib::ptSubject, true );

					prev_x = -1;
					count += 1;
				}

				continue;
			}

			if( prev_x == -1 )
			{
				prev_x = x;
			}
		}

		if( prev_x != -1 )
		{
			ClipperLib::Path subj;

			ClipperLib::cInt p_prev_x = (ClipperLib::cInt)prev_x;
			ClipperLib::cInt p_x = (ClipperLib::cInt)rect.x + rect.width;
			ClipperLib::cInt p_y = (ClipperLib::cInt)y;

			subj << ClipperLib::IntPoint( (p_prev_x - epsilon * 0.5) * PRECISION, (p_y - epsilon * 0.5) * PRECISION );
			subj << ClipperLib::IntPoint( (p_x + epsilon * 0.5) * PRECISION, (p_y - epsilon * 0.5) * PRECISION );
			subj << ClipperLib::IntPoint( (p_x + epsilon * 0.5) * PRECISION, (p_y + epsilon * 0.5) * PRECISION );
			subj << ClipperLib::IntPoint( (p_prev_x - epsilon * 0.5) * PRECISION, (p_y + epsilon * 0.5) * PRECISION );

			clipper.AddPath( subj, ClipperLib::ptSubject, true );

			count += 1;
		}

		if( count != 0 )
		{
			ClipperLib::PolyTree solution;			
			clipper.Execute( ClipperLib::ctUnion, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero );

			ClipperLib::Paths paths;
			for( ClipperLib::PolyNode* p = solution.GetFirst(); p != nullptr; p = p->GetNext() )
			{
				if( p->IsHole() )
				{
					continue;
				}

				paths.push_back( p->Contour );
			}
			//ClipperLib::PolyTreeToPaths( solution, paths );

			total_paths.insert( total_paths.end(), paths.begin(), paths.end() );
		}
	}

	ClipperLib::Clipper clipper_total;
	clipper_total.StrictlySimple( true );

	for( ClipperLib::Paths::const_iterator
		it = total_paths.begin(),
		it_end = total_paths.end();
	it != it_end;
	++it )
	{
		const ClipperLib::Path & path = *it;

		clipper_total.AddPath( path, ClipperLib::ptSubject, true );
	}

	ClipperLib::PolyTree total_solution;
	clipper_total.Execute( ClipperLib::ctUnion, total_solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero );

	ClipperLib::Clipper clipper_total2;
	clipper_total2.StrictlySimple( true );
	
	for( ClipperLib::PolyNode* p = total_solution.GetFirst(); p != nullptr; p = p->GetNext() )
	{
		if( p->IsHole() )
		{
			continue;
		}

		ClipperLib::ClipperOffset co;
		co.AddPath( p->Contour, ClipperLib::jtSquare, ClipperLib::etClosedPolygon );

		ClipperLib::PolyTree final_solution;
		co.Execute( final_solution, epsilon * PRECISION * 0.25 );

		for( ClipperLib::PolyNode* p2 = final_solution.GetFirst(); p2 != nullptr; p2 = p2->GetNext() )
		{
			if( p2->IsHole() )
			{
				continue;
			}
			ClipperLib::Path path2;
			ClipperLib::CleanPolygon( p2->Contour, path2, epsilon * PRECISION * 0.05 );


			ClipperLib::Clipper cl;
			cl.StrictlySimple( true );
			cl.AddPath( path2, ClipperLib::ptSubject, true );
			//create the clipping rect
			ClipperLib::Path clamp;
			clamp.push_back( ClipperLib::IntPoint( 0, 0 ) );
			clamp.push_back( ClipperLib::IntPoint( (ClipperLib::cInt)(rect.width * PRECISION), 0 ) );
			clamp.push_back( ClipperLib::IntPoint( (ClipperLib::cInt)(rect.width * PRECISION), (ClipperLib::cInt)(rect.height * PRECISION) ) );
			clamp.push_back( ClipperLib::IntPoint( 0, (ClipperLib::cInt)(rect.height * PRECISION) ) );
			cl.AddPath( clamp, ClipperLib::ptClip, true );

			ClipperLib::PolyTree out_clamp;
			cl.Execute( ClipperLib::ctIntersection, out_clamp );

			ClipperLib::PolyNode* p3 = out_clamp.GetFirst();

			for( ClipperLib::PolyNode* p3 = out_clamp.GetFirst(); p3 != nullptr; p3 = p3->GetNext() )
			{
				if( p3->IsHole() )
				{
					continue;
				}

				const ClipperLib::Path & path3 = p3->Contour;

				clipper_total2.AddPath( path3, ClipperLib::ptSubject, true );
			}
		}
	}

	ClipperLib::PolyTree total_solution2;
	clipper_total2.Execute( ClipperLib::ctUnion, total_solution2, ClipperLib::pftNonZero, ClipperLib::pftNonZero );

	Pointfss outPointss;

	for( ClipperLib::PolyNode* p = total_solution2.GetFirst(); p != nullptr; p = p->GetNext() )
	{
		if( p->IsHole() )
		{
			continue;
		}

		const ClipperLib::Path & path3 = p->Contour;

		Pointfs outPoints;

		for( std::vector<ClipperLib::IntPoint>::const_iterator
			it = path3.begin(),
			it_end = path3.end();
		it != it_end;
		++it )
		{
			const ClipperLib::IntPoint & pt = *it;

			double x = (double)pt.X / (double)PRECISION;
			double y = (double)m_height - (double)pt.Y / (double)PRECISION;

			outPoints.push_back( Pointf( x, y ) );
		}

		outPointss.push_back( outPoints );
	}
	
	return outPointss;
}
//////////////////////////////////////////////////////////////////////////
uint8_t AutoPolygon::getAlphaByIndex( uint32_t i ) const
{
	return *(m_data + i * 4 + 3);
}
//////////////////////////////////////////////////////////////////////////
uint8_t AutoPolygon::getAlphaByPos( const Point & p ) const
{
	if( p.x >= m_width || p.y >= m_height )
	{
		return 0;
	}

	uint32_t index = p.y * m_width + p.x;

	uint8_t alpha = this->getAlphaByIndex( index );

	return alpha;
}
//////////////////////////////////////////////////////////////////////////
static bool contains_point2( const Rect & _rect, const Point & p )
{
	if( p.x >= _rect.x && p.x < _rect.x + _rect.width
		&& p.y >= _rect.y && p.y < _rect.y + _rect.height )
	{
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
Triangles AutoPolygon::triangulate( const Pointfs & points ) const
{
	// if there are less than 3 points, then we can't triangulate
	if( points.size() < 3 )
	{
		return Triangles();
	}

	std::vector<p2t::Point*> p2points;
	for( Pointfs::const_iterator it = points.begin(); it < points.end(); it++ )
	{
		double x = it->x;
		double y = it->y;

		p2t::Point * p = new (std::nothrow) p2t::Point( x, y );
		p2points.push_back( p );
	}
	p2t::CDT cdt( p2points );
	cdt.Triangulate();
	std::vector<p2t::Triangle*> tris = cdt.GetTriangles();

	Vertex * verts = new (std::nothrow) Vertex[points.size()];
	uint16_t* indices = new (std::nothrow) uint16_t[tris.size() * 3];
	uint16_t idx = 0;
	uint16_t vdx = 0;

	for( std::vector<p2t::Triangle*>::const_iterator ite = tris.begin(); ite < tris.end(); ite++ )
	{
		for( int i = 0; i < 3; i++ )
		{
			p2t::Point * p = (*ite)->GetPoint( i );
			mt::vec2f v2 = mt::vec2f( (float)p->x, (float)p->y );
			bool found = false;
			uint16_t j;
			uint16_t length = vdx;
			for( j = 0; j < length; j++ )
			{
				if( verts[j].pos == v2 )
				{
					found = true;
					break;
				}
			}
			if( found )
			{
				//if we found the same vertex, don't add to verts, but use the same vertex with indices
				indices[idx] = j;
				idx++;
			}
			else
			{
				//vert does not exist yet, so we need to create a new one,                
				mt::vec2f t2f( 0, 0 ); // don't worry about tex coords now, we calculate that later
				Vertex vert = {v2, t2f};
				verts[vdx] = vert;
				indices[idx] = vdx;
				idx++;
				vdx++;
			}
		}
	}

	for( p2t::Point* j : p2points )
	{
		delete j;
	};

	Triangles tri;

	tri.verts.insert( tri.verts.begin(), verts, verts + vdx );
	tri.indices.insert( tri.indices.begin(), indices, indices + idx );

	return tri;
}
//////////////////////////////////////////////////////////////////////////
void AutoPolygon::calculateUV( const Rect & rect, std::vector<Vertex> & verts ) const
{
	/*
	whole texture UV coordination
	0,0                  1,0
	+---------------------+
	|                     |0.1
	|                     |0.2
	|     +--------+      |0.3
	|     |texRect |      |0.4
	|     |        |      |0.5
	|     |        |      |0.6
	|     +--------+      |0.7
	|                     |0.8
	|                     |0.9
	+---------------------+
	0,1                  1,1
	*/

	float texWidth = (float)m_width;
	float texHeight = (float)m_height;

	for( size_t i = 0; i != verts.size(); ++i )
	{
		Vertex & vx = verts[i];
		// for every point, offset with the center point
		float u = (vx.pos.x + rect.x) / texWidth;
		float v = (rect.y + rect.height - vx.pos.y) / texHeight;
		vx.uv.x = u;
		vx.uv.y = v;
	}
}
//////////////////////////////////////////////////////////////////////////
Rect AutoPolygon::getRealRect( const Rect & rect ) const
{
	Rect realRect = rect;
	//check rect to see if its zero
	if( realRect.width == 0 || realRect.height == 0 )
	{
		//if the instance doesn't have width and height, then the whole operation is kaput
		realRect = Rect( 0, 0, m_width, m_height );
	}
	else
	{
		//rect is specified, so convert to real rect
		realRect = rect;
	}
	return realRect;
}
//////////////////////////////////////////////////////////////////////////
Triangles AutoPolygon::generateTriangles( const Rect & rect, float epsilon, bool & overlaped, uint8_t threshold ) const
{
	Rect realRect = this->getRealRect( rect );

	Pointfss polygons;
	Masks excluded;

	Pointfss ptre = test2( rect, epsilon, threshold );

	Triangles tri2;

	for( Pointfss::const_iterator
		it = ptre.begin(),
		it_end = ptre.end();
	it != it_end;
	++it )
	{
		const Pointfs & p = *it;

		Triangles tri1 = triangulate( p );

		calculateUV( realRect, tri1.verts );

		size_t current_triangles_size = tri2.verts.size();
		
		for( size_t i = 0; i != tri1.indices.size(); ++i )
		{
			uint16_t new_index = tri1.indices[i] + current_triangles_size;

			tri2.indices.push_back( new_index );
		}

		tri2.verts.insert( tri2.verts.end(), tri1.verts.begin(), tri1.verts.end() );
	}
		
	return tri2;
}