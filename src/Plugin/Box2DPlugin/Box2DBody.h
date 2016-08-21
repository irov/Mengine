#	pragma once

#	include "Kernel/Node.h"
#	include "Core/Polygon.h"

#	include "Box2D/Box2D.h"

namespace Menge
{
    class Box2DBody
        : public Node
    {
    public:
		Box2DBody();
		~Box2DBody();

	public:
		void setBox2dWorld( b2World * _world );
		b2World * getBox2dWorld() const;

	public:
		void onCollide( Box2DBody * _body, b2Contact* _contact );

    public:
		void setBody( b2Body * _body );
		b2Body * getBody() const;

    public:
		bool addShapeConvex( const Menge::Polygon & _vertices, float _density, float _friction, float _restitution, bool _isSensor,
            unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex );
        bool addShapeCircle( float _radius, const mt::vec2f& _localPos,
            float _density, float _friction, float _restitution, bool _isSensor,
            unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex );
		bool addShapeBox( float _width, float _height, const mt::vec2f& _localPos, float _angle,
            float _density, float _friction, float _restitution, bool _isSensor,
            unsigned short _collisionMask, unsigned short _categoryBits, unsigned short _groupIndex );

    public:
        float getMass() const;
        float getInertia() const;

		void setLinearVelocity( const mt::vec2f & _velocity );
        mt::vec2f getLinearVelocity() const;

        void setAngularVelocity( float _w );
        float getAngularVelocity() const;

		void applyForce( const mt::vec2f & _force, const mt::vec2f & _point );
		void applyImpulse( const mt::vec2f & _impulse, const mt::vec2f & _point );
        void applyTorque( float _torque );

    public:
        bool isFrozen() const;
		bool isSleeping() const;
        bool isStatic() const;

        void setLinearDumping( float _dumping );
        float getLinearDumping() const;
        void setAngularDumping( float _dumping );
        float getAngularDumping() const;
        void setFixedRotation( bool _rotation );
        bool getFixedRotation() const;
        void setIsBullet( bool _isBullet ); 
        bool getIsBullet() const; 

        void sleep();
        void wakeUp();

		void updateFilterData( uint16_t _categoryBits, uint16_t _collisionMask, int16_t _groupIndex );

	public:
		void updateLocalMatrix() const;
        
	protected:
        b2World* m_world;
        b2Body* m_body;		
    };
}