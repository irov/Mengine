#include "Unit.h"

#include "Plugins/GraphicsPlugin/GraphicsInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/ColorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Unit::Unit()
        : m_radius( 0.f )
        , m_moveDirection( 0.f, 0.f )
        , m_moveSpeed( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Unit::~Unit()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Unit::initialize( const Box2DWorldInterfacePtr & _world, const NodePtr & _root, const mt::vec2f & _position, const jpp::object & _data )
    {
        m_world = _world;
        m_root = _root;

        m_radius = _data["radius"];
        m_moveSpeed = _data["move_speed"];

        jpp::object data_collision = _data["collision"];

        Box2DBodyInterfacePtr body = this->makeBody( _position, data_collision );

        m_body = body;

        jpp::object data_render = _data["render"];

        NodePtr graphicsNode = this->makeRender( data_render );

        NodePtr phisicalPlaceholder = this->makePhysicalPlaceholder( m_body );

        phisicalPlaceholder->addChild( graphicsNode );

        m_node = phisicalPlaceholder;

        m_root->addChild( m_node );
    }
    //////////////////////////////////////////////////////////////////////////
    void Unit::finalize()
    {
        m_world = nullptr;
        m_root = nullptr;
        m_node = nullptr;
        m_body = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const Box2DWorldInterfacePtr & Unit::getWorld() const
    {
        return m_world;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Unit::getRoot() const
    {
        return m_root;
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DBodyInterfacePtr Unit::makeBody( const mt::vec2f & _position, const jpp::object & _data )
    {
        uint32_t collisionCategory = _data["category"];
        uint32_t collisionMask = _data["mask"];
        float linearDumping = _data.get( "linear_dumping", 0.f );
        float angularDamping = _data.get( "angular_Damping", 0.f );

        Box2DBodyInterfacePtr body = m_world->createBody(
            false
            , _position, 0.f, linearDumping, angularDamping
            , true, false, true
            , MENGINE_DOCUMENT_FUNCTION
        );

        body->addShapeCircle( m_radius, {0.f, 0.f}, 1.f, 0.99f, 0.1f, false, collisionCategory, collisionMask, 0 );

        return body;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Unit::makeRender( const jpp::object & _data )
    {
        NodePtr graphicsNode = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), MENGINE_DOCUMENT_FUNCTION );

        GraphicsInterface * graphics = graphicsNode->getUnknown();

        Color colorLine = _data["color_line"];
        Color colorFill = _data["color_fill"];

        graphics->setLineColor( colorLine );
        graphics->setLineWidth( 1.f );

        graphics->drawCircle( {0.f, 0.f}, m_radius );

        return graphicsNode;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Unit::makePhysicalPlaceholder( const Box2DBodyInterfacePtr & _body )
    {
        NodePtr phisicalPlaceholder = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PhysicalPlaceholder" ), MENGINE_DOCUMENT_FACTORABLE );

        UnknownPhysicalPlaceholderInterface * unknownPhisicalPlaceholder = phisicalPlaceholder->getUnknown();

        unknownPhisicalPlaceholder->setBox2DBody( _body );

        return phisicalPlaceholder;
    }
    //////////////////////////////////////////////////////////////////////////
    float Unit::getRadius() const
    {
        return m_radius;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Unit::getNode() const
    {
        return m_node;
    }
    //////////////////////////////////////////////////////////////////////////
    const Box2DBodyInterfacePtr & Unit::getBody() const
    {
        return m_body;
    }
    //////////////////////////////////////////////////////////////////////////
    void Unit::setMoveDirection( const mt::vec2f & _moveDirection )
    {
        if( mt::equal_v2_v2( m_moveDirection, _moveDirection ) == true )
        {
            return;
        }

        m_moveDirection = _moveDirection;

        mt::vec2f moveVelocity;
        mt::mul_v2_f( &moveVelocity, _moveDirection, m_moveSpeed );

        m_body->setBodyLinearVelocity( moveVelocity );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Unit::getMoveDirection() const
    {
        return m_moveDirection;
    }
    //////////////////////////////////////////////////////////////////////////
}