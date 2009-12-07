#	pragma once

#	include "Node.h"
#	include "Interface/PhysicSystem2DInterface.h"

#	include "Math/polygon.h"

class PhysicBody2DInterface;

namespace Menge
{
	class RigidBody2D;

	class BodyListenerProxy
		: public PhysicBody2DListener
	{
	public:
		BodyListenerProxy( RigidBody2D* _body )
			: m_body( _body )
		{
		}
		void onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY ) override;
		void onUpdate() override;

	private:
		RigidBody2D * m_body;
	};

	class RigidBody2D
		: public Node
	{
		FACTORABLE_DECLARE( RigidBody2D )

	public:
		RigidBody2D();
		virtual ~RigidBody2D();

		virtual void onCollide( PhysicBody2DInterface * _otherObj, float _worldX, float _worldY, float _normalX, float _normalY );

	public:
		virtual void loader( XmlElement * _xml ) override;

		void _loaderPhysics( XmlElement * _xml );
		void _addShapeBox( float _width, float _heigth, const mt::vec2f& _pos, float _angle );
		void _addShapeConvex( const mt::TVectorPoints & _points, bool _isSensor );

		void _addShapePolygon( std::vector<mt::vec2f> & _points );

	// scripted
	public:
		void onUpdate();
		void onApplyForceAndTorque();
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
		void freeze( bool _freeze );
		void setCollisionMask( int _mask );

		PhysicBody2DInterface* getInterface();

		void enableStabilization( bool _enable, float _stabilityAngle, float _stabilityForce );

	protected:
		//bool _activate() override;
		//void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;
		void _setListener() override;
		void _invalidateWorldMatrix() override;

	protected:
		void loaderShape_( XmlElement * _xml );
		void loaderShapeCircle_( XmlElement * _xml );
		void loaderShapeBox_( XmlElement * _xml );

		BodyListenerProxy * m_bodyListener;

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
		int m_groupIndex;
		
		class ConvexShape
		{
			//std::vector<mt::vec2f> points;
			std::vector<mt::vec2f> points;
			bool isSensor;
		};

		typedef std::vector<mt::polygon> TConvexShapeList;
		TConvexShapeList m_convexShapeList;


		typedef std::vector<mt::polygon> TShapeList;
		TShapeList m_shapeList;
		
		typedef std::vector< std::pair< float, mt::vec2f > > TShapeCircleList;
		TShapeCircleList m_shapeCircleList;

		typedef std::vector< std::pair< std::pair< float, float >, std::pair< mt::vec2f, float > > > TShapeBoxList;
		TShapeBoxList m_shapeBoxList;

		mt::polygon m_shape;

		void _render( Camera2D * _camera ) override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;

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
		bool m_frozen;
		void _updateFilterData();
		void compileShapes_();

		float m_stabilityAngle;
		float m_stabilityForce;
		bool m_stabilization;

	};
}
