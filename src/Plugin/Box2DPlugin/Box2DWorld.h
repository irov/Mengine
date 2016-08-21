#	pragma once

#	include "Core/ModuleBase.h"

#	include "Box2D/Box2D.h"

#	include <vector>

namespace Menge
{
	class Box2DBody;
	class Box2DJoint;

	class Box2DWorld
		: public b2DestructionListener
		, public b2ContactFilter
		, public b2ContactListener
    {
    public:
		Box2DWorld();
		~Box2DWorld();

    public:
		bool initialize( const mt::vec2f& _gravity );
		void finalize();

	public:
		void update( float _time, float _timing );

	public:
		void setTimeStep( float _timeStep, uint32_t _velocityIterations, uint32_t _positionIterations );

	public:
		Box2DBody * createBody( bool _static, const mt::vec2f& _pos, float _angle, float _linearDamping, float _angularDamping,
			bool _allowSleep, bool _isBullet, bool _fixedRotation );

		void destroyBody( Box2DBody * _body );

	public:
        Box2DJoint * createDistanceJoint( Box2DBody * _body1,
			Box2DBody * _body2,
            const mt::vec2f& _offsetBody1,
            const mt::vec2f& _offsetBody2,
            bool _collideBodies );

		Box2DJoint * createHingeJoint( Box2DBody* _body1,
			Box2DBody* _body2,
            const mt::vec2f& _offsetBody1,
            const mt::vec2f& _limits,
            bool _collideBodies );

		Box2DJoint * createPrismaticJoint( Box2DBody* _body1
			, Box2DBody* _body2
            , const mt::vec2f& _unitsWorldAxis
            , bool _collideConnected 
            , bool _enableLimit
            , const mt::vec2f& _translation 
            , bool _enableMotor
            , float _maxMotorForce
            , float _motorSpeed);

		Box2DJoint * createPulleyJoint( Box2DBody* _body1
			, Box2DBody* _body2
            , const mt::vec2f& _offsetBody1
            , const mt::vec2f& _offsetBody2
            , const mt::vec2f& _offsetGroundBody1
            , const mt::vec2f& _offsetGroundBody2
            , float _ratio
            , bool _collideConnected );

		Box2DJoint * createGearJoint( Box2DBody * _body1
			, Box2DBody * _body2
			, Box2DJoint * _joint1
			, Box2DJoint * _joint2
            , float _ratio
            , bool _collideConnected );	

		Box2DJoint * createRopeJoint( Box2DBody * _body1
			, Box2DBody * _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , float _maxlength
            , bool _collideConnected );	


		Box2DJoint * createWheelJoint( Box2DBody * _body1
			, Box2DBody * _body2
            , const mt::vec2f & _localAnchor1
            , const mt::vec2f & _localAnchor2
            , const mt::vec2f & _localAxis1
            , float _frequencyHz
            , float _dampingRatio
            , bool _collideConnected 
            , float _maxMotorTorque
            , float _motorSpeed		
            );	

		void destroyJoint( Box2DJoint * _joint );

	protected:
		void SayGoodbye( b2Joint* joint ) override;
		void SayGoodbye( b2Fixture* fixture ) override;

	protected:
		bool ShouldCollide( b2Fixture* fixtureA, b2Fixture* fixtureB ) override;

	protected:
		void BeginContact( b2Contact* contact ) override;
		void EndContact( b2Contact* contact ) override;
		void PreSolve( b2Contact* contact, const b2Manifold* oldManifold ) override;
		void PostSolve( b2Contact* contact, const b2ContactImpulse* impulse ) override;

	protected:
		Box2DJoint * createJoint_( const b2JointDef * _jointDef );

	protected:
        ServiceProviderInterface * m_serviceProvider;

		float m_timing;

        b2World * m_world;

		float m_timeStep;
		uint32_t m_velocityIterations;
		uint32_t m_positionIterations;

        typedef std::vector<b2Contact *> TVectorContact;
        TVectorContact m_contacts;
        
        struct JoinDef
        {
            b2JointDef * def;
            Box2DJoint * join;
        };

        typedef std::vector<JoinDef> TVectorJoints;
        TVectorJoints m_joints;

        typedef std::vector<Box2DBody *> TVectorBodies;
		TVectorBodies m_bodies;
        TVectorBodies m_deletingBodies;
    };
}