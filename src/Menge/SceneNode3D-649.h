#	pragma once

#	include "NodeCore.h"
#	include "NodeChildren.h"

#	include "Allocator3D-649.h"
#	include "Renderable3D.h"

#	include "ValueInterpolator.h"

namespace Menge
{
	class SceneNode3D_
		: public NodeCore
		, public NodeChildren<SceneNode3D_>
		, public Allocator3D_
		, public Renderable3D
	{
		OBJECT_DECLARE( SceneNode3D_ );
	public:

		SceneNode3D_();

		void yaw( float _degrees );
		void pitch( float _degrees );
		float getYaw();
		float getPitch();

		void yawTime( float _degrees, float _time );
		void pitchTime( float _degrees, float _time );

		virtual const mt::mat4f & getWorldMatrix() override;
		void changePivot() override;

		virtual void loader( XmlElement * _xml ) override;

		virtual void setListener( PyObject* _listener ) override;

	protected:
		virtual void _update( float _timing ) override;


		ValueInterpolator<float> m_yawTime;
		ValueInterpolator<float> m_pitchTime;

		float m_yaw;
		float m_pitch;
	};
}