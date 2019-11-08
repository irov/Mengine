#include "Jewelry.h"

#include "Engine/Engine.h"

#include "Engine/SurfaceSolidColor.h"
#include "Engine/ShapeCircle.h"
#include "Engine/ShapeQuadFixed.h"
#include "Engine/HotSpotCircle.h"

#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const Color jewelry_colors[9] = {
        Color( 1.f, 0.f, 0.f, 1.f ), Color( 0.f, 1.f, 0.f, 1.f ), Color( 0.f, 0.f, 1.f, 1.f ),
        Color( 1.f, 1.f, 0.f, 1.f ), Color( 0.f, 1.f, 1.f, 1.f ), Color( 1.f, 0.f, 1.f, 1.f ),
        Color( 1.f, 0.5f, 0.5f, 1.f ), Color( 0.5f, 1.f, 1.f, 1.f ), Color( 1.f, 0.5f, 1.f, 1.f )
    };
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
    void Jewelry::setDead( bool _dead )
    {
        m_dead = _dead;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Jewelry::isDead() const
    {
        return m_dead;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::makeNodeActive_()
    {
        SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FUNCTION );

        Color jewelry_color = jewelry_colors[m_type];

        surface->setSolidColor( jewelry_color );

        float width = m_size * 0.5f;
        surface->setSolidSize( { width, width } );

        ShapeCirclePtr shape = Helper::generateShapeCircle( MENGINE_DOCUMENT_FUNCTION );

        shape->setSurface( surface );

        m_node->addChild( shape );

        m_nodeActive = shape;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::makeNodeBlock_()
    {
        SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FUNCTION );

        Color jewelry_color = jewelry_colors[m_type] * 0.5f;

        surface->setSolidColor( jewelry_color );

        float full_size = m_size + 10.f;
        surface->setSolidSize( { full_size, full_size } );

        ShapeQuadFixedPtr shape = Helper::generateShapeQuadFixed( MENGINE_DOCUMENT_FUNCTION );

        shape->setSurface( surface );

        shape->setLocalOrigin( { full_size * 0.5f, full_size * 0.5f, 0.f } );

        shape->disable();

        m_node->addChild( shape );

        m_nodeBlock = shape;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::makePickerable_()
    {
        HotSpotCirclePtr hotspot = Helper::generateHotSpotCircle( MENGINE_DOCUMENT_FUNCTION );

        float radius = m_size * 0.5f;
        hotspot->setRadius( radius );

        m_node->addChild( hotspot );

        m_pickerable = hotspot;
    }
    //////////////////////////////////////////////////////////////////////////
    void Jewelry::block()
    {
        m_nodeBlock->enable();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Jewelry::isBlock() const
    {
        return m_nodeBlock->isEnable();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Jewelry::initialize( uint32_t _line, uint32_t _type )
    {
        m_line = _line;
        m_type = _type;

        NodePtr node = Helper::generateNode( MENGINE_DOCUMENT_FUNCTION );

        m_node = node;

        this->makeNodeActive_();
        this->makeNodeBlock_();
        this->makePickerable_();

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
    const NodePtr & Jewelry::getNodeActive() const
    {
        return m_nodeActive;
    }
    //////////////////////////////////////////////////////////////////////////
    const PickerablePtr & Jewelry::getPickerable() const
    {
        return m_pickerable;
    }
}