#include "Vectorizator.h"

#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/BezierHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/Document.h"

#include "math/line2.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Vectorizator::Vectorizator()
        : m_invalidateLocalVertex2D( false )
        , m_lineWidth( 2.f )
        , m_lineSoft( 1.f )
        , m_lineColor( 1.f, 1.f, 1.f, 1.f )
        , m_fillColor( 1.f, 1.f, 1.f, 1.f )
        , m_filling( false )
        , m_curveQuality( 20 )
        , m_ellipseQuality( 32 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Vectorizator::~Vectorizator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setLineWidth( float _width )
    {
        m_lineWidth = _width;
    }
    //////////////////////////////////////////////////////////////////////////
    float Vectorizator::getLineWidth() const
    {
        return m_lineWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setLineSoft( float _soft )
    {
        m_lineSoft = _soft;
    }
    //////////////////////////////////////////////////////////////////////////
    float Vectorizator::getLineSoft() const
    {
        return m_lineSoft;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setLineColor( const Color & _color )
    {
        m_lineColor = _color;
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & Vectorizator::getLineColor() const
    {
        return m_lineColor;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setCurveQuality( uint8_t _quality )
    {
        m_curveQuality = _quality;
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Vectorizator::getCurveQuality() const
    {
        return m_curveQuality;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setEllipseQuality( uint8_t _quality )
    {
        m_ellipseQuality = _quality;
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Vectorizator::getEllipseQuality() const
    {
        return m_ellipseQuality;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::beginFill( const Color & _color )
    {
        m_filling = true;
        m_fillColor = _color;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::endFill()
    {
        m_filling = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::moveTo( const mt::vec2f & _point )
    {
        LineDesc desc;
        desc.points.emplace_back( _point );
        desc.soft = m_lineSoft;

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
        edge.lineWidth = m_lineWidth;
        edge.lineColor = m_lineColor;

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
        edge.lineWidth = m_lineWidth;
        edge.lineColor = m_lineColor;

        desc.edges.emplace_back( edge );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::bezierCurveTo( const mt::vec2f & _p0, const mt::vec2f & _p1, const mt::vec2f & _point )
    {
        LineDesc & desc = m_lines.back();

        desc.points.emplace_back( _point );

        LineEdge edge;
        edge.controls = 2;
        edge.p[0] = _p0;
        edge.p[1] = _p1;
        edge.quality = m_curveQuality;
        edge.dt = 1.f / (float)(edge.quality - 1);
        edge.lineWidth = m_lineWidth;
        edge.lineColor = m_lineColor;

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
        desc.lineWidth = m_lineWidth;
        desc.lineSoft = m_lineSoft;
        desc.lineColor = m_lineColor;        
        desc.fillColor = m_fillColor;
        desc.filling = m_filling;

        m_rects.emplace_back( desc );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::drawCircle( const mt::vec2f & _point, float _radius )
    {
        this->drawEllipse( _point, _radius, _radius );
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::drawEllipse( const mt::vec2f & _point, float _width, float _height )
    {
        EllipseDesc desc;
        desc.point = _point;
        desc.width = _width;
        desc.height = _height;
        desc.quality = m_ellipseQuality;
        desc.lineWidth = m_lineWidth;
        desc.lineSoft = m_lineSoft;
        desc.lineColor = m_lineColor;
        desc.fillColor = m_fillColor;
        desc.filling = m_filling;

        m_ellipses.emplace_back( desc );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::clear()
    {
        m_lines.clear();
        m_rects.clear();
        m_ellipses.clear();

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        if( m_invalidateLocalVertex2D == true )
        {
            this->updateLocalVertex2D_();
        }

        if( m_renderIndices.empty() == true )
        {
            return;
        }

        RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
            ->getMaterial3( EM_COLOR_BLEND, PT_TRIANGLELIST, 0, nullptr, MENGINE_DOCUMENT_FACTORABLE );

        VectorRenderVertex2D::value_type * vertexData = m_renderVertex2D.data();
        VectorRenderVertex2D::size_type vertexSize = m_renderVertex2D.size();

        VectorRenderIndex::value_type * indexData = m_renderIndices.data();
        VectorRenderIndex::size_type indexSize = m_renderIndices.size();

        _renderPipeline->addRenderObject( _context, material, nullptr, vertexData, vertexSize, indexData, indexSize, nullptr, false, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::_invalidateColor()
    {
        BaseRender::_invalidateColor();

        m_invalidateLocalVertex2D = true;
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
    void Vectorizator::updateLocalVertex2D_() const
    {
        m_invalidateLocalVertex2D = false;

        Color color;
        this->calcTotalColor( color );

        uint32_t vertexSize = 0;
        uint32_t indexSize = 0;

        for( const LineDesc & line : m_lines )
        {
            if( line.edges.empty() == true )
            {
                continue;
            }

            uint16_t point_count = 0;

            for( const LineEdge & edge : line.edges )
            {
                point_count += edge.quality - 1;
            }

            point_count += 1;

            if( line.soft > 0.f )
            {
                vertexSize += point_count * 4;
                indexSize += (point_count - 1) * 18;
            }
            else
            {
                vertexSize += point_count * 2;
                indexSize += (point_count - 1) * 6;
            }
        }

        for( const RectDesc & desc : m_rects )
        {
            if( desc.lineSoft > 0.f )
            {
                vertexSize += 16;
                indexSize += 72;
            }
            else
            {
                vertexSize += 8;
                indexSize += 24;
            }

            if( desc.filling == true )
            {
                vertexSize += 4;
                indexSize += 6;
            }
        }

        for( const EllipseDesc & desc : m_ellipses )
        {
            if( desc.lineSoft > 0.f )
            {
                vertexSize += desc.quality * 4;
                indexSize += desc.quality * 18;
            }
            else
            {
                vertexSize += desc.quality * 2;
                indexSize += desc.quality * 6;
            }
        }

        m_renderVertex2D.resize( vertexSize );
        m_renderIndices.resize( indexSize );

        uint16_t vertexIterator = 0;
        uint16_t indexIterator = 0;

        for( const LineDesc & line : m_lines )
        {
            if( line.edges.empty() == true )
            {
                continue;
            }

            struct LinePoints
            {
                mt::vec2f pos;
                float width;
                ColorValue_ARGB argb;
            };

            typedef Vector<LinePoints> VectorLinePoints;

            VectorLinePoints points;

            uint32_t pointIterator = 0;

            for( const LineEdge & edge : line.edges )
            {
                const mt::vec2f & p0 = line.points[pointIterator++];
                const mt::vec2f & p1 = line.points[pointIterator];

                Color line_color = color * edge.lineColor;
                uint32_t argb = line_color.getAsARGB();

                switch( edge.controls )
                {
                case 0:
                    {
                        LinePoints lp0;
                        lp0.pos = p0;
                        lp0.width = edge.lineWidth;
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

                            LinePoints p;
                            p.pos = bp;
                            p.width = edge.lineWidth;
                            p.argb = argb;

                            points.emplace_back( p );
                        }
                    }break;
                case 2:
                    {
                        float t = 0.f;

                        for( uint8_t index = 0; index != edge.quality - 1; ++index )
                        {
                            mt::vec2f bp;
                            Helper::calculateBezierPosition<mt::vec2f, 2>( bp, p0, p1, edge.p, t );

                            t += edge.dt;

                            LinePoints p;
                            p.pos = bp;
                            p.width = edge.lineWidth;
                            p.argb = argb;

                            points.emplace_back( p );
                        }
                    }break;
                }
            }

            {
                const mt::vec2f & p1 = line.points[pointIterator];

                LinePoints lp1;
                lp1.pos = p1;
                lp1.width = line.edges.back().lineWidth;

                Color line_color = color * line.edges.back().lineColor;
                uint32_t argb = line_color.getAsARGB();

                lp1.argb = argb;

                points.emplace_back( lp1 );
            }

            VectorLinePoints::size_type pointSize = points.size();

            if( line.soft > 0.f )
            {
                for( uint16_t index = 0; index != pointSize - 1; ++index )
                {
                    m_renderIndices[indexIterator + 0] = vertexIterator + index * 4 + 0;
                    m_renderIndices[indexIterator + 1] = vertexIterator + index * 4 + 1;
                    m_renderIndices[indexIterator + 2] = vertexIterator + index * 4 + 4;
                    m_renderIndices[indexIterator + 3] = vertexIterator + index * 4 + 4;
                    m_renderIndices[indexIterator + 4] = vertexIterator + index * 4 + 1;
                    m_renderIndices[indexIterator + 5] = vertexIterator + index * 4 + 5;

                    indexIterator += 6;

                    m_renderIndices[indexIterator + 0] = vertexIterator + index * 4 + 1;
                    m_renderIndices[indexIterator + 1] = vertexIterator + index * 4 + 2;
                    m_renderIndices[indexIterator + 2] = vertexIterator + index * 4 + 5;
                    m_renderIndices[indexIterator + 3] = vertexIterator + index * 4 + 5;
                    m_renderIndices[indexIterator + 4] = vertexIterator + index * 4 + 2;
                    m_renderIndices[indexIterator + 5] = vertexIterator + index * 4 + 6;

                    indexIterator += 6;

                    m_renderIndices[indexIterator + 0] = vertexIterator + index * 4 + 2;
                    m_renderIndices[indexIterator + 1] = vertexIterator + index * 4 + 3;
                    m_renderIndices[indexIterator + 2] = vertexIterator + index * 4 + 6;
                    m_renderIndices[indexIterator + 3] = vertexIterator + index * 4 + 6;
                    m_renderIndices[indexIterator + 4] = vertexIterator + index * 4 + 3;
                    m_renderIndices[indexIterator + 5] = vertexIterator + index * 4 + 7;

                    indexIterator += 6;
                }
            }
            else
            {
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
            }

            {
                float width = points[0].width;
                uint32_t argb = points[0].argb;

                const mt::vec2f & p0 = points[0].pos;
                const mt::vec2f & p1 = points[1].pos;

                mt::vec2f perp;
                makeLinePerp( perp, p0, p1 );

                float line_width = width * 0.5f;

                if( line.soft > 0.f )
                {
                    float line_width_soft = (width - line.soft * 2.f) * 0.5f;

                    m_renderVertex2D[vertexIterator + 0].position.x = p0.x - perp.x * line_width;
                    m_renderVertex2D[vertexIterator + 0].position.y = p0.y - perp.y * line_width;
                    m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 0].color = argb & 0x00ffffff;

                    m_renderVertex2D[vertexIterator + 1].position.x = p0.x - perp.x * line_width_soft;
                    m_renderVertex2D[vertexIterator + 1].position.y = p0.y - perp.y * line_width_soft;
                    m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 1].color = argb;

                    m_renderVertex2D[vertexIterator + 2].position.x = p0.x + perp.x * line_width_soft;
                    m_renderVertex2D[vertexIterator + 2].position.y = p0.y + perp.y * line_width_soft;
                    m_renderVertex2D[vertexIterator + 2].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 2].color = argb;

                    m_renderVertex2D[vertexIterator + 3].position.x = p0.x + perp.x * line_width;
                    m_renderVertex2D[vertexIterator + 3].position.y = p0.y + perp.y * line_width;
                    m_renderVertex2D[vertexIterator + 3].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 3].color = argb & 0x00ffffff;

                    vertexIterator += 4;
                }
                else
                {
                    m_renderVertex2D[vertexIterator + 0].position.x = p0.x - perp.x * line_width;
                    m_renderVertex2D[vertexIterator + 0].position.y = p0.y - perp.y * line_width;
                    m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 0].color = argb;

                    m_renderVertex2D[vertexIterator + 1].position.x = p0.x + perp.x * line_width;
                    m_renderVertex2D[vertexIterator + 1].position.y = p0.y + perp.y * line_width;
                    m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 1].color = argb;

                    vertexIterator += 2;
                }
            }

            for( uint16_t index = 1; index != pointSize - 1; ++index )
            {
                float width = points[index + 0].width;
                ColorValue_ARGB argb = points[index + 0].argb;

                const mt::vec2f & p0 = points[index - 1].pos;
                const mt::vec2f & p1 = points[index + 0].pos;
                const mt::vec2f & p2 = points[index + 1].pos;

                float line_width = width * 0.5f;

                mt::vec2f perp01;
                makeLinePerp( perp01, p0, p1 );

                mt::vec2f perp12;
                makeLinePerp( perp12, p1, p2 );

                mt::line2f line01l;
                mt::line_from_two_point_v2( line01l, p0 - perp01 * line_width, p1 - perp01 * line_width );

                mt::line2f line12l;
                mt::line_from_two_point_v2( line12l, p1 - perp12 * line_width, p2 - perp12 * line_width );

                mt::vec2f pl;
                if( mt::line_intersect_v2( line01l, line12l, pl ) == false )
                {
                    pl = p1 - perp01 * line_width;
                }

                mt::line2f line01r;
                mt::line_from_two_point_v2( line01r, p0 + perp01 * line_width, p1 + perp01 * line_width );

                mt::line2f line12r;
                mt::line_from_two_point_v2( line12r, p1 + perp12 * line_width, p2 + perp12 * line_width );

                mt::vec2f pr;
                if( mt::line_intersect_v2( line01r, line12r, pr ) == false )
                {
                    pr = p1 + perp01 * line_width;
                }

                if( line.soft > 0.f )
                {
                    float line_width_soft = (width - line.soft * 2.f) * 0.5f;

                    mt::line2f line01l_soft;
                    mt::line_from_two_point_v2( line01l_soft, p0 - perp01 * line_width_soft, p1 - perp01 * line_width_soft );

                    mt::line2f line12l_soft;
                    mt::line_from_two_point_v2( line12l_soft, p1 - perp12 * line_width_soft, p2 - perp12 * line_width_soft );

                    mt::vec2f pl_soft;
                    if( mt::line_intersect_v2( line01l_soft, line12l_soft, pl_soft ) == false )
                    {
                        pl_soft = p1 - perp01 * line_width_soft;
                    }

                    mt::line2f line01r_soft;
                    mt::line_from_two_point_v2( line01r_soft, p0 + perp01 * line_width_soft, p1 + perp01 * line_width_soft );

                    mt::line2f line12r_soft;
                    mt::line_from_two_point_v2( line12r_soft, p1 + perp12 * line_width_soft, p2 + perp12 * line_width_soft );

                    mt::vec2f pr_soft;
                    if( mt::line_intersect_v2( line01r_soft, line12r_soft, pr_soft ) == false )
                    {
                        pr_soft = p1 + perp01 * line_width_soft;
                    }

                    m_renderVertex2D[vertexIterator + 0].position.x = pl.x;
                    m_renderVertex2D[vertexIterator + 0].position.y = pl.y;
                    m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 0].color = argb & 0x00ffffff;

                    m_renderVertex2D[vertexIterator + 1].position.x = pl_soft.x;
                    m_renderVertex2D[vertexIterator + 1].position.y = pl_soft.y;
                    m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 1].color = argb;

                    m_renderVertex2D[vertexIterator + 2].position.x = pr_soft.x;
                    m_renderVertex2D[vertexIterator + 2].position.y = pr_soft.y;
                    m_renderVertex2D[vertexIterator + 2].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 2].color = argb;

                    m_renderVertex2D[vertexIterator + 3].position.x = pr.x;
                    m_renderVertex2D[vertexIterator + 3].position.y = pr.y;
                    m_renderVertex2D[vertexIterator + 3].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 3].color = argb & 0x00ffffff;

                    vertexIterator += 4;
                }
                else
                {
                    m_renderVertex2D[vertexIterator + 0].position.x = pl.x;
                    m_renderVertex2D[vertexIterator + 0].position.y = pl.y;
                    m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 0].color = argb;

                    m_renderVertex2D[vertexIterator + 1].position.x = pr.x;
                    m_renderVertex2D[vertexIterator + 1].position.y = pr.y;
                    m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 1].color = argb;

                    vertexIterator += 2;
                }
            }

            {
                float width = points[pointSize - 2].width;
                uint32_t argb = points[pointSize - 2].argb;

                const mt::vec2f & p0 = points[pointSize - 2].pos;
                const mt::vec2f & p1 = points[pointSize - 1].pos;

                mt::vec2f perp;
                makeLinePerp( perp, p0, p1 );

                float line_width = width * 0.5f;

                if( line.soft > 0.f )
                {
                    float line_width_soft = (width - line.soft * 2.f) * 0.5f;

                    m_renderVertex2D[vertexIterator + 0].position.x = p1.x - perp.x * line_width;
                    m_renderVertex2D[vertexIterator + 0].position.y = p1.y - perp.y * line_width;
                    m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 0].color = argb & 0x00ffffff;

                    m_renderVertex2D[vertexIterator + 1].position.x = p1.x - perp.x * line_width_soft;
                    m_renderVertex2D[vertexIterator + 1].position.y = p1.y - perp.y * line_width_soft;
                    m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 1].color = argb;

                    m_renderVertex2D[vertexIterator + 2].position.x = p1.x + perp.x * line_width_soft;
                    m_renderVertex2D[vertexIterator + 2].position.y = p1.y + perp.y * line_width_soft;
                    m_renderVertex2D[vertexIterator + 2].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 2].color = argb;

                    m_renderVertex2D[vertexIterator + 3].position.x = p1.x + perp.x * line_width;
                    m_renderVertex2D[vertexIterator + 3].position.y = p1.y + perp.y * line_width;
                    m_renderVertex2D[vertexIterator + 3].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 3].color = argb & 0x00ffffff;

                    vertexIterator += 4;
                }
                else
                {
                    m_renderVertex2D[vertexIterator + 0].position.x = p1.x - perp.x * line_width;
                    m_renderVertex2D[vertexIterator + 0].position.y = p1.y - perp.y * line_width;
                    m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 0].color = argb;

                    m_renderVertex2D[vertexIterator + 1].position.x = p1.x + perp.x * line_width;
                    m_renderVertex2D[vertexIterator + 1].position.y = p1.y + perp.y * line_width;
                    m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 1].color = argb;

                    vertexIterator += 2;
                }
            }
        }

        for( const RectDesc & rect : m_rects )
        {
            Color line_color = color * rect.lineColor;
            uint32_t argb = line_color.getAsARGB();

            if( rect.lineSoft > 0.f )
            {
                for( uint16_t index = 0; index != 4; ++index )
                {
                    m_renderIndices[indexIterator + 0] = vertexIterator + (index * 4 + 0) % 16;
                    m_renderIndices[indexIterator + 1] = vertexIterator + (index * 4 + 1) % 16;
                    m_renderIndices[indexIterator + 2] = vertexIterator + (index * 4 + 4) % 16;
                    m_renderIndices[indexIterator + 3] = vertexIterator + (index * 4 + 4) % 16;
                    m_renderIndices[indexIterator + 4] = vertexIterator + (index * 4 + 1) % 16;
                    m_renderIndices[indexIterator + 5] = vertexIterator + (index * 4 + 5) % 16;

                    indexIterator += 6;

                    m_renderIndices[indexIterator + 0] = vertexIterator + (index * 4 + 1) % 16;
                    m_renderIndices[indexIterator + 1] = vertexIterator + (index * 4 + 2) % 16;
                    m_renderIndices[indexIterator + 2] = vertexIterator + (index * 4 + 5) % 16;
                    m_renderIndices[indexIterator + 3] = vertexIterator + (index * 4 + 5) % 16;
                    m_renderIndices[indexIterator + 4] = vertexIterator + (index * 4 + 2) % 16;
                    m_renderIndices[indexIterator + 5] = vertexIterator + (index * 4 + 6) % 16;

                    indexIterator += 6;

                    m_renderIndices[indexIterator + 0] = vertexIterator + (index * 4 + 2) % 16;
                    m_renderIndices[indexIterator + 1] = vertexIterator + (index * 4 + 3) % 16;
                    m_renderIndices[indexIterator + 2] = vertexIterator + (index * 4 + 6) % 16;
                    m_renderIndices[indexIterator + 3] = vertexIterator + (index * 4 + 6) % 16;
                    m_renderIndices[indexIterator + 4] = vertexIterator + (index * 4 + 3) % 16;
                    m_renderIndices[indexIterator + 5] = vertexIterator + (index * 4 + 7) % 16;

                    indexIterator += 6;
                }
            }
            else
            {
                for( uint16_t index = 0; index != 4; ++index )
                {
                    m_renderIndices[indexIterator + 0] = vertexIterator + (index * 2 + 0) % 8;
                    m_renderIndices[indexIterator + 1] = vertexIterator + (index * 2 + 1) % 8;
                    m_renderIndices[indexIterator + 2] = vertexIterator + (index * 2 + 2) % 8;
                    m_renderIndices[indexIterator + 3] = vertexIterator + (index * 2 + 2) % 8;
                    m_renderIndices[indexIterator + 4] = vertexIterator + (index * 2 + 1) % 8;
                    m_renderIndices[indexIterator + 5] = vertexIterator + (index * 2 + 3) % 8;

                    indexIterator += 6;
                }
            }

            mt::vec2f p0 = rect.point + mt::vec2f( 0.f, 0.f );
            mt::vec2f p1 = rect.point + mt::vec2f( rect.width, 0.f );
            mt::vec2f p2 = rect.point + mt::vec2f( rect.width, rect.height );
            mt::vec2f p3 = rect.point + mt::vec2f( 0.f, rect.height );

            float sqrt2_width = rect.lineWidth * 0.5f;

            if( rect.lineSoft > 0.f )
            {
                float sqrt2_width_soft = (rect.lineWidth - rect.lineSoft * 2.f) * 0.5f;

                m_renderVertex2D[vertexIterator + 0].position.x = p0.x - sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.y = p0.y - sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb & 0x00ffffff;

                m_renderVertex2D[vertexIterator + 1].position.x = p0.x - sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 1].position.y = p0.y - sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                m_renderVertex2D[vertexIterator + 2].position.x = p0.x + sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 2].position.y = p0.y + sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 2].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 2].color = argb;

                m_renderVertex2D[vertexIterator + 3].position.x = p0.x + sqrt2_width;
                m_renderVertex2D[vertexIterator + 3].position.y = p0.y + sqrt2_width;
                m_renderVertex2D[vertexIterator + 3].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 3].color = argb & 0x00ffffff;

                vertexIterator += 4;

                m_renderVertex2D[vertexIterator + 0].position.x = p1.x + sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.y = p1.y - sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb & 0x00ffffff;

                m_renderVertex2D[vertexIterator + 1].position.x = p1.x + sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 1].position.y = p1.y - sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                m_renderVertex2D[vertexIterator + 2].position.x = p1.x - sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 2].position.y = p1.y + sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 2].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 2].color = argb;

                m_renderVertex2D[vertexIterator + 3].position.x = p1.x - sqrt2_width;
                m_renderVertex2D[vertexIterator + 3].position.y = p1.y + sqrt2_width;
                m_renderVertex2D[vertexIterator + 3].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 3].color = argb & 0x00ffffff;

                vertexIterator += 4;

                m_renderVertex2D[vertexIterator + 0].position.x = p2.x + sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.y = p2.y + sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb & 0x00ffffff;

                m_renderVertex2D[vertexIterator + 1].position.x = p2.x + sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 1].position.y = p2.y + sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                m_renderVertex2D[vertexIterator + 2].position.x = p2.x - sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 2].position.y = p2.y - sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 2].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 2].color = argb;

                m_renderVertex2D[vertexIterator + 3].position.x = p2.x - sqrt2_width;
                m_renderVertex2D[vertexIterator + 3].position.y = p2.y - sqrt2_width;
                m_renderVertex2D[vertexIterator + 3].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 3].color = argb & 0x00ffffff;

                vertexIterator += 4;

                m_renderVertex2D[vertexIterator + 0].position.x = p3.x - sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.y = p3.y + sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb & 0x00ffffff;

                m_renderVertex2D[vertexIterator + 1].position.x = p3.x - sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 1].position.y = p3.y + sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                m_renderVertex2D[vertexIterator + 2].position.x = p3.x + sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 2].position.y = p3.y - sqrt2_width_soft;
                m_renderVertex2D[vertexIterator + 2].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 2].color = argb;

                m_renderVertex2D[vertexIterator + 3].position.x = p3.x + sqrt2_width;
                m_renderVertex2D[vertexIterator + 3].position.y = p3.y - sqrt2_width;
                m_renderVertex2D[vertexIterator + 3].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 3].color = argb & 0x00ffffff;

                vertexIterator += 4;
            }
            else
            {
                m_renderVertex2D[vertexIterator + 0].position.x = p0.x - sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.y = p0.y - sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb;

                m_renderVertex2D[vertexIterator + 1].position.x = p0.x + sqrt2_width;
                m_renderVertex2D[vertexIterator + 1].position.y = p0.y + sqrt2_width;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                vertexIterator += 2;

                m_renderVertex2D[vertexIterator + 0].position.x = p1.x + sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.y = p1.y - sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb;

                m_renderVertex2D[vertexIterator + 1].position.x = p1.x - sqrt2_width;
                m_renderVertex2D[vertexIterator + 1].position.y = p1.y + sqrt2_width;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                vertexIterator += 2;

                m_renderVertex2D[vertexIterator + 0].position.x = p2.x + sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.y = p2.y + sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb;

                m_renderVertex2D[vertexIterator + 1].position.x = p2.x - sqrt2_width;
                m_renderVertex2D[vertexIterator + 1].position.y = p2.y - sqrt2_width;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                vertexIterator += 2;

                m_renderVertex2D[vertexIterator + 0].position.x = p3.x - sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.y = p3.y + sqrt2_width;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb;

                m_renderVertex2D[vertexIterator + 1].position.x = p3.x + sqrt2_width;
                m_renderVertex2D[vertexIterator + 1].position.y = p3.y - sqrt2_width;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                vertexIterator += 2;
            }

            if( rect.filling == true )
            {
                m_renderIndices[indexIterator + 0] = vertexIterator + 0;
                m_renderIndices[indexIterator + 1] = vertexIterator + 1;
                m_renderIndices[indexIterator + 2] = vertexIterator + 3;
                m_renderIndices[indexIterator + 3] = vertexIterator + 3;
                m_renderIndices[indexIterator + 4] = vertexIterator + 1;
                m_renderIndices[indexIterator + 5] = vertexIterator + 2;

                indexIterator += 6;

                Color fill_color = color * rect.fillColor;
                uint32_t fill_argb = fill_color.getAsARGB();

                m_renderVertex2D[vertexIterator + 0].position.x = p0.x;
                m_renderVertex2D[vertexIterator + 0].position.y = p0.y;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = fill_argb;

                m_renderVertex2D[vertexIterator + 1].position.x = p1.x;
                m_renderVertex2D[vertexIterator + 1].position.y = p1.y;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = fill_argb;

                m_renderVertex2D[vertexIterator + 2].position.x = p2.x;
                m_renderVertex2D[vertexIterator + 2].position.y = p2.y;
                m_renderVertex2D[vertexIterator + 2].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 2].color = fill_argb;

                m_renderVertex2D[vertexIterator + 3].position.x = p3.x;
                m_renderVertex2D[vertexIterator + 3].position.y = p3.y;
                m_renderVertex2D[vertexIterator + 3].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 3].color = fill_argb;

                vertexIterator += 4;
            }
        }

        for( const EllipseDesc & ellipse : m_ellipses )
        {
            Color line_color = color * ellipse.lineColor;
            uint32_t argb = line_color.getAsARGB();

            if( ellipse.lineSoft > 0.f )
            {
                uint32_t ellipse_quality2 = ellipse.quality * 4;

                for( uint16_t index = 0; index != ellipse.quality; ++index )
                {
                    m_renderIndices[indexIterator + 0] = vertexIterator + (index * 4 + 0) % ellipse_quality2;
                    m_renderIndices[indexIterator + 1] = vertexIterator + (index * 4 + 1) % ellipse_quality2;
                    m_renderIndices[indexIterator + 2] = vertexIterator + (index * 4 + 4) % ellipse_quality2;
                    m_renderIndices[indexIterator + 3] = vertexIterator + (index * 4 + 4) % ellipse_quality2;
                    m_renderIndices[indexIterator + 4] = vertexIterator + (index * 4 + 1) % ellipse_quality2;
                    m_renderIndices[indexIterator + 5] = vertexIterator + (index * 4 + 5) % ellipse_quality2;

                    indexIterator += 6;

                    m_renderIndices[indexIterator + 0] = vertexIterator + (index * 4 + 1) % ellipse_quality2;
                    m_renderIndices[indexIterator + 1] = vertexIterator + (index * 4 + 2) % ellipse_quality2;
                    m_renderIndices[indexIterator + 2] = vertexIterator + (index * 4 + 5) % ellipse_quality2;
                    m_renderIndices[indexIterator + 3] = vertexIterator + (index * 4 + 5) % ellipse_quality2;
                    m_renderIndices[indexIterator + 4] = vertexIterator + (index * 4 + 2) % ellipse_quality2;
                    m_renderIndices[indexIterator + 5] = vertexIterator + (index * 4 + 6) % ellipse_quality2;

                    indexIterator += 6;

                    m_renderIndices[indexIterator + 0] = vertexIterator + (index * 4 + 2) % ellipse_quality2;
                    m_renderIndices[indexIterator + 1] = vertexIterator + (index * 4 + 3) % ellipse_quality2;
                    m_renderIndices[indexIterator + 2] = vertexIterator + (index * 4 + 6) % ellipse_quality2;
                    m_renderIndices[indexIterator + 3] = vertexIterator + (index * 4 + 6) % ellipse_quality2;
                    m_renderIndices[indexIterator + 4] = vertexIterator + (index * 4 + 3) % ellipse_quality2;
                    m_renderIndices[indexIterator + 5] = vertexIterator + (index * 4 + 7) % ellipse_quality2;

                    indexIterator += 6;
                }
            }
            else
            {
                uint32_t ellipse_quality2 = ellipse.quality * 2;

                for( uint16_t index = 0; index != ellipse.quality; ++index )
                {
                    m_renderIndices[indexIterator + 0] = vertexIterator + (index * 2 + 0) % ellipse_quality2;
                    m_renderIndices[indexIterator + 1] = vertexIterator + (index * 2 + 1) % ellipse_quality2;
                    m_renderIndices[indexIterator + 2] = vertexIterator + (index * 2 + 2) % ellipse_quality2;
                    m_renderIndices[indexIterator + 3] = vertexIterator + (index * 2 + 2) % ellipse_quality2;
                    m_renderIndices[indexIterator + 4] = vertexIterator + (index * 2 + 1) % ellipse_quality2;
                    m_renderIndices[indexIterator + 5] = vertexIterator + (index * 2 + 3) % ellipse_quality2;

                    indexIterator += 6;
                }
            }

            float line_width = ellipse.lineWidth * 0.5f;

            float dt = mt::constant::two_pi / float( ellipse.quality );

            float t = 0.f;

            for( uint32_t index = 0; index != ellipse.quality; ++index )
            {
                float ct = MT_cosf( t );
                float st = MT_sinf( t );

                float x0 = (ellipse.width + line_width) * ct;
                float y0 = (ellipse.height + line_width) * st;

                float x1 = (ellipse.width - line_width) * ct;
                float y1 = (ellipse.height - line_width) * st;

                if( ellipse.lineSoft > 0.f )
                {
                    float line_width_soft = (ellipse.lineWidth - ellipse.lineSoft * 2.f) * 0.5f;

                    float x0_soft = (ellipse.width + line_width_soft) * ct;
                    float y0_soft = (ellipse.height + line_width_soft) * st;

                    float x1_soft = (ellipse.width - line_width_soft) * ct;
                    float y1_soft = (ellipse.height - line_width_soft) * st;

                    m_renderVertex2D[vertexIterator + 0].position.x = ellipse.point.x + x0;
                    m_renderVertex2D[vertexIterator + 0].position.y = ellipse.point.y + y0;
                    m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 0].color = argb & 0x00ffffff;

                    m_renderVertex2D[vertexIterator + 1].position.x = ellipse.point.x + x0_soft;
                    m_renderVertex2D[vertexIterator + 1].position.y = ellipse.point.y + y0_soft;
                    m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 1].color = argb;

                    m_renderVertex2D[vertexIterator + 2].position.x = ellipse.point.x + x1_soft;
                    m_renderVertex2D[vertexIterator + 2].position.y = ellipse.point.y + y1_soft;
                    m_renderVertex2D[vertexIterator + 2].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 2].color = argb;

                    m_renderVertex2D[vertexIterator + 3].position.x = ellipse.point.x + x1;
                    m_renderVertex2D[vertexIterator + 3].position.y = ellipse.point.y + y1;
                    m_renderVertex2D[vertexIterator + 3].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 3].color = argb & 0x00ffffff;

                    vertexIterator += 4;
                }
                else
                {
                    m_renderVertex2D[vertexIterator + 0].position.x = ellipse.point.x + x0;
                    m_renderVertex2D[vertexIterator + 0].position.y = ellipse.point.y + y0;
                    m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 0].color = argb;

                    m_renderVertex2D[vertexIterator + 1].position.x = ellipse.point.x + x1;
                    m_renderVertex2D[vertexIterator + 1].position.y = ellipse.point.y + y1;
                    m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                    m_renderVertex2D[vertexIterator + 1].color = argb;

                    vertexIterator += 2;
                }

                t += dt;
            }
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        for( RenderVertex2D & vertex : m_renderVertex2D )
        {
            mt::vec3f pwm;
            mt::mul_v3_v3_m4( pwm, vertex.position, wm );

            vertex.position = pwm;
        }
    }
}
