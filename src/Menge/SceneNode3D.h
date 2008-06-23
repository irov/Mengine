#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "math/quat.h"

class SceneNodeInterface;
class EntityInterface;

namespace Menge
{
	class DiscreteEntity;
	class Light;
	class Camera3D;

	class SceneNode3D
		: public NodeCore
		, public NodeChildren<SceneNode3D>
	{
		OBJECT_DECLARE(SceneNode3D);
	public:
		SceneNode3D();
		virtual ~SceneNode3D();

	public:
		virtual const mt::quatf & getWorldOrient();
		virtual const mt::vec3f & getWorldPosition();

		virtual const mt::quatf & getLocalOrient();
		const mt::vec3f & getLocalPosition();

		void setLocalPosition( const mt::vec3f & _position );
		void setLocalOrient( const mt::quatf & _quat );

		void translate( const mt::vec3f & _delta );
		void setScale( const mt::vec3f & _scale );
		
		virtual void yaw( float _angle );
		virtual void pitch( float _angle );
		virtual void roll( float _angle );
		void setFixedYawAxis( bool _fixed );
		void setYawSpeed( float _yawSpeed, float _accelDown, float _yawTime );
		void setYawLimits( const mt::vec2f& _limits );
		float getYaw() const;
		float getPitch() const;

	public:
		void addChild(SceneNode3D * _node);
		void attachToRootNode();

	public:
		virtual void loader( XmlElement * _xml ) override;

		void loaderTransformation_( XmlElement * _xml );

		void setListener( PyObject* _listener );

	protected:
		bool _activate() override;
		//void _deactivate() override;
		void _release() override;
		void _addChildren( SceneNode3D * _node ) override;
		void _update( float _timing ) override;

	protected:
		SceneNodeInterface * m_interface;

		float m_yaw;
		float m_pitch;
		mt::vec2f m_yawLimits;
		bool m_yt;
		float m_yawSpeed;
		float m_yawTime;
		float m_yawAddSpeed;
		float m_yawTiming;
		float m_yawSpeedLimit;
		float m_yawAccelDown;

		PyObject * m_listener;
		virtual void _onSetListener();

	private:
		//Camera3D * m_camera;

		void render( bool _enableDebug );
		bool isRenderable();
	};
}
