#include "Vectorizator.h"

#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/BezierHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/Document.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Vectorizator::Vectorizator()
        : m_invalidateLocalVertex2D( false )
        , m_lineWeight( 0.f )
        , m_lineColor( 1.f, 1.f, 1.f, 1.f )
        , m_curveQuality( 10 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Vectorizator::~Vectorizator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setLineWeight( float _weight )
    {
        m_lineWeight = _weight;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setLineColor( const Color & _color )
    {
        m_lineColor = _color;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setCurveQuality( uint8_t _quality )
    {
        m_curveQuality = _quality;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::moveTo( const mt::vec2f & _point )
    {
        LineDesc desc;
        desc.points.emplace_back( _point );

        m_lines.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::lineTo( const mt::vec2f & _point )
    {
        LineDesc & desc = m_lines.back();

        desc.points.emplace_back( _point );

        LineEdge edge;
        edge.controls = 0;
        edge.quality = 2;
        edge.dt = 1.f;
        edge.weight = m_lineWeight;
        edge.color = m_lineColor;

        desc.edges.emplace_back( edge );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::quadraticCurveTo( const mt::vec2f & _p0, const mt::vec2f & _point )
    {
        LineDesc & desc = m_lines.back();

        desc.points.emplace_back( _point );

        LineEdge edge;
        edge.controls = 1;
        edge.p[0] = _p0;
        edge.quality = m_curveQuality;
        edge.dt = 1.f / (float)(edge.quality - 1);
        edge.weight = m_lineWeight;
        edge.color = m_lineColor;

        desc.edges.emplace_back( edge );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::drawRect( const mt::vec2f & _point, float _width, float _height )
    {
        RectDesc desc;
        desc.point = _point;
        desc.width = _width;
        desc.height = _height;
        desc.weight = m_lineWeight;
        desc.color = m_lineColor;

        m_rects.emplace_back( desc );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::render( const RenderContext * _context ) const
    {
        if( m_invalidateLocalVertex2D == true )
        {
            this->updateLocalVertex2D_();
        }

        RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
            ->getMaterial3( EM_COLOR_SOLID, PT_TRIANGLELIST, 0, nullptr, MENGINE_DOCUMENT_FUNCTION );

        VectorRenderVertex2D::value_type * vertexData = m_renderVertex2D.data();
        VectorRenderVertex2D::size_type vertexSize = m_renderVertex2D.size();

        VectorRenderIndex::value_type * indexData = m_renderIndices.data();
        VectorRenderIndex::size_type indexSize = m_renderIndices.size();
        
        this->addRenderObject( _context, material, nullptr, vertexData, vertexSize, indexData, indexSize, nullptr, false );
    }
    //////////////////////////////////////////////////////////////////////////
    static void makeLinePerp( mt::vec2f & _perp, const mt::vec2f & _from, const mt::vec2f & _to )
    {
        mt::vec2f dir;
        mt::sub_v2_v2( dir, _to, _from );

        mt::vec2f dir_norm;
        mt::norm_v2_v2( dir_norm, dir );

        mt::perp_v2( _perp, dir_norm );
    }
    //////////////////////////////////////////////////////////////////////////
    static void makeBendPerp( mt::vec2f & _perp, const mt::vec2f & _p0, const mt::vec2f & _p1, const mt::vec2f & _p2 )
    {
        mt::vec2f dir0;
        mt::sub_v2_v2( dir0, _p1, _p0 );

        mt::vec2f dir1;
        mt::sub_v2_v2( dir1, _p2, _p1 );

        mt::vec2f dir_norm;
        mt::norm_v2_v2( dir_norm, dir0 + dir1 );

        mt::perp_v2( _perp, dir_norm );
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::updateLocalVertex2D_() const
    {
        m_invalidateLocalVertex2D = false;

        const mt::mat4f & wm = this->getWorldMatrix();

        Color color;
        this->calcTotalColor( color );

        uint16_t pointSize = 0;

        for( const LineDesc & line : m_lines )
        {
            if( line.edges.empty() == true )
            {
                continue;
            }

            for( const LineEdge & edge : line.edges )
            {
                pointSize += edge.quality - 1;
            }

            pointSize += 1;
        }

        uint32_t vertexSize = pointSize * 2;
        uint32_t indexSize = (pointSize - 1) * 6;

        m_renderVertex2D.resize( vertexSize );
        m_renderIndices.resize( indexSize );

        uint16_t vertexIterator = 0;
        uint16_t indexIterator = 0;

        for( const LineDesc & line : m_lines )
        {
            struct LinePoints
            {
                mt::vec2f pos;
                float weight;
                ColorValue_ARGB argb;
            };

            typedef Vector<LinePoints> VectorLinePoints;

            VectorLinePoints points;
            points.reserve( pointSize );

            uint32_t pointIterator = 0;

            for( const LineEdge & edge : line.edges )
            {
                const mt::vec2f & p0 = line.points[pointIterator++];
                const mt::vec2f & p1 = line.points[pointIterator];

                Color line_color = color * edge.color;
                uint32_t argb = line_color.getAsARGB();

                switch( edge.controls )
                {
                case 0:
                    {
                        mt::vec2f p0_wm;
                        mt::mul_v2_v2_m4( p0_wm, p0, wm );

                        LinePoints lp0;
                        lp0.pos = p0_wm;
                        lp0.weight = edge.weight;
                        lp0.argb = argb;

                        points.emplace_back( lp0 );
                    }break;
                case 1:
                    {
                        float t = 0.f;

                        for( uint8_t index = 0; index != edge.quality - 1; ++index )
                        {
                            mt::vec2f bp;
                            Helper::calculateBezierPosition<mt::vec2f, 1>( bp, p0, p1, edge.p, t );

                            t += edge.dt;

                            mt::vec2f bp_wm;
                            mt::mul_v2_v2_m4( bp_wm, bp, wm );

                            LinePoints p;
                            p.pos = bp_wm;
                            p.weight = edge.weight;
                            p.argb = argb;

                            points.emplace_back( p );                            
                        }
                    }break;
                }
            }

            {
                const mt::vec2f & p1 = line.points[pointIterator];

                mt::vec2f p1_wm;
                mt::mul_v2_v2_m4( p1_wm, p1, wm );

                LinePoints lp1;
                lp1.pos = p1_wm;
                lp1.weight = line.edges.back().weight;

                Color line_color = color * line.edges.back().color;
                uint32_t argb = line_color.getAsARGB();

                lp1.argb = argb;

                points.emplace_back( lp1 );
            }

            for( uint16_t index = 0; index != pointSize - 1; ++index )
            {
                m_renderIndices[indexIterator + 0] = vertexIterator + index * 2 + 0;
                m_renderIndices[indexIterator + 1] = vertexIterator + index * 2 + 1;
                m_renderIndices[indexIterator + 2] = vertexIterator + index * 2 + 2;
                m_renderIndices[indexIterator + 3] = vertexIterator + index * 2 + 2;
                m_renderIndices[indexIterator + 4] = vertexIterator + index * 2 + 1;
                m_renderIndices[indexIterator + 5] = vertexIterator + index * 2 + 3;

                indexIterator += 6;
            }

            {
                float weight = points[0].weight;
                uint32_t argb = points[0].argb;

                const mt::vec2f & p0 = points[0].pos;
                const mt::vec2f & p1 = points[1].pos;

                mt::vec2f perp;
                makeLinePerp( perp, p0, p1 );

                m_renderVertex2D[vertexIterator + 0].position.x = p0.x - perp.x * weight;
                m_renderVertex2D[vertexIterator + 0].position.y = p0.y - perp.y * weight;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb;

                m_renderVertex2D[vertexIterator + 1].position.x = p0.x + perp.x * weight;
                m_renderVertex2D[vertexIterator + 1].position.y = p0.y + perp.y * weight;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                vertexIterator += 2;
            }

            for( uint16_t index = 1; index != pointSize - 1; ++index )
            {
                float weight = points[index + 0].weight;
                ColorValue_ARGB argb = points[index + 0].argb;

                const mt::vec2f & p0 = points[index - 1].pos;
                const mt::vec2f & p1 = points[index + 0].pos;
                const mt::vec2f & p2 = points[index + 1].pos;

                mt::vec2f perp;
                makeBendPerp( perp, p0, p1, p2 );

                m_renderVertex2D[vertexIterator + 0].position.x = p1.x - perp.x * weight;
                m_renderVertex2D[vertexIterator + 0].position.y = p1.y - perp.y * weight;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb;

                m_renderVertex2D[vertexIterator + 1].position.x = p1.x + perp.x * weight;
                m_renderVertex2D[vertexIterator + 1].position.y = p1.y + perp.y * weight;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                vertexIterator += 2;
            }

            {
                float weight = points[pointSize - 2].weight;
                uint32_t argb = points[pointSize - 2].argb;

                const mt::vec2f & p0 = points[pointSize - 2].pos;
                const mt::vec2f & p1 = points[pointSize - 1].pos;

                mt::vec2f perp;
                makeLinePerp( perp, p0, p1 );

                m_renderVertex2D[vertexIterator + 0].position.x = p0.x - perp.x * weight;
                m_renderVertex2D[vertexIterator + 0].position.y = p0.y - perp.y * weight;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb;

                m_renderVertex2D[vertexIterator + 1].position.x = p0.x + perp.x * weight;
                m_renderVertex2D[vertexIterator + 1].position.y = p0.y + perp.y * weight;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                vertexIterator += 2;
            }
        }
    }
}
