#	pragma once

#	include "Node.h"
#	include "Interface/PhysicSystem2DInterface.h"
#	include "Utils/Core/Polygon.h"

namespace Menge
{
	class PhysicalBody2D
		: public Node
		, public PhysicBody2DListener
		
	{
	public:
		PhysicalBody2D();
		~PhysicalBody2D();
		
	public:
		PhysicBody2DInterface* getInterface();	

	//scripted
	public:		
		
		void applyForce( const mt::vec2f& _force, const mt::vec2f& _position );
		void applyImpulse( const mt::vec2f& _impulse, const mt::vec2f& _position );
		void applyConstantForce( const mt::vec2f& _force, const mt::vec2f& _position );
		void removeConstantForce();
		void setDirectionForce( bool _relative );
		float getMass() const;
		mt::vec2f getLinearVelocity() const;
		void setLinearVelocity( float _x, float _y, bool _countGravity );
		void unsetLinearVelocity();
		
		void wakeUp();
		void enableStabilization( bool _enable, float _stabilityAngle, float _stabilityForce );
		void setFilterData(  int _categoryBits ,int _mask , int _groupIndex );
		void setCollisionMask( int _mask );
		void setCategoryBits( int _categoryBits );
		void setGroupIndex( int _groupIndex );
		void setLinearDumping( float _linearDamping );
		void setAngularDumping( float _angularDamping );
		void setAllowSleep( bool _allowSleep );
		void setIsBullet( bool isBullet );
		void setFixedRotation( bool fixedRotation );
		void setOrientation( float _angle );
		
		/*
		void setLocalPosition( const mt::vec2f & _position ) override;
		float getDensity();
		float getFriction();
		float getRestitution();
		void setDensity( float _density );
		void setFriction( float _friction );
		void setRestitution( float _restitution );
		void setIsSensor( bool isSensor );
		*/

		size_t getCollisionMask();
		size_t getCategoryBits();
		size_t getGroupIndex();
		float getLinearDumping();
		float getAngularDumping();
		bool  getAllowSleep();
		bool  getIsBullet();
		float getFixedRotation();
		float getIsSensor();

		void addShapeConvex( const Polygon & _polygon ,float _density ,float _friction, float _restitution, bool _isSensor );
		void addShapeCircle( float _radius, const mt::vec2f& _localPos ,float _density ,float _friction, float _restitution, bool _isSensor );
		void addShapeBox( float _width, float _height, const mt::vec2f& _localPos, float _angle ,float _density ,float _friction, float _restitution, bool _isSensor );
		void createBody( int _type , float _angle, float _linearDamping, float _angularDamping, bool _allowSleep, bool _isBullet, bool _fixedRotation );
		//end scripted
	
	public:
		void _render( Camera2D * _camera );
	
	protected:
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;
		void _setEventListener( PyObject * _listener ) override;
		void _freeze( bool _value ) override;
	
	protected:
		void onCollide( PhysicBody2DInterface * _otherObj, float _worldX, float _worldY, float _normalX, float _normalY ) override;
		void onUpdate() override;
		
	protected:
#	ifndef MENGE_MASTER_RELEASE
		void _debugRender( Camera2D * _camera, unsigned int _debugMask ) override;
#	endif
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
			
	protected:
		float m_linearDamping;
		float m_angularDamping;
		bool m_allowSleep;
		bool m_isBullet;
		bool m_fixedRotation;
		bool m_isSensor;
		/*
		float m_density;
		float m_friction;
		float m_restitution;
		*/
		size_t m_collisionMask;
		size_t m_categoryBits;
		unsigned short m_groupIndex;
		float m_angle;
	
	protected:
		PhysicBody2DInterface* m_interface;
		
		bool m_constantForce;
		bool m_directionForce;
		bool m_linearVelocity;
		bool m_unsetLinearVelocity;
		mt::vec2f m_force;
		mt::vec2f m_forcePoint;
		mt::vec2f m_velocity;
		bool m_countGravity;
		float m_stabilityAngle;
		float m_stabilityForce;
		bool m_stabilization;
	//private:*/
		void onApplyForceAndTorque_();
		void updateFilterData_();
		void compileShapes_();
		

		typedef std::vector<TVectorVertex2D> TVectorShapeData;
		TVectorShapeData m_shapeData;

		struct ConvexShape
		{
			Menge::Polygon vertices;
			TVectorVertex2D worldVertices;
		};

		typedef std::vector<ConvexShape> TVectorConvexShapes;
		TVectorConvexShapes m_convexShapes;
	};
}
