#   include "AutoPolygon.h"

#   include "Math/utils.h"

#   include "poly2tri/poly2tri.h"
#   include "clipper.hpp"

#   include <algorithm>
#   include <math.h>

const static float PRECISION = 10.0f;
//////////////////////////////////////////////////////////////////////////
AutoPolygon::AutoPolygon( const uint8_t * _data, uint32_t _width, uint32_t _height )
    : m_data( _data )
    , m_width( _width )
    , m_height( _height )
    , m_scaleFactor( 1.f )
    , m_threshold( 0 )
{
}
//////////////////////////////////////////////////////////////////////////
AutoPolygon::~AutoPolygon()
{
}
//////////////////////////////////////////////////////////////////////////
Points AutoPolygon::trace( const mt::rectf & rect, float threshold )
{
    mt::vec2f first = this->findFirstNoneTransparentPixel( rect, threshold );
    return this->marchSquare( rect, first, threshold );
}
//////////////////////////////////////////////////////////////////////////
mt::vec2f AutoPolygon::findFirstNoneTransparentPixel( const mt::rectf & rect, float threshold ) const
{
    bool found = false;
    mt::vec2f i;
    for( i.y = rect.y; i.y < rect.y + rect.height; i.y++ )
    {
        if( found )break;
        for( i.x = rect.x; i.x < rect.x + rect.width; i.x++ )
        {
            uint8_t alpha = this->getAlphaByPos( i );
            if( alpha > threshold )
            {
                found = true;
                break;
            }
        }
    }

    return i;
}
//////////////////////////////////////////////////////////////////////////
uint8_t AutoPolygon::getAlphaByIndex( uint32_t i ) const
{
    return *(m_data + i * 4 + 3);
}
//////////////////////////////////////////////////////////////////////////
uint8_t AutoPolygon::getAlphaByPos( const mt::vec2f& pos ) const
{
    return *(m_data + ((size_t)pos.y * m_width + (size_t)pos.x) * 4 + 3);
}
//////////////////////////////////////////////////////////////////////////
uint32_t AutoPolygon::getSquareValue( uint32_t x, uint32_t y, const mt::rectf & rect, float threshold )
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
    mt::rectf fixedRect = mt::rectf( rect.x, rect.y, rect.width - 2.f, rect.height - 2.f );

    mt::vec2f tl( (float)x - 1.f, (float)y - 1.f );
    mt::vec2f tr( (float)x - 0.f, (float)y - 1.f );
    mt::vec2f bl( (float)x - 1.f, (float)y - 0.f );
    mt::vec2f br( (float)x - 0.f, (float)y - 0.f );

    uint32_t sv = 0;
    sv += (rect_contains_point( fixedRect, tl ) && this->getAlphaByPos( tl ) > threshold) ? 1 : 0;
    sv += (rect_contains_point( fixedRect, tr ) && this->getAlphaByPos( tr ) > threshold) ? 2 : 0;
    sv += (rect_contains_point( fixedRect, bl ) && this->getAlphaByPos( bl ) > threshold) ? 4 : 0;
    sv += (rect_contains_point( fixedRect, br ) && this->getAlphaByPos( br ) > threshold) ? 8 : 0;

    return sv;
}
//////////////////////////////////////////////////////////////////////////
Points AutoPolygon::marchSquare( const mt::rectf & rect, const mt::vec2f& start, float threshold )
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
            }break;
        }
        //little optimization
        // if previous direction is same as current direction,
        // then we should modify the last vec to current
        curx += stepx;
        cury += stepy;
        if( stepx == prevx && stepy == prevy )
        {
            _points.back().x = (float)(curx - rect.x) / m_scaleFactor;
            _points.back().y = (float)(rect.height - cury + rect.y) / m_scaleFactor;
        }
        else if( problem )
        {
            //TODO: we triangulation cannot work collinear points, so we need to modify same point a little
            //TODO: maybe we can detect if we go into a hole and coming back the hole, we should extract those points and remove them
            _points.push_back( mt::vec2f( (float)(curx - rect.x) / m_scaleFactor, (float)(rect.height - cury + rect.y) / m_scaleFactor ) );
        }
        else
        {
            _points.push_back( mt::vec2f( (float)(curx - rect.x) / m_scaleFactor, (float)(rect.height - cury + rect.y) / m_scaleFactor ) );
        }

        count++;
        prevx = stepx;
        prevy = stepy;
        problem = false;
    }
    while( curx != startx || cury != starty );
    return _points;
}
//////////////////////////////////////////////////////////////////////////
float AutoPolygon::perpendicularDistance( const mt::vec2f & i, const mt::vec2f & start, const mt::vec2f & end )
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
Points AutoPolygon::rdp( const Points& v, float optimization )
{
    if( v.size() < 3 )
        return v;

    size_t index = (size_t)-1;
    float dist = 0;
    //not looping first and last point
    for( size_t i = 1; i < v.size() - 1; i++ )
    {
        float cdist = this->perpendicularDistance( v[i], v.front(), v.back() );
        if( cdist > dist )
        {
            dist = cdist;
            index = i;
        }
    }
    if( dist > optimization )
    {
        Points::const_iterator begin = v.begin();
        Points::const_iterator end = v.end();
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
        ret.push_back( v.front() );
        ret.push_back( v.back() );
        return ret;
    }
}
//////////////////////////////////////////////////////////////////////////
Points AutoPolygon::reduce( const Points& points, const mt::rectf & rect, float epsilon )
{
    auto size = points.size();
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

    float maxEp = mt::min_f( rect.width, rect.height );
    float ep = mt::clamp( 0.f, epsilon, maxEp / m_scaleFactor / 2 );
    Points result = rdp( points, ep );

    mt::vec2f last = result.back();
    if( last.y > result.front().y && mt::length_v2_v2( last, result.front() ) < ep*0.5 )
    {
        result.front().y = last.y;
        result.pop_back();
    }
    return result;
}
//////////////////////////////////////////////////////////////////////////
Points AutoPolygon::expand( const Points& points, const mt::rectf &rect, float epsilon )
{
    auto size = points.size();
    // if there are less than 3 points, then we have nothing
    if( size < 3 )
    {
        return Points();
    }

    ClipperLib::Path subj;
    ClipperLib::PolyTree solution;
    ClipperLib::PolyTree out;

    for( Points::const_iterator it = points.begin(); it < points.end(); it++ )
    {
        subj << ClipperLib::IntPoint( (ClipperLib::cInt)(it->x* PRECISION), (ClipperLib::cInt)(it->y * PRECISION) );
    }

    ClipperLib::ClipperOffset co;
    co.AddPath( subj, ClipperLib::jtMiter, ClipperLib::etClosedPolygon );
    co.Execute( solution, epsilon * PRECISION );

    ClipperLib::PolyNode* p = solution.GetFirst();

    if( !p )
    {
        return points;
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
    clamp.push_back( ClipperLib::IntPoint( (ClipperLib::cInt)(rect.width / m_scaleFactor * PRECISION), 0 ) );
    clamp.push_back( ClipperLib::IntPoint( (ClipperLib::cInt)(rect.width / m_scaleFactor * PRECISION), (ClipperLib::cInt)(rect.height / m_scaleFactor * PRECISION) ) );
    clamp.push_back( ClipperLib::IntPoint( 0, (ClipperLib::cInt)(rect.height / m_scaleFactor * PRECISION) ) );
    cl.AddPath( clamp, ClipperLib::ptClip, true );
    cl.Execute( ClipperLib::ctIntersection, out );

    Points outPoints;
    ClipperLib::PolyNode* p2 = out.GetFirst();
    while( p2->IsHole() )
    {
        p2 = p2->GetNext();
    }
    auto end = p2->Contour.end();
    for( std::vector<ClipperLib::IntPoint>::const_iterator pt = p2->Contour.begin(); pt < end; pt++ )
    {
        outPoints.push_back( mt::vec2f( pt->X / PRECISION, pt->Y / PRECISION ) );
    }
    return outPoints;
}
//////////////////////////////////////////////////////////////////////////
Triangles AutoPolygon::triangulate( const Points& points )
{
    // if there are less than 3 points, then we can't triangulate
    if( points.size() < 3 )
    {
        return Triangles();
    }
    std::vector<p2t::Point*> p2points;
    for( Points::const_iterator it = points.begin(); it < points.end(); it++ )
    {
        p2t::Point * p = new (std::nothrow) p2t::Point( it->x, it->y );
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
    for( auto j : p2points )
    {
        delete j;
    };

    Triangles triangles = {verts, indices, vdx, idx};

    return triangles;
}
//////////////////////////////////////////////////////////////////////////
void AutoPolygon::calculateUV( const mt::rectf & rect, Vertex * verts, size_t count )
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
        float u = (vertex->pos.x*m_scaleFactor + rect.x) / texWidth;
        float v = (rect.y + rect.height - vertex->pos.y*m_scaleFactor) / texHeight;
        vertex->uv.x = u;
        vertex->uv.y = v;
    }
}
//////////////////////////////////////////////////////////////////////////
mt::rectf AutoPolygon::getRealRect( const mt::rectf & rect )
{
    mt::rectf realRect = rect;
    //check rect to see if its zero
    if( realRect.empty() == true )
    {
        //if the instance doesn't have width and height, then the whole operation is kaput
        realRect = mt::rectf( 0, 0, (float)m_width, (float)m_height);
    }
    else
    {
        //rect is specified, so convert to real rect
        realRect = rect;
    }
    return realRect;
}
//////////////////////////////////////////////////////////////////////////
Triangles AutoPolygon::generateTriangles( const mt::rectf & rect, float epsilon, float threshold )
{
    mt::rectf realRect = this->getRealRect( rect );
    Points p = this->trace( realRect, threshold );
    p = this->reduce( p, realRect, epsilon );
    p = this->expand( p, realRect, epsilon );
    Triangles tri = triangulate( p );
    calculateUV( realRect, tri.verts, tri.vertCount );
    return tri;
}
