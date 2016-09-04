#	pragma once

#	include "Factory/FactorablePtr.h"

#	include "Kernel/Scriptable.h"
#	include "Kernel/Servant.h"

#	include "Core/ModuleBase.h"

#   include "Factory/FactoryStore.h"

#	include "Box2D/Box2D.h"

#	include <vector>

namespace Menge
{
	typedef stdex::intrusive_ptr<class Box2DBody> Box2DBodyPtr;
	typedef stdex::intrusive_ptr<class Box2DJoint> Box2DJointPtr;

	class Box2DWorld
		: public FactorablePtr
		, public Scriptable
		, public Servant
		, public b2DestructionListener
		, public b2ContactFilter
		, public b2ContactListener		
    {
    public:
		Box2DWorld();
		~Box2DWorld();

	public:
		void setDead();
		bool isDead() const;

    public:
		bool initialize( const mt::vec2f& _gravity );
		void finalize();

	public:
		void update( float _time, float _timing );

	public:
		void setTimeStep( float _timeStep, uint32_t _velocityIterations, uint32_t _positionIterations );

	public:
		Box2DBodyPtr createBody( bool _static, const mt::vec2f& _pos, float _angle, float _linearDamping, float _angularDamping,
			bool _allowSleep, bool _isBullet, bool _fixedRotation );

	public:
		Box2DJointPtr createDistanceJoint( const Box2DBodyPtr & _body1,
			const Box2DBodyPtr & _body2,
            const mt::vec2f& _offsetBody1,
            const mt::vec2f& _offsetBody2,
            bool _collideBodies );

		Box2DJointPtr createHingeJoint( const Box2DBodyPtr & _body1,
			const Box2DBodyPtr & _body2,
            const mt::vec2f& _offsetBody1,
            const mt::vec2f& _limits,
            bool _collideBodies );

		Box2DJointPtr createPrismaticJoint( const Box2DBodyPtr & _body1
			, const Box2DBodyPtr & _body2
            , const mt::vec2f& _unitsWorldAxis
            , bool _collideConnected 
            , bool _enableLimit
            , const mt::vec2f& _translation 
            , bool _enableMotor
            , float _maxMotorForce
            , float _motorSpeed);

		Box2DJointPtr createPulleyJoint( const Box2DBodyPtr & _body1
			, const Box2DBodyPtr & _body2
            , const mt::vec2f& _offsetBody1
            , const mt::vec2f& _offsetBody2
            , const mt::vec2f& _offsetGroundBody1
            , const mt::vec2f& _offsetGroundBody2
            , float _ratio
            , bool _collideConnected );

		Box2DJointPtr createGearJoint( const Box2DBodyPtr & _body1
			, const Box2DBodyPtr & _body2
			, const Box2DJointPtr & _joint1
			, const Box2DJointPtr & _joint2
            , float _ratio
            , bool _collideConnected );	

		Box2DJointPtr createRopeJoint( const Box2DBodyPtr & _body1
			, const Box2DBodyPtr & _body2
            , const mt::vec2f & _offsetBody1
            , const mt::vec2f & _offsetBody2
            , float _maxlength
            , bool _collideConnected );	


		Box2DJointPtr createWheelJoint( const Box2DBodyPtr & _body1
			, const Box2DBodyPtr & _body2
            , const mt::vec2f & _localAnchor1
            , const mt::vec2f & _localAnchor2
            , const mt::vec2f & _localAxis1
            , float _frequencyHz
            , float _dampingRatio
            , bool _collideConnected 
            , float _maxMotorTorque
            , float _motorSpeed		
            );	

	protected:
		void SayGoodbye( b2Joint * joint ) override;
		void SayGoodbye( b2Fixture * fixture ) override;

	protected:
		bool ShouldCollide( b2Fixture * fixtureA, b2Fixture * fixtureB ) override;

	protected:
		void BeginContact( b2Contact * contact ) override;
		void EndContact( b2Contact * contact ) override;
		void PreSolve( b2Contact * contact, const b2Manifold * oldManifold ) override;
		void PostSolve( b2Contact * contact, const b2ContactImpulse * impulse ) override;

	protected:
		Box2DJointPtr createJoint_( const b2JointDef * _jointDef );

	protected:
		bool m_dead;

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
    };
}