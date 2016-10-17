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
bool AutoPolygon::trace( const Rect & rect, const Masks & _excluded, Points & _points, Mask & _mask, uint8_t threshold ) const
{
	Point first(0, 0);
	if( this->findFirstNoneTransparentPixel( rect, first, _excluded, threshold ) == false )
	{
		return false;
	}

	this->marchExclude( rect, first, _mask, threshold );

	Points points = this->marchSquare( rect, first, threshold );

	_points = points;

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool AutoPolygon::testExcludedPoint( const Masks & _excluded, const Point & _point ) const
{
	uint32_t test_index = _point.x + _point.y * m_width;

	for( Masks::const_iterator
		it = _excluded.begin(),
		it_end = _excluded.end();
	it != it_end;
	++it )
	{
		const Mask & mask = *it;

		if( mask[test_index] == 0 )
		{
			continue;
		}

		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool AutoPolygon::findFirstNoneTransparentPixel( const Rect & rect, Point & _point, const Masks & _excluded, uint8_t threshold ) const
{	
	for( uint32_t y = rect.y; y < rect.y + rect.height; ++y)
	{
		for( uint32_t x = rect.x; x < rect.x + rect.width; ++x )
		{
			Point p( x, y );

			uint8_t alpha = this->getAlphaByPos( p );

			if( alpha <= threshold )
			{
				continue;
			}

			if( this->testExcludedPoint( _excluded, p ) == true )
			{
				continue;
			}

			_point = p;

			return true;
		}
	}

	return false;
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
void AutoPolygon::marchExclude( const Rect & rect, const Point & start, Mask & mask, uint8_t threshold ) const
{	
	mask.resize( m_width * m_height );
	std::fill( mask.begin(), mask.end(), 0U );

	Points work_points;
	work_points.reserve( m_width * m_height );
	work_points.push_back( start );

	uint32_t start_index = start.x + start.y * m_width;

	mask[start_index] = 1;
	
	while( work_points.empty() == false )
	{
		Point p = work_points.back();
		work_points.pop_back();

		if( contains_point2( rect, p ) == false )
		{
			continue;
		}

		uint8_t alpha = this->getAlphaByPos( p );

		if( alpha <= threshold )
		{
			continue;
		}

		for( uint32_t i = 0; i != 4; ++i )
		{
			const int32_t dx[] = {-1, 0, +1, 0};
			const int32_t dy[] = {0, -1, 0, +1};

			Point pw( p.x + dx[i], p.y + dy[i] );

			if( contains_point2( rect, pw ) == false )
			{
				continue;
			}

			uint32_t test_index = pw.x + pw.y * m_width;

			if( mask[test_index] == 0 )
			{
				mask[test_index] = 1;

				work_points.push_back( pw );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
static bool contains_point( const Rect & _rect, const Point & p )
{
	if( p.x >= _rect.x && p.x <= _rect.x + _rect.width
		&& p.y >= _rect.y && p.y <= _rect.y + _rect.height )
	{
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
uint32_t AutoPolygon::getSquareValue( uint32_t x, uint32_t y, const Rect & rect, uint8_t threshold ) const
{
	/*
	checking the 2x2 pixel grid, assigning these values to each pixel, if not transparent
	+---+---+
	| 1 | 2 |
	+---+---+
	| 4 | 8 | <- current pixel (curx,cury)
	+---+---+
	*/

	//NOTE: due to the way we pick points from texture, rect needs to be smaller, otherwise it goes outside 1 pixel
	Rect fixedRect = Rect( rect.x, rect.y, rect.width - 2, rect.height - 2 );

	Point tl( x - 1, y - 1 );
	Point tr( x - 0, y - 1 );
	Point bl( x - 1, y - 0 );
	Point br( x - 0, y - 0 );

	uint32_t sv = 0;
	sv += (contains_point( fixedRect, tl ) && this->getAlphaByPos( tl ) > threshold) ? 1 : 0;
	sv += (contains_point( fixedRect, tr ) && this->getAlphaByPos( tr ) > threshold) ? 2 : 0;
	sv += (contains_point( fixedRect, bl ) && this->getAlphaByPos( bl ) > threshold) ? 4 : 0;
	sv += (contains_point( fixedRect, br ) && this->getAlphaByPos( br ) > threshold) ? 8 : 0;

	return sv;
}
//////////////////////////////////////////////////////////////////////////
Points AutoPolygon::marchSquare( const Rect & rect, const Point & start, uint8_t threshold ) const
{
	int stepx = 0;
	int stepy = 0;
	int prevx = 0;
	int prevy = 0;
	int startx = (int)start.x;
	int starty = (int)start.y;
	int curx = startx;
	int cury = starty;
	uint32_t count = 0;
	unsigned int totalPixel = m_width*m_height;
	bool problem = false;
	std::vector<int> case9s;
	std::vector<int> case6s;
	int i;
	std::vector<int>::iterator it;
	Points _points;
	do
	{
		int sv = this->getSquareValue( curx, cury, rect, threshold );
		switch( sv )
		{

		case 1:
		case 5:
		case 13:
			/* going UP with these cases:
			1          5           13
			+---+---+  +---+---+  +---+---+
			| 1 |   |  | 1 |   |  | 1 |   |
			+---+---+  +---+---+  +---+---+
			|   |   |  | 4 |   |  | 4 | 8 |
			+---+---+  +---+---+  +---+---+
			*/
			stepx = 0;
			stepy = -1;
			break;


		case 8:
		case 10:
		case 11:
			/* going DOWN with these cases:
			8          10          11
			+---+---+  +---+---+   +---+---+
			|   |   |  |   | 2 |   | 1 | 2 |
			+---+---+  +---+---+   +---+---+
			|   | 8 |  |   | 8 |   |   | 8 |
			+---+---+  +---+---+  	+---+---+
			*/
			stepx = 0;
			stepy = 1;
			break;


		case 4:
		case 12:
		case 14:
			/* going LEFT with these cases:
			4          12          14
			+---+---+  +---+---+   +---+---+
			|   |   |  |   |   |   |   | 2 |
			+---+---+  +---+---+   +---+---+
			| 4 |   |  | 4 | 8 |   | 4 | 8 |
			+---+---+  +---+---+  	+---+---+
			*/
			stepx = -1;
			stepy = 0;
			break;


		case 2:
		case 3:
		case 7:
			/* going RIGHT with these cases:
			2          3           7
			+---+---+  +---+---+   +---+---+
			|   | 2 |  | 1 | 2 |   | 1 | 2 |
			+---+---+  +---+---+   +---+---+
			|   |   |  |   |   |   | 4 |   |
			+---+---+  +---+---+  	+---+---+
			*/
			stepx = 1;
			stepy = 0;
			break;
		case 9:
			/*
			+---+---+
			| 1 |   |
			+---+---+
			|   | 8 |
			+---+---+
			this should normally go UP, but if we already been here, we go down
			*/
			//find index from xy;
			i = this->getIndexFromPos( curx, cury );
			it = std::find( case9s.begin(), case9s.end(), i );
			if( it != case9s.end() )
			{
				//found, so we go down, and delete from case9s;
				stepx = 0;
				stepy = 1;
				case9s.erase( it );
				problem = true;
			}
			else
			{
				//not found, we go up, and add to case9s;
				stepx = 0;
				stepy = -1;
				case9s.push_back( i );
			}
			break;
		case 6:
			/*
			6
			+---+---+
			|   | 2 |
			+---+---+
			| 4 |   |
			+---+---+
			this normally go RIGHT, but if its coming from UP, it should go LEFT
			*/
			i = this->getIndexFromPos( curx, cury );
			it = std::find( case6s.begin(), case6s.end(), i );
			if( it != case6s.end() )
			{
				//found, so we go down, and delete from case9s;
				stepx = -1;
				stepy = 0;
				case6s.erase( it );
				problem = true;
			}
			else
			{
				//not found, we go up, and add to case9s;
				stepx = 1;
				stepy = 0;
				case6s.push_back( i );
			}
			break;
		default:
			{
				return Points();
			}break;
		}
		//little optimization
		// if previous direction is same as current direction,
		// then we should modify the last vec to current
		curx += stepx;
		cury += stepy;
		if( stepx == prevx && stepy == prevy )
		{
			_points.back().x = curx - rect.x;
			_points.back().y = rect.height - cury + rect.y;
		}
		else if( problem )
		{
			//TODO: we triangulation cannot work collinear points, so we need to modify same point a little
			//TODO: maybe we can detect if we go into a hole and coming back the hole, we should extract those points and remove them
			_points.push_back( Point( curx - rect.x, rect.height - cury + rect.y ) );
		}
		else
		{
			_points.push_back( Point( curx - rect.x, rect.height - cury + rect.y ) );
		}

		count++;
		prevx = stepx;
		prevy = stepy;
		problem = false;

		if( count > totalPixel )
		{
			return Points();
		}
	}
	while( curx != startx || cury != starty );
	return _points;
}
//////////////////////////////////////////////////////////////////////////
float AutoPolygon::perpendicularDistance( const mt::vec2f & i, const mt::vec2f & start, const mt::vec2f & end ) const
{
	float res;
	float slope;
	float intercept;

	if( start.x == end.x )
	{
		res = fabsf( i.x - end.x );
	}
	else if( start.y == end.y )
	{
		res = fabsf( i.y - end.y );
	}
	else
	{
		slope = (end.y - start.y) / (end.x - start.x);
		intercept = start.y - (slope*start.x);
		res = fabsf( slope * i.x - i.y + intercept ) / sqrtf( powf( slope, 2 ) + 1 );
	}
	return res;
}
//////////////////////////////////////////////////////////////////////////
static mt::vec2f point_to_v( const Point & _p )
{
	mt::vec2f v( (float)_p.x, (float)_p.y );

	return v;
}
//////////////////////////////////////////////////////////////////////////
Points AutoPolygon::rdp( const Points& points, float optimization ) const
{
	if( points.size() < 3 )
		return points;

	size_t index = (size_t)-1;
	float dist = 0;
	//not looping first and last point

	const Point & v_front = points.front();
	const Point & v_back = points.front();

	mt::vec2f v_frontf = point_to_v( v_front );
	mt::vec2f v_backf = point_to_v( v_back );

	for( size_t i = 1; i < points.size() - 1; i++ )
	{
		const Point & v = points[i];

		mt::vec2f vf = point_to_v( v );
		float cdist = this->perpendicularDistance( vf, v_frontf, v_backf );
		if( cdist > dist )
		{
			dist = cdist;
			index = i;
		}
	}

	if( dist > optimization )
	{
		Points::const_iterator begin = points.begin();
		Points::const_iterator end = points.end();
		Points l1( begin, begin + index + 1 );
		Points l2( begin + index, end );

		Points r1 = rdp( l1, optimization );
		Points r2 = rdp( l2, optimization );

		r1.insert( r1.end(), r2.begin() + 1, r2.end() );
		return r1;
	}
	else
	{
		Points ret;
		ret.push_back( v_front );
		ret.push_back( v_back );
		return ret;
	}
}
//////////////////////////////////////////////////////////////////////////
Points AutoPolygon::reduce( const Points & points, const Rect & rect, float epsilon ) const
{
	Points::size_type size = points.size();
	// if there are less than 3 points, then we have nothing
	if( size < 3 )
	{
		return Points();
	}
	// if there are less than 9 points (but more than 3), then we don't need to reduce it
	else if( size < 9 )
	{
		return points;
	}

	uint32_t maxEp = (rect.width < rect.height) ? rect.width : rect.height;
	
	float ep = mt::clamp( 0.f, epsilon, (float)maxEp * 0.5f );
	Points result = rdp( points, ep );

	const Point & first = result.front();
	const Point & last = result.back();

	if( last.y > first.y && (last.x - first.x) * (last.x * first.x) + (last.y - first.y) * (last.y * first.y) < (uint32_t)(ep * 0.5f) )
	{
		result.front().y = last.y;
		result.pop_back();
	}
	return result;
}
//////////////////////////////////////////////////////////////////////////
Pointfs AutoPolygon::expand( const Points& points, const Rect &rect, float epsilon ) const
{
	Points::size_type size = points.size();
	// if there are less than 3 points, then we have nothing
	if( size < 3 )
	{
		return Pointfs();
	}

	ClipperLib::Path subj;
	ClipperLib::PolyTree solution;
	ClipperLib::PolyTree out;

	for( Points::const_iterator it = points.begin(); it < points.end(); it++ )
	{
		subj << ClipperLib::IntPoint( (ClipperLib::cInt)(it->x* PRECISION), (ClipperLib::cInt)(it->y * PRECISION) );
	}

	Pointfs outPoints;

	ClipperLib::ClipperOffset co;
	co.AddPath( subj, ClipperLib::jtMiter, ClipperLib::etClosedPolygon );
	co.Execute( solution, epsilon * PRECISION );

	ClipperLib::PolyNode* p = solution.GetFirst();
	
	if( p == nullptr )
	{
		for( Points::const_iterator it = points.begin(); it < points.end(); it++ )
		{
			double x = (double)it->x;
			double y = (double)it->y;

			outPoints.push_back( Pointf( x, y ) );
		}

		return outPoints;
	}

	while( p->IsHole() )
	{
		p = p->GetNext();
	}

	//turn the result into simply polygon (AKA, fix overlap)

	//clamp into the specified rect
	ClipperLib::Clipper cl;
	cl.StrictlySimple( true );
	cl.AddPath( p->Contour, ClipperLib::ptSubject, true );
	//create the clipping rect
	ClipperLib::Path clamp;
	clamp.push_back( ClipperLib::IntPoint( 0, 0 ) );
	clamp.push_back( ClipperLib::IntPoint( (ClipperLib::cInt)(rect.width * PRECISION), 0 ) );
	clamp.push_back( ClipperLib::IntPoint( (ClipperLib::cInt)(rect.width * PRECISION), (ClipperLib::cInt)(rect.height * PRECISION) ) );
	clamp.push_back( ClipperLib::IntPoint( 0, (ClipperLib::cInt)(rect.height * PRECISION) ) );
	cl.AddPath( clamp, ClipperLib::ptClip, true );
	cl.Execute( ClipperLib::ctIntersection, out );

	ClipperLib::PolyNode* p2 = out.GetFirst();
	while( p2->IsHole() )
	{
		p2 = p2->GetNext();
	}

	std::vector<ClipperLib::IntPoint>::const_iterator end = p2->Contour.end();
	for( std::vector<ClipperLib::IntPoint>::const_iterator pt = p2->Contour.begin(); pt < end; pt++ )
	{
		double x = (double)pt->X / (double)PRECISION;
		double y = (double)pt->Y / (double)PRECISION;

		outPoints.push_back( Pointf( x, y ) );
	}
	return outPoints;
}
//////////////////////////////////////////////////////////////////////////
Triangles AutoPolygon::triangulate( const Pointfs& points ) const
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

	Triangles triangles = {verts, indices, vdx, idx};

	return triangles;
}
//////////////////////////////////////////////////////////////////////////
void AutoPolygon::calculateUV( const Rect & rect, Vertex * verts, size_t count ) const
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

	for( Vertex
		*vertex = verts,
		*vertex_end = verts + count;
	vertex != vertex_end;
	++vertex )
	{
		// for every point, offset with the center point
		float u = (vertex->pos.x + rect.x) / texWidth;
		float v = (rect.y + rect.height - vertex->pos.y) / texHeight;
		vertex->uv.x = u;
		vertex->uv.y = v;
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
	for( ;; )
	{		
		Points pt;
		Mask mask;
		if( this->trace( realRect, excluded, pt, mask, threshold ) == false )
		{
			break;
		}

		excluded.push_back( mask );

		Points ptr = this->reduce( pt, realRect, epsilon );
		Pointfs ptre = this->expand( ptr, realRect, epsilon );

		polygons.push_back( ptre );
	}

	Pointfs ptre = mergePolygons( polygons );
	
	//Triangless tris;
	Triangles tri = triangulate( ptre );
	calculateUV( realRect, tri.verts, tri.vertCount );

	//tris.push_back( tri );

	//overlaped = testOverlapTriangles( tris );

	//Triangles merge_tri = mergeTriangles( tris );
	//Triangles merge_tri;
	//return merge_tri;

	return tri;
}
//////////////////////////////////////////////////////////////////////////
Pointfs AutoPolygon::mergePolygons( const Pointfss & _polygons ) const
{
	ClipperLib::Clipper clipper;

	for( Pointfss::const_iterator
		it = _polygons.begin(),
		it_end = _polygons.end();
	it != it_end;
	++it )
	{
		const Pointfs & pointfs = *it;

		ClipperLib::Path subj;
		ClipperLib::PolyTree out;

		for( Pointfs::const_iterator it = pointfs.begin(); it < pointfs.end(); it++ )
		{
			const Pointf & p = *it;

			subj << ClipperLib::IntPoint( (ClipperLib::cInt)(p.x * (double)PRECISION), (ClipperLib::cInt)(p.y * (double)PRECISION) );
		}

		clipper.AddPath( subj, ClipperLib::ptSubject, true );
	}
	
	ClipperLib::PolyTree solution;
	clipper.Execute( ClipperLib::ctUnion, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero );

	ClipperLib::PolyNode* p = solution.GetFirst();

	if( p == nullptr )
	{
		return Pointfs();
	}

	while( p->IsHole() )
	{
		p = p->GetNext();
	}

	Pointfs outPoints;

	std::vector<ClipperLib::IntPoint>::const_iterator end = p->Contour.end();
	for( std::vector<ClipperLib::IntPoint>::const_iterator pt = p->Contour.begin(); pt < end; pt++ )
	{
		double x = (double)pt->X / (double)PRECISION;
		double y = (double)pt->Y / (double)PRECISION;

		outPoints.push_back( Pointf( x, y ) );
	}

	////turn the result into simply polygon (AKA, fix overlap)

	////clamp into the specified rect
	//ClipperLib::Clipper cl;
	//cl.StrictlySimple( true );
	//cl.AddPath( p->Contour, ClipperLib::ptSubject, true );
	////create the clipping rect
	//ClipperLib::Path clamp;
	//clamp.push_back( ClipperLib::IntPoint( 0, 0 ) );
	//clamp.push_back( ClipperLib::IntPoint( (ClipperLib::cInt)(m_width * PRECISION), 0 ) );
	//clamp.push_back( ClipperLib::IntPoint( (ClipperLib::cInt)(m_width * PRECISION), (ClipperLib::cInt)(m_height * PRECISION) ) );
	//clamp.push_back( ClipperLib::IntPoint( 0, (ClipperLib::cInt)(m_height * PRECISION) ) );
	//cl.AddPath( clamp, ClipperLib::ptClip, true );

	//ClipperLib::PolyTree out;
	//cl.Execute( ClipperLib::ctIntersection, out );

	//ClipperLib::PolyNode* p2 = out.GetFirst();
	//while( p2->IsHole() )
	//{
	//	p2 = p2->GetNext();
	//}

	//Pointfs outPoints;

	//std::vector<ClipperLib::IntPoint>::const_iterator end = p2->Contour.end();
	//for( std::vector<ClipperLib::IntPoint>::const_iterator pt = p2->Contour.begin(); pt < end; pt++ )
	//{
	//	double x = (double)pt->X / (double)PRECISION;
	//	double y = (double)pt->Y / (double)PRECISION;

	//	outPoints.push_back( Pointf( x, y ) );
	//}

	return outPoints;
}
//////////////////////////////////////////////////////////////////////////
Triangles AutoPolygon::mergeTriangles( const Triangless & _tris ) const
{
	Triangles merge_tri;

	uint32_t merge_vertex_count = 0;
	uint32_t merge_indices_count = 0;

	for( Triangless::const_iterator
		it = _tris.begin(),
		it_end = _tris.end();
	it != it_end;
	++it )
	{
		const Triangles & tri = *it;

		merge_vertex_count += tri.vertCount;
		merge_indices_count += tri.indexCount;
	}

	merge_tri.vertCount = merge_vertex_count;
	merge_tri.indexCount = merge_indices_count;
	merge_tri.verts = new Vertex[merge_vertex_count];	
	merge_tri.indices = new uint16_t[merge_indices_count];

	uint32_t verts_offset = 0;
	uint32_t indices_offset = 0;

	for( Triangless::const_iterator
		it = _tris.begin(),
		it_end = _tris.end();
	it != it_end;
	++it )
	{
		const Triangles & tri = *it;

		for( uint32_t i = 0; i != tri.vertCount; ++i )
		{
			merge_tri.verts[verts_offset + i] = tri.verts[i];
		}

		for( uint32_t i = 0; i != tri.indexCount; ++i )
		{
			merge_tri.indices[indices_offset + i] = tri.indices[i] + (uint16_t)verts_offset;
		}

		verts_offset += tri.vertCount;
		indices_offset += tri.indexCount;
	}

	return merge_tri;
}