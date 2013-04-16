#	include "Box2DPhysicBody.h"
#	include "Box2DPhysicScaler.h"
//#	include <boost/geometry/strategies/strategies.hpp>
//// boost centroid  is using now. 
//mt::vec2f getPolygonCentroid( const Menge::Polygon& _verticesCCW )
//{
//	size_t numpoints = boost::geometry::num_points( _verticesCCW );
//
//	float area = 0;
//
//	int i = 0;
//	int j = numpoints - 1;
//	for( unsigned int k = 0; k!= numpoints; ++k )
//	{
//		mt::vec2f p1 = _verticesCCW.outer()[i];
//		mt::vec2f p2 = _verticesCCW.outer()[j];
//		area += (p1.x*p2.y);
//		area -= p1.y*p2.x;
//		j = i;
//		i += 1;
//	}
//	area/=2;
//	int x = 0;
//	int y = 0;
//	float f;
//	j = numpoints - 1;
//	i = 0;
//
//	for( unsigned int k = 0; k!= numpoints; ++k )
//	{
//
//		mt::vec2f p1 = _verticesCCW.outer()[i];
//		mt::vec2f p2 = _verticesCCW.outer()[j];
//
//		f = p1.x*p2.y - p2.x*p1.y;
//		x += (p1.x+p2.x)*f;
//		y += (p1.y+p2.y)*f;
//		j=i;
//		i+=1;
//	}
//	f = area * 6;
//
//	mt::vec2f center( x/f , y/f ); 
//	return center;
//}
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    Box2DPhysicBody::Box2DPhysicBody( b2World* _world )
        : m_world(_world)
        , m_listener(0)
        , m_body(0)
        , m_scaler(NULL)
        , m_boundingBox( mt::vec2f(0,0), mt::vec2f(0,0))
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DPhysicBody::~Box2DPhysicBody()
    {
        if( m_world && m_body )
        {
            m_world->DestroyBody( m_body );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DPhysicBody::initialize( const b2BodyDef& _bodyDef, Box2DPhysicScaler * _scaler )
    {
        if( m_world == 0 )
        {
            return false;
        }
        m_scaler = _scaler;
        m_body = m_world->CreateBody( &_bodyDef );
        static b2Vec2 pos(0.0f, 0.0f);
        pos = m_body->GetPosition();
        printf(  "Box2DPhysicBody::initialize  %f  %f \n",pos.x,pos.y );

        if( m_body == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::box2f& Box2DPhysicBody::getBoundingBox()
    {
        _updateBoundingBox();
        return m_boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::_updateBoundingBox()
    {
        //const b2Transform& xf = m_body->GetTransform();
        b2AABB aabb;
        aabb.lowerBound = b2Vec2(1000,1000);
        aabb.upperBound = b2Vec2(-1000,-1000); 
        for (b2Fixture* fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            aabb.Combine(aabb, fixture->GetAABB(0));
            //int x =1;
        }
        m_boundingBox.minimum.x = m_scaler->scaleToMenge( aabb.lowerBound.x );
        m_boundingBox.minimum.y = m_scaler->scaleToMenge( aabb.lowerBound.y );
        m_boundingBox.maximum.x = m_scaler->scaleToMenge( aabb.upperBound.x );
        m_boundingBox.maximum.y = m_scaler->scaleToMenge( aabb.upperBound.y );
//#ifndef PHYSIC_DEBUG
        //return;
//#endif
        //printf(" minimum :  (x :%4.2f , y: %4.2f) maximum (x: %4.2f ,y: %4.2f) \n" ,m_boundingBox.minimum.x , m_boundingBox.minimum.y , m_boundingBox.maximum.x , m_boundingBox.maximum.y);
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::addShapeConvex( Menge::Polygon _vertices, float _density, float _friction, float _restitution, bool _isSensor,
        unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
    {
        b2PolygonShape shape;
        size_t numpoints = boost::geometry::num_points(_vertices);

        Menge::Polygon verticesCCW;
        for( unsigned int j = 0; j != numpoints; ++j )
        {
            mt::vec2f vec  = _vertices.outer()[j];
            vec.x*=-1;
            verticesCCW.outer().push_back(vec);
        }

        mt::vec2f center;
        boost::geometry::centroid<Menge::Polygon,mt::vec2f>(verticesCCW ,center );

        b2Vec2 vertices[8];

        float pointX, pointY;
        for( unsigned int i = 0; i != numpoints; ++i )
        {
            //float x = verticesCCW.outer()[i].x;
            //float y = verticesCCW.outer()[i].y;
            pointX =   ( verticesCCW.outer()[i].x - center.x );
            pointY =   ( center.y  - verticesCCW.outer()[i].y );

            pointX =  m_scaler->scaleToBox2D(pointX );
            pointY =  m_scaler->scaleToBox2D( pointY );

            vertices[i].Set( pointX, pointY );
        }
        /*
        vertices[0].Set(-40.0f, -40.0f);
        vertices[1].Set(40.0f, -40.0f);
        vertices[2].Set(40.0f, 40.0f);
        vertices[3].Set(-40.0f, 40.0f);
        vertices[4].Set(80.0f, 0.0f);
        for ( int i =0; i<8; ++i)
        {
        m_scaler->scaleVectorToBox2D(vertices[i]);
        }
        */
        shape.Set(vertices , numpoints);

        b2FixtureDef fd;

        fd.shape = &shape;
        fd.density = _density;
        fd.friction = _friction;
        fd.filter.categoryBits = _categoryBits;
        fd.filter.maskBits =_collisionMask;
        fd.filter.groupIndex = _groupIndex;
        fd.restitution = _restitution;
        fd.isSensor = _isSensor;

        //density as second argument?
        m_body->CreateFixture( &fd );

    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::addShapeCircle(float _radius, const mt::vec2f& _localPos,
        float _density, float _friction, float _restitution, bool _isSensor,
        unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
    {

        b2CircleShape shape;
        shape.m_radius = m_scaler->scaleToBox2D( _radius );
        shape.m_p.Set( m_scaler->scaleToBox2D( _localPos[0] ) ,m_scaler->scaleToBox2D( _localPos[1] ) );

        b2FixtureDef fd;
        fd.shape = &shape;
        fd.density = _density;
        fd.friction = _friction;
        fd.restitution = _restitution;
        fd.filter.maskBits =_collisionMask;
        fd.filter.categoryBits = _categoryBits;
        fd.filter.groupIndex = _groupIndex;
        fd.isSensor = _isSensor;

        m_body->CreateFixture( &fd );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::addShapeBox(float _width, float _height, const mt::vec2f& _localPos, float _angle,
        float _density, float _friction, float _restitution, bool _isSensor,
        unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex )
    {

        b2PolygonShape shape;
        float boxWidth = m_scaler->scaleToBox2D( _width ) * 0.5f;
        float boxHeight = m_scaler->scaleToBox2D( _height ) * 0.5f;
        shape.SetAsBox(
            boxWidth
            , boxHeight
            , b2Vec2( m_scaler->scaleToBox2D( _localPos[0] ) ,m_scaler->scaleToBox2D( _localPos[1] ) )
            , _angle );

        b2FixtureDef fd;

        fd.shape = &shape;
        fd.density = _density;
        fd.friction = _friction;
        fd.restitution = _restitution;
        fd.filter.maskBits =_collisionMask;
        fd.filter.categoryBits = _categoryBits;
        fd.filter.groupIndex = _groupIndex;
        fd.isSensor = _isSensor;

        m_body->CreateFixture( &fd );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f& Box2DPhysicBody::getPosition()
    {
        b2Vec2 position = m_body->GetPosition();
        m_position.x = position.x;
        m_position.y = position.y;
        m_scaler->scalePositionToMenge( m_position );
        return m_position;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::setPosition( float _x, float _y )
    {
        b2Vec2 position( _x , _y );
        m_scaler->scalePositionToBox2D( position );
        float angle = m_body->GetAngle();
        m_body->SetTransform( position, angle );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f& Box2DPhysicBody::getOrientation()
    {
        float angle = m_body->GetTransform().q.GetAngle();
        m_direction.x =  cosf(angle);
        m_direction.y =  sinf(angle);
        return m_direction;
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DPhysicBody::getMass() const
    {
        return m_body->GetMass();
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DPhysicBody::getInertia() const
    {
        return m_body->GetInertia();
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DPhysicBody::getAngle()
    {
        return m_body->GetAngle();
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::setOrientation( float _angle )
    {
        m_body->SetTransform( m_body->GetPosition(), _angle );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::setLinearVelocity( float _x, float _y )
    {
        b2Vec2 velocity( _x , _y  );
        m_scaler->scaleVectorToBox2D( velocity );
        velocity.x = m_scaler->convertX(velocity.x); 
        m_body->SetLinearVelocity( velocity );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f& Box2DPhysicBody::getLinearVelocity()
    {
        static b2Vec2 velocity(0.0f, 0.0f);
        velocity = m_body->GetLinearVelocity();
        m_scaler->scaleVectorToMenge(velocity);
        velocity.x = m_scaler->convertX(velocity.x); 
        m_velocity.x = velocity.x;
        m_velocity.y = velocity.y;
        return m_velocity;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::setAngularVelocity( float _w )
    {
        m_body->SetAngularVelocity( _w );
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DPhysicBody::getAngularVelocity() const
    {
        return m_body->GetAngularVelocity();
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::applyForce( float _forceX, float _forceY, float _pointX, float _pointY )
    {
        b2Vec2 force( _forceX , _forceY );
        b2Vec2 point( _pointX, _pointY );
        b2Vec2 posWorld = m_body->GetWorldCenter();
        m_scaler->scaleVectorToBox2D( force );
        force.x = m_scaler->convertX(force.x);

        m_scaler->scaleVectorToBox2D( point );
        point = posWorld + point;
        m_body->ApplyForce( force, point );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY )
    {
        b2Vec2 impulse( _impulseX, _impulseY );
        b2Vec2 posWorld = m_body->GetWorldCenter();
        b2Vec2 point( _pointX , _pointY  );
        m_scaler->scaleVectorToBox2D( impulse );
        impulse.x = m_scaler->convertX(impulse.x);
        m_scaler->scaleVectorToBox2D( point );
        point = posWorld + point;
        m_body->ApplyLinearImpulse( impulse, point );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::setBodyListener( Menge::PhysicBody2DListener* _listener )
    {
        m_listener = _listener;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::_collide( b2Body* _otherBody, b2Contact* _contact )
    {
        if( !m_listener ) return;

        Box2DPhysicBody* _otherObj = static_cast<Box2DPhysicBody*> ( _otherBody->GetUserData() );
        if( _otherObj->getUserData() == 0 )
        {
            return;
        }

        b2Vec2 contact_position = _contact->GetManifold()->localPoint;
        m_scaler->scalePositionToMenge( contact_position );
        b2Vec2 normal = _contact->GetManifold()->localNormal;
        m_listener->onCollide( _otherObj, contact_position.x, contact_position.y, normal.x, normal.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::setUserData( void* _data )
    {
        m_userData = _data;
    }
    //////////////////////////////////////////////////////////////////////////
    void* Box2DPhysicBody::getUserData() const
    {
        return m_userData;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::setLinearDumping( float _dumping )
    {
        m_body->SetLinearDamping( _dumping );
    } 
    //////////////////////////////////////////////////////////////////////////
    float Box2DPhysicBody::getLinearDumping() const
    {
        return m_body->GetLinearDamping();
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::setAngularDumping( float _dumping )
    {
        m_body->SetAngularDamping( _dumping );
    }
    //////////////////////////////////////////////////////////////////////////
    float Box2DPhysicBody::getAngularDumping() const
    {
        return m_body->GetAngularDamping();
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::setFixedRotation( bool _rotation )
    {
        m_body->SetFixedRotation( _rotation );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DPhysicBody::getFixedRotation() const
    {
        return m_body->IsFixedRotation();
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::setIsBullet( bool _isBullet )
    {
        m_body->SetBullet( _isBullet );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DPhysicBody::getIsBullet() const
    {
        return m_body->IsBullet();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DPhysicBody::isFrozen() const
    {
        return !(m_body->IsActive());
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DPhysicBody::isSleeping() const
    {
        return m_body->IsSleepingAllowed();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DPhysicBody::isStatic() const
    {
        return (m_body->GetType() == b2_staticBody);
    }
    //////////////////////////////////////////////////////////////////////////
    b2Body* Box2DPhysicBody::getBody()
    {
        return m_body;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::sleep()
    {
        m_body->SetSleepingAllowed(true);
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::wakeUp()
    {
        m_body->SetAwake(true);
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::applyTorque( float _torque )
    {
        m_body->ApplyTorque( _torque );
    }
    ////////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::update()
    {
        if( m_listener )
        {
            m_listener->onUpdate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::updateFilterData( Menge::uint16 _categoryBits, Menge::uint16 _collisionMask, signed short _groupIndex )
    {
        b2Filter filter;
        filter.categoryBits = _categoryBits;
        filter.maskBits = _collisionMask;
        filter.groupIndex = _groupIndex;

        b2Fixture* fixture = m_body->GetFixtureList();
        while( fixture != 0 )
        {
            fixture->SetFilterData( filter );
            //may be not need : m_world->Refilter( shape );
            fixture = fixture->GetNext();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DPhysicBody::dump()
    {
        b2Vec2 position = m_body->GetPosition();
        float32 angle = m_body->GetAngle();
        int type = m_body->GetType();
        /*if(position.y < -0.1f)
        {
        printf("Error body going to the center of the earth  x : %4.2f y : %4.2f  angle : %4.2f\n", position.x, position.y, angle);
        }*/
        printf("x : %4.2f y : %4.2f  angle : %4.2f type %i ", position.x, position.y, angle , type);
        const b2Transform& xf = m_body->GetTransform();
        for (b2Fixture* fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            printf (" Fixture type %i Shape type %i \n", fixture->GetType(), fixture->GetShape()->GetType() );

            b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
            int32 vertexCount = poly->m_vertexCount;
            b2Assert(vertexCount <= b2_maxPolygonVertices);
            b2Vec2 vertices[b2_maxPolygonVertices];

            for (int32 i = 0; i < vertexCount; ++i)
            {
                vertices[i] = b2Mul(xf, poly->m_vertices[i]);
                printf(" vertice %i x %4.2f y %4.2f \n",i,vertices[i].x, vertices[i].y );
            }
        }

        _updateBoundingBox();
        printf("Bounding BOX minimum :  (x :%4.2f , y: %4.2f) maximum (x: %4.2f ,y: %4.2f) \n" ,m_boundingBox.minimum.x , m_boundingBox.minimum.y , m_boundingBox.maximum.x , m_boundingBox.maximum.y);
    }
    //////////////////////////////////////////////////////////////////////////
}