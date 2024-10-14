#include "HeroBullet.h"

#include "Plugins/GraphicsPlugin/GraphicsInterface.h"

#include "Kernel/TimepipeHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/ColorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    HeroBullet::HeroBullet()
        : m_startPosition( 0.f, 0.f )
        , m_startDirection( 0.f, 0.f )
        , m_radius( 2.f )
        , m_speed( 2000.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    HeroBullet::~HeroBullet()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void HeroBullet::initialize( const HeroPtr & _hero, const mt::vec2f & _startPosition, const mt::vec2f & _startDirection )
    {
        m_hero = _hero;

        m_startPosition = _startPosition;
        m_startDirection = _startDirection;

        const Box2DWorldInterfacePtr & world = m_hero->getWorld();

        Box2DBodyInterfacePtr body = world->createBody(
            false
            , m_startPosition, 0.f, 0.f, 0.f
            , true, true, false
            , MENGINE_DOCUMENT_FUNCTION
        );

        body->setBodyLinearDamping( 1.f );

        body->addShapeCircle( m_radius, {0.f, 0.f}, 1.f, 0.99f, 0.1f, false, 0xFFFF, 0x00000003, 0 );

        mt::vec2f moveVelocity = m_startDirection * m_speed;

        body->setBodyLinearVelocity( moveVelocity );

        m_body = body;

        NodePtr graphicsNode = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), MENGINE_DOCUMENT_FUNCTION );

        GraphicsInterface * graphics = graphicsNode->getUnknown();

        Color colorLine = Helper::makeColor8( 0, 255, 0, 255 );
        Color colorFill = Helper::makeColor8( 255, 0, 0, 255 );

        graphics->setLineColor( colorLine );
        graphics->setLineWidth( 1.f );

        graphics->drawCircle( {0.f, 0.f}, m_radius );

        NodePtr phisicalPlaceholder = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PhysicalPlaceholder" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownPhysicalPlaceholderInterface * unknownPhisicalPlaceholder = phisicalPlaceholder->getUnknown();

        unknownPhisicalPlaceholder->setBox2DBody( body );

        phisicalPlaceholder->addChild( graphicsNode );

        m_node = phisicalPlaceholder;

        const NodePtr & root = m_hero->getRoot();

        root->addChild( m_node );
    }
    //////////////////////////////////////////////////////////////////////////
    void HeroBullet::finalize()
    {
        m_hero = nullptr;

        m_node = nullptr;
        m_body = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}