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

	// scripted
	public:
		void setListener( PyObject * _listener );
		PyObject* getListener();

		void applyForce( float _forceX, float _forceY, float _pointX, float _pointY );
		void applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY );
		void setOrientation( float _angle );

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

		void _loaderShape( XmlElement * _xml );
		float m_density;
		float m_friction;
		float m_restitution;
		mt::polygon m_shape;

	private:
		PhysicBody2DInterface* m_interface;
		PyObject* m_scriptListener;

	};

}	// namespace Menge