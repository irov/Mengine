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
		
		void applyForce( float _forceX, float _forceY, float _pointX, float _pointY );
		void applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY );
		void applyConstantForce( float _forceX, float _forceY, float _pointX, float _pointY );
		void removeConstantForce();
		void setDirectionForce( bool _relative );
		float getMass() const;
		mt::vec2f getLinearVelocity() const;
		void setLinearVelocity( float _x, float _y, bool _countGravity );
		void unsetLinearVelocity();
		void wakeUp();

		void enableStabilization( bool _enable, float _stabilityAngle, float _stabilityForce );

		void setFilterData(  int _categoryBits ,int _mask , int _groupIndex );
		void setDensity( float _density );
		void setFriction( float _friction );
		void setRestitution( float _restitution );
		void setCollisionMask( int _mask );
		void setCategoryBits( int _categoryBits );
		void setGroupIndex( int _groupIndex );
		void setLinearDumping( float _linearDamping );
		void setAngularDumping( float _angularDamping );
		void setAllowSleep( bool _allowSleep );
		void setIsBullet( bool isBullet );
		void setFixedRotation( bool fixedRotation );
		void setIsSensor( bool isSensor );
		void setOrientation( float _angle );
		void setLocalPosition( const mt::vec2f & _position ) override;
		float getDensity();
		float getFriction();
		float getRestitution();
		std::size_t getCollisionMask();
		std::size_t getCategoryBits();
		std::size_t getGroupIndex();
		float getLinearDumping();
		float getAngularDumping();
		bool getAllowSleep();
		bool getIsBullet();
		float getFixedRotation();
		float getIsSensor();
		
		void addShapeConvex( int _pointsNum, const Polygon & _polygon );
		void addShapeCircle( float _radius, mt::vec2f _localPos );
		void addShapeBox( float _width, float _height, mt::vec2f _localPos, float _angle );
		void createBody( int type/*, float _angle, float _linearDamping, float _angularDamping, float _allowSleep, bool _isBullet, float _fixedRotation*/ );
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
		void loader( BinParser * _parser ) override;

	
		
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
		float m_density;
		float m_friction;
		float m_restitution;
		std::size_t m_collisionMask;
		std::size_t m_categoryBits;
		unsigned short m_groupIndex;
		mt::vec2f m_position;
		float m_angle;
		mt::vec2f m_limits;
		
		
		/*
		typedef std::vector<Menge::Polygon> TConvexShapeList;
		TConvexShapeList m_convexShapeList;

		typedef std::vector<Menge::Polygon> TShapeList;
		TShapeList m_shapeList;

		typedef std::vector< std::pair< float, mt::vec2f > > TShapeCircleList;
		TShapeCircleList m_shapeCircleList;

		typedef std::vector< std::pair< std::pair< float, float >, std::pair< mt::vec2f, float > > > TShapeBoxList;
		TShapeBoxList m_shapeBoxList;
		*/


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


		void addShapeData_( const Menge::TVectorPoints& _points );
		
	};
}
