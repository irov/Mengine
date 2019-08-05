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
        : m_enumerator( 0 )
        , m_invalidateLocalVertex2D( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Vectorizator::~Vectorizator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Vectorizator::addLine( const mt::vec2f & _from, const mt::vec2f & _to, float _weight, const Color & _color )
    {
        uint32_t id = ++m_enumerator;

        LineDesc desc;
        desc.id = id;
        desc.from = _from;
        desc.to = _to;
        desc.weight = _weight;
        desc.color = _color;

        m_lines.emplace_back( desc );

        m_invalidateLocalVertex2D = true;

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::removeLine( uint32_t _id )
    {
        VectorLines::iterator it_found = std::find_if( m_lines.begin(), m_lines.end(), [_id]( const LineDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION( it_found != m_lines.end() );

        m_lines.erase( it_found );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Vectorizator::addQuadraticBezier( const mt::vec2f & _from, const mt::vec2f & _to, const mt::vec2f & _p0, uint8_t _quality, float _weight, const Color & _color )
    {
        MENGINE_ASSERTION( _quality >= 3 && _quality <= 32 );

        uint32_t id = ++m_enumerator;

        QuadraticBezierDesc desc;
        desc.id = id;
        desc.from = _from;
        desc.to = _to;
        desc.p[0] = _p0;
        desc.quality = _quality;
        desc.weight = _weight;
        desc.color = _color;

        m_quadraticBeziers.emplace_back( desc );

        m_invalidateLocalVertex2D = true;

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::removeQuadraticBezier( uint32_t _id )
    {
        VectorQuadraticBeziers::iterator it_found = std::find_if( m_quadraticBeziers.begin(), m_quadraticBeziers.end(), [_id]( const QuadraticBezierDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION( it_found != m_quadraticBeziers.end() );

        m_quadraticBeziers.erase( it_found );

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

        uint32_t vertexSize = 0; 
        uint32_t indexSize = 0;

        vertexSize += m_lines.size() * 4;
        indexSize += m_lines.size() * 6;

        for( const QuadraticBezierDesc & bezier : m_quadraticBeziers )
        {
            vertexSize += bezier.quality * 2;
            indexSize += (bezier.quality - 1) * 6;
        }

        m_renderVertex2D.resize( vertexSize );
        m_renderIndices.resize( indexSize );

        uint16_t vertexIterator = 0;
        uint16_t indexIterator = 0;
        for( const LineDesc & line : m_lines )
        {
            mt::vec2f fromWM;
            mt::mul_v2_v2_m4( fromWM, line.from, wm );

            mt::vec2f toWM;
            mt::mul_v2_v2_m4( toWM, line.to, wm );

            mt::vec2f perp;
            makeLinePerp( perp, fromWM, toWM );

            Color line_color = color * line.color;
            uint32_t argb = line_color.getAsARGB();

            m_renderVertex2D[vertexIterator + 0].position.x = fromWM.x - perp.x * line.weight;
            m_renderVertex2D[vertexIterator + 0].position.y = fromWM.y - perp.y * line.weight;
            m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
            m_renderVertex2D[vertexIterator + 0].color = argb;

            m_renderVertex2D[vertexIterator + 1].position.x = toWM.x - perp.x * line.weight;
            m_renderVertex2D[vertexIterator + 1].position.y = toWM.y - perp.y * line.weight;
            m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
            m_renderVertex2D[vertexIterator + 1].color = argb;

            m_renderVertex2D[vertexIterator + 2].position.x = toWM.x + perp.x * line.weight;
            m_renderVertex2D[vertexIterator + 2].position.y = toWM.y + perp.y * line.weight;
            m_renderVertex2D[vertexIterator + 2].position.z = 0.f;
            m_renderVertex2D[vertexIterator + 2].color = argb;

            m_renderVertex2D[vertexIterator + 3].position.x = fromWM.x + perp.x * line.weight;
            m_renderVertex2D[vertexIterator + 3].position.y = fromWM.y + perp.y * line.weight;
            m_renderVertex2D[vertexIterator + 3].position.z = 0.f;
            m_renderVertex2D[vertexIterator + 3].color = argb;

            m_renderIndices[indexIterator + 0] = vertexIterator + 0;
            m_renderIndices[indexIterator + 1] = vertexIterator + 3;
            m_renderIndices[indexIterator + 2] = vertexIterator + 1;
            m_renderIndices[indexIterator + 3] = vertexIterator + 1;
            m_renderIndices[indexIterator + 4] = vertexIterator + 3;
            m_renderIndices[indexIterator + 5] = vertexIterator + 2;

            vertexIterator += 4;
            indexIterator += 6;
        }

        for( const QuadraticBezierDesc & bezier : m_quadraticBeziers )
        {
            Color bezier_color = color * bezier.color;
            uint32_t argb = bezier_color.getAsARGB();

            float dt = 1.f / (float)(bezier.quality - 1);
            float t = 0.f;

            mt::vec2f bp[32];
            for( uint8_t index = 0; index != bezier.quality; ++index )
            {
                Helper::calculateBezierPosition( bp[index], bezier.from, bezier.to, bezier.p, t );

                t += dt;
            }

            for( uint8_t index = 0; index != bezier.quality - 1; ++index )
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
                mt::vec2f fromWM;
                mt::mul_v2_v2_m4( fromWM, bp[0], wm );

                mt::vec2f toWM;
                mt::mul_v2_v2_m4( toWM, bp[bezier.quality - 1], wm );

                mt::vec2f perp;
                makeLinePerp( perp, fromWM, toWM );

                m_renderVertex2D[vertexIterator + 0].position.x = fromWM.x - perp.x * bezier.weight;
                m_renderVertex2D[vertexIterator + 0].position.y = fromWM.y - perp.y * bezier.weight;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb;

                m_renderVertex2D[vertexIterator + 1].position.x = fromWM.x + perp.x * bezier.weight;
                m_renderVertex2D[vertexIterator + 1].position.y = fromWM.y + perp.y * bezier.weight;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                vertexIterator += 2;
            }

            for( uint8_t index = 1; index != bezier.quality - 1; ++index )
            {
                mt::vec2f p0;
                mt::mul_v2_v2_m4( p0, bp[index - 1], wm );

                mt::vec2f p1;
                mt::mul_v2_v2_m4( p1, bp[index + 0], wm );

                mt::vec2f p2;
                mt::mul_v2_v2_m4( p2, bp[index + 1], wm );

                mt::vec2f perp;
                makeBendPerp( perp, p0, p1, p2 );

                m_renderVertex2D[vertexIterator + 0].position.x = p1.x - perp.x * bezier.weight;
                m_renderVertex2D[vertexIterator + 0].position.y = p1.y - perp.y * bezier.weight;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb;

                m_renderVertex2D[vertexIterator + 1].position.x = p1.x + perp.x * bezier.weight;
                m_renderVertex2D[vertexIterator + 1].position.y = p1.y + perp.y * bezier.weight;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                vertexIterator += 2;
            }

            {
                mt::vec2f fromWM;
                mt::mul_v2_v2_m4( fromWM, bp[0], wm );

                mt::vec2f toWM;
                mt::mul_v2_v2_m4( toWM, bp[bezier.quality - 1], wm );

                mt::vec2f perp;
                makeLinePerp( perp, fromWM, toWM );

                m_renderVertex2D[vertexIterator + 0].position.x = toWM.x - perp.x * bezier.weight;
                m_renderVertex2D[vertexIterator + 0].position.y = toWM.y - perp.y * bezier.weight;
                m_renderVertex2D[vertexIterator + 0].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 0].color = argb;

                m_renderVertex2D[vertexIterator + 1].position.x = toWM.x + perp.x * bezier.weight;
                m_renderVertex2D[vertexIterator + 1].position.y = toWM.y + perp.y * bezier.weight;
                m_renderVertex2D[vertexIterator + 1].position.z = 0.f;
                m_renderVertex2D[vertexIterator + 1].color = argb;

                vertexIterator += 2;
            }
        }
    }
}
