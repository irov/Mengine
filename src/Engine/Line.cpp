#include "Line.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Line::Line()
        : m_from( 0.f, 0.f, 0.f )
        , m_to( 0.f, 0.f, 0.f )
        , m_weight( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Line::~Line()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Line::setFrom( const mt::vec3f & _value )
    {
        m_from = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & Line::getFrom() const
    {
        return m_from;
    }
    //////////////////////////////////////////////////////////////////////////
    void Line::setTo( const mt::vec3f & _value )
    {
        m_to = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & Line::getTo() const
    {
        return m_to;
    }
    //////////////////////////////////////////////////////////////////////////
    void Line::setWeight( float _width )
    {
        m_weight = _width;
    }
    //////////////////////////////////////////////////////////////////////////
    float Line::getWeight() const
    {
        return m_weight;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr Line::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = this->getMaterial3( EM_DEBUG, PT_LINELIST, 0, nullptr, MENGINE_DOCUMENT_FACTORABLE );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    void Line::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::vec3f fromWM;
        mt::mul_v3_v3_m4( &fromWM, m_from, wm );

        mt::vec3f toWM;
        mt::mul_v3_v3_m4( &toWM, m_to, wm );

        mt::vec3f dir;
        mt::sub_v3_v3( &dir, toWM, fromWM );

        mt::vec3f dir_norm;
        mt::norm_v3_v3( &dir_norm, dir );

        mt::vec2f perp;
        mt::perp_v2( &perp, mt::vec2f( dir_norm.x, dir_norm.y ) );

        m_vertices[0].position.x = fromWM.x + perp.x * m_weight;
        m_vertices[0].position.y = fromWM.y + perp.y * m_weight;
        m_vertices[0].position.z = fromWM.z;

        m_vertices[1].position.x = toWM.x + perp.x * m_weight;
        m_vertices[1].position.y = toWM.y + perp.y * m_weight;
        m_vertices[1].position.z = toWM.z;

        m_vertices[2].position.x = toWM.x - perp.x * m_weight;
        m_vertices[2].position.y = toWM.y - perp.y * m_weight;
        m_vertices[2].position.z = toWM.z;

        m_vertices[3].position.x = fromWM.x - perp.x * m_weight;
        m_vertices[3].position.y = fromWM.y - perp.y * m_weight;
        m_vertices[3].position.z = fromWM.z;

        Color color;
        this->calcTotalColor( &color );

        ColorValue_ARGB argb = color.getAsARGB();

        for( uint32_t i = 0; i != 4; ++i )
        {
            m_vertices[i].color = argb;
            m_vertices[i].uv[0].x = 0.f;
            m_vertices[i].uv[0].y = 0.f;
            m_vertices[i].uv[1].x = 0.f;
            m_vertices[i].uv[1].y = 0.f;
        }

        const RenderMaterialInterfacePtr & material = this->getMaterial();

        _renderPipeline->addRenderQuad( _context, material, m_vertices, 4, nullptr, false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
}