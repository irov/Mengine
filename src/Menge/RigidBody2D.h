#	pragma once

#	include "SceneNode2D.h"
#	include "Interface/PhysicSystem2DInterface.h"

#	include "Math/polygon.h"

class PhysicBody2DInterface;

namespace Menge
{
	//class PyObject;

	class RigidBody2D
		: public SceneNode2D
		, public PhysicBody2DCollisionListener
	{
		OBJECT_DECLARE( RigidBody2D )

	public:
		RigidBody2D();
		virtual ~RigidBody2D();

		virtual void onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY ) override;

	public:
		virtual void loader( XmlElement * _xml ) override;
		void _loaderPhysics( XmlElement * _xml );

	// scripted
	public:
		void setListener( PyObject * _listener );
		PyObject* getListener();

		void applyForce( float _forceX, float _forceY, float _pointX, float _pointY );
		void applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY );
		void applyConstantForce( float _forceX, float _forceY, float _pointX, float _pointY );
		void removeConstantForce();
		void setOrientation( float _angle );
		void setPosition( float _x, float _y );
		void setDirectionForce( bool _relative );
		void wakeUp();

		PhysicBody2DInterface* getInterface();

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

		void _loaderShape( XmlElement * _xml );
		void _loaderShapeCircle( XmlElement * _xml );
		void _loaderShapeBox( XmlElement * _xml );

		float m_linearDamping;
		float m_angularDamping;
		bool m_allowSleep;
		bool m_isBullet;
		bool m_fixedRotation;
		float m_density;
		float m_friction;
		float m_restitution;
		unsigned int m_collisionMask;
		unsigned int m_categoryBits;
		unsigned int m_groupIndex;
		
		typedef std::vector<mt::polygon> TShapeList;
		TShapeList m_shapeList;
		
		typedef std::vector< std::pair< float, mt::vec2f > > TShapeCircleList;
		TShapeCircleList m_shapeCircleList;

		typedef std::vector< std::pair< std::pair< float, float >, std::pair< mt::vec2f, float > > > TShapeBoxList;
		TShapeBoxList m_shapeBoxList;

		mt::polygon m_shape;

	private:
		PhysicBody2DInterface* m_interface;
		PyObject* m_scriptListener;
		bool m_constantForce;
		bool m_directionForce;
		mt::vec2f m_force;
		mt::vec2f m_forcePoint;
	};

}	// namespace Menge