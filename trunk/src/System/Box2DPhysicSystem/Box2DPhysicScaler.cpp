#	include	"Box2DPhysicScaler.h"
#	include "../../Interface/PhysicSystem2DInterface.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Box2DPhysicScaler::Box2DPhysicScaler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DPhysicScaler::~Box2DPhysicScaler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicScaler::setLimits( const mt::vec2f& _limits )
    {
        m_limits = _limits;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f& Box2DPhysicScaler::getLimits( ) const
    {
        return m_limits;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DPhysicScaler::scaleToBox2D( float _value ) const
    {
        return _value * Menge::physicsScaler;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DPhysicScaler::scaleToMenge( float _value ) const
    {
        return _value * Menge::oneDivPhysicsScaler;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DPhysicScaler::convertX( float _x ) const
    {
        return -1 * _x;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DPhysicScaler::convertY( float _y ) const
    {
        return m_limits.y - _y;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicScaler::scaleVectorToMenge( mt::vec2f& _point ) const
    {
        _point.x = scaleToMenge(_point.x);
        _point.y = scaleToMenge(_point.y);
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicScaler::scaleVectorToMenge( b2Vec2& _point ) const
    {
        _point.x = scaleToMenge(_point.x);
        _point.y = scaleToMenge(_point.y);
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicScaler::scalePositionToMenge(  mt::vec2f& _point ) const
    {
        _point.x = convertX( scaleToMenge( _point.x ) );
        _point.y = convertY( scaleToMenge( _point.y ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicScaler::scalePositionToMenge(  b2Vec2& _point ) const
    {
        _point.x = convertX( scaleToMenge( _point.x ) );
        _point.y = convertY( scaleToMenge( _point.y ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicScaler::scalePositionToBox2D( mt::vec2f& _point ) const
    {
        _point.x = convertX( scaleToBox2D(_point.x) );
        _point.y = scaleToBox2D( convertY( _point.y ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicScaler::scalePositionToBox2D( b2Vec2& _point ) const
    {
        _point.x = convertX( scaleToBox2D(_point.x) );
        _point.y = scaleToBox2D( convertY(_point.y) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicScaler::scaleVectorToBox2D( mt::vec2f& _point ) const
    {
        _point.x = scaleToBox2D(_point.x);
        _point.y = scaleToBox2D(_point.y);
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicScaler::scaleVectorToBox2D( b2Vec2& _point ) const
    {
        _point.x = scaleToBox2D(_point.x);
        _point.y = scaleToBox2D(_point.y);
    }
    //////////////////////////////////////////////////////////////////////////
}