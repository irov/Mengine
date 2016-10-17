#	pragma once

#	include <string>
#	include <vector>

#	include "Math\vec2.h"

#	include <stdint.h>

struct Vertex
{
	mt::vec2f pos;
	mt::vec2f uv;
};

struct Triangles
{
	Vertex * verts;
	uint16_t * indices;
	size_t vertCount;
	size_t indexCount;
};

typedef std::vector<Triangles> Triangless;

struct Rect
{
	Rect( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
		: x( x )
		, y( y )
		, width( width )
		, height( height )
	{
	}

	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
};

struct Point
{
	Point( uint32_t x, uint32_t y )
		: x(x)
		, y(y)
	{
	}

	uint32_t x;
	uint32_t y;

	bool operator == (const Point & _p) const
	{
		if( x != _p.x )
		{
			return false;
		}

		if( y != _p.y )
		{
			return false;
		}

		return true;
	}
};

struct Pointf
{
	Pointf( double x, double y )
		: x( x )
		, y( y )
	{
	}

	double x;
	double y;

	bool operator == (const Point & _p) const
	{
		if( x != _p.x )
		{
			return false;
		}

		if( y != _p.y )
		{
			return false;
		}

		return true;
	}
};

typedef std::vector<Pointf> Pointfs;
typedef std::vector<Pointfs> Pointfss;
typedef std::vector<Point> Points;

typedef std::vector<uint8_t> Mask;
typedef std::vector<Mask> Masks;

class AutoPolygon
{
public:
	AutoPolygon( const uint8_t * _data, uint32_t _width, uint32_t _height );
	~AutoPolygon();

	bool trace( const Rect & rect, const Masks & _excluded, Points & _points, Mask & _mask, uint8_t threshold = 0 ) const;
	Points reduce( const Points & points, const Rect & rect, float epsilon ) const;
	Pointfs expand( const Points & points, const Rect & rect, float epsilon ) const;

	Triangles triangulate( const Pointfs & points ) const;

	void calculateUV( const Rect & rect, Vertex * verts, size_t count ) const;

	Triangles generateTriangles( const Rect & rect, float epsilon, bool & overlaped, uint8_t threshold ) const;
	bool testOverlapTriangles( const Triangless & _tris ) const;
	Triangles mergeTriangles( const Triangless & _tris ) const;
	Pointfs mergePolygons( const Pointfss & _polygons ) const;

protected:
	bool findFirstNoneTransparentPixel( const Rect & rect, Point & _point, const Masks & _excluded, uint8_t threshold ) const;
	void marchExclude( const Rect & rect, const Point & first, Mask & _points, uint8_t threshold ) const;
	Points marchSquare( const Rect & rect, const Point & first, uint8_t threshold ) const;
	uint32_t getSquareValue( uint32_t x, uint32_t y, const Rect & rect, uint8_t threshold ) const;

	uint8_t getAlphaByIndex( uint32_t i ) const;
	uint8_t getAlphaByPos( const Point & p ) const;

	int getIndexFromPos( uint32_t x, uint32_t y ) const { return y*m_width + x; };
	
	Points rdp( const Points & v, float optimization ) const;
	float perpendicularDistance( const mt::vec2f & i, const mt::vec2f & start, const mt::vec2f & end ) const;

	//real rect is the size that is in scale with the texture file
	Rect getRealRect( const Rect & rect ) const;
	bool testExcludedPoint( const Masks & _excluded, const Point & _point ) const;

protected:
	const uint8_t * m_data;
	uint32_t m_width;
	uint32_t m_height;
};
