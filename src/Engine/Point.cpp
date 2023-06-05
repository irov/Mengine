#include "Point.h"

#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Point::Point()
        : m_owner( false )
        , m_width( 1.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Point::~Point()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Point::setLinkedPoint( const PointPtr & _linked )
    {
        if( m_linked != nullptr )
        {
            return;
        }

        m_linked = _linked;
        m_owner = true;

        _linked->m_linked = this;
        _linked->m_owner = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Point::removeLinkedPoint()
    {
        if( m_linked == nullptr )
        {
            return;
        }

        m_linked->m_linked = nullptr;
        m_linked->m_owner = false;

        m_linked = nullptr;
        m_owner = false;
    }
    //////////////////////////////////////////////////////////////////////////
    const PointPtr & Point::getLinkedPoint() const
    {
        return m_linked;
    }
    //////////////////////////////////////////////////////////////////////////
    void Point::setWidth( float _width )
    {
        m_width = _width;
    }
    //////////////////////////////////////////////////////////////////////////
    float Point::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    void Point::_dispose()
    {
        this->removeLinkedPoint();

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr Point::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = this->getMaterial3( EM_DEBUG, PT_LINELIST, 0, nullptr, MENGINE_DOCUMENT_FACTORABLE );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    void Point::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        if( m_linked == nullptr )
        {
            return;
        }

        if( m_owner == false )
        {
            return;
        }

        const mt::mat4f & wm1 = this->getWorldMatrix();
        const mt::mat4f & wm2 = m_linked->getWorldMatrix();

        mt::vec3f fromWM;
        mt::mul_v3_v3_m4( &fromWM, mt::vec3f( 0.f, 0.f, 0.f ), wm1 );

        mt::vec3f toWM;
        mt::mul_v3_v3_m4( &toWM, mt::vec3f( 0.f, 0.f, 0.f ), wm2 );

        mt::vec3f dir;
        mt::sub_v3_v3( &dir, toWM, fromWM );

        mt::vec3f dir_norm;
        mt::norm_v3_v3( &dir_norm, dir );

        mt::vec2f perp;
        mt::perp_v2( &perp, mt::vec2f( dir_norm.x, dir_norm.y ) );

        m_vertices[0].position.x = fromWM.x + perp.x * m_width;
        m_vertices[0].position.y = fromWM.y + perp.y * m_width;
        m_vertices[0].position.z = fromWM.z;

        m_vertices[1].position.x = toWM.x + perp.x * m_width;
        m_vertices[1].position.y = toWM.y + perp.y * m_width;
        m_vertices[1].position.z = toWM.z;

        m_vertices[2].position.x = toWM.x - perp.x * m_width;
        m_vertices[2].position.y = toWM.y - perp.y * m_width;
        m_vertices[2].position.z = toWM.z;

        m_vertices[3].position.x = fromWM.x - perp.x * m_width;
        m_vertices[3].position.y = fromWM.y - perp.y * m_width;
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
