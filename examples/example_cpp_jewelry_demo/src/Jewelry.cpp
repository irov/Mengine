#include "Jewelry.h"

#include "Engine/Engine.h"

#include "Engine/SurfaceSolidColor.h"
#include "Engine/ShapeCircle.h"
#include "Engine/HotSpotCircle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Jewelry::Jewelry()
        : m_size( 50.f )
        , m_iterator( 0 )
        , m_line( 0 )
        , m_type( 0 )
        , m_dead( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Jewelry::~Jewelry()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::setIterator( uint32_t _iterator )
    {
        m_iterator = _iterator;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Jewelry::getIterator() const
    {
        return m_iterator;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Jewelry::isDead() const
    {
        return m_dead;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Jewelry::initialize( uint32_t _line, uint32_t _type )
    {
        m_line = _line;
        m_type = _type;

        SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FUNCTION );

        Color colors[6] = {
            Color( 1.f, 0.f, 0.f, 1.f ), Color( 0.f, 1.f, 0.f, 1.f ), Color( 0.f, 0.f, 1.f, 1.f ),
            Color( 1.f, 1.f, 0.f, 1.f ), Color( 0.f, 1.f, 1.f, 1.f ), Color( 1.f, 0.f, 1.f, 1.f )
        };

        Color jewelry_color = colors[_type];

        surface->setSolidColor( jewelry_color );

        float width = m_size * 0.5f;
        surface->setSolidSize( {width, width} );

        ShapeCirclePtr shape = Helper::generateShapeCircle( MENGINE_DOCUMENT_FUNCTION );

        shape->setSurface( surface );

        HotSpotCirclePtr hotspot = Helper::generateHotSpotCircle( MENGINE_DOCUMENT_FUNCTION );

        hotspot->setRadius( 25.f );

        shape->addChild( hotspot );

        m_node = shape;
        m_pickerable = hotspot;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::finalize()
    {
        m_dead = true;

        m_node->removeFromParent();
        m_node = nullptr;

        m_pickerable = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Jewelry::getLine() const
    {
        return m_line;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Jewelry::getType() const
    {
        return m_type;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Jewelry::getNode() const
    {
        return m_node;
    }
    //////////////////////////////////////////////////////////////////////////
    const PickerablePtr & Jewelry::getPickerable() const
    {
        return m_pickerable;
    }
}