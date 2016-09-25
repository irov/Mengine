#	pragma once

#	include <string>
#	include <vector>

#	include "Math\vec2.h"
#	include "Math\rect.h"


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

typedef std::vector<mt::vec2f> Points;

class AutoPolygon
{
public:
    AutoPolygon( const uint8_t * _data, uint32_t _width, uint32_t _height );
    ~AutoPolygon();
    
	Points trace(const mt::rectf & rect, float threshold = 0.0);
	Points reduce(const Points & points, const mt::rectf& rect, float epsilon = 2.0);
	Points expand(const Points & points, const mt::rectf& rect, float epsilon);
    
    Triangles triangulate(const Points& points);
    
    void calculateUV(const mt::rectf & rect, Vertex * verts, size_t count);

	Triangles generateTriangles(const mt::rectf & rect, float epsilon = 2.0, float threshold = 0.05);
    
protected:
    mt::vec2f findFirstNoneTransparentPixel(const mt::rectf & rect, float threshold) const;
    Points marchSquare(const mt::rectf & rect, const mt::vec2f & first, float threshold);
    uint32_t getSquareValue( uint32_t x, uint32_t y, const mt::rectf & rect, float threshold);

    uint8_t getAlphaByIndex( uint32_t i) const;
	uint8_t getAlphaByPos(const mt::vec2f & pos) const;

    int getIndexFromPos( uint32_t x, uint32_t y) const {return y*m_width+x;};
    mt::vec2f getPosFromIndex( uint32_t i) const {return mt::vec2f((float)(i%m_width), (float)(i/m_width));};

    Points rdp(const Points & v, float optimization);
    float perpendicularDistance(const mt::vec2f & i, const mt::vec2f & start, const mt::vec2f & end);

    //real rect is the size that is in scale with the texture file
    mt::rectf getRealRect(const mt::rectf & rect);
    
protected:
	const uint8_t * m_data;
    uint32_t m_width;
	uint32_t m_height;
    float m_scaleFactor;
	uint32_t m_threshold;
};
