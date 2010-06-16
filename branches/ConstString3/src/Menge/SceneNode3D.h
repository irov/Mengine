#	pragma once

#	include "Node.h"

#	include "Allocator3D.h"

#	include "ValueInterpolator.h"

namespace Menge
{
	class SceneNode3D
		: public Node
		, public Allocator3D
	{
	public:
		SceneNode3D();

	public:
		void yaw( float _degrees );
		void pitch( float _degrees );
		float getYaw();
		float getPitch();

		void yawTime( float _degrees, float _time );
		void pitchTime( float _degrees, float _time );

		virtual const mt::mat4f & getWorldMatrix3D() override;
		void changePivot3D() override;

		virtual void loader( XmlElement * _xml ) override;

	protected:
		virtual void _update( float _timing ) override;
		void _setListener( PyObject * _listener ) override;


		ValueInterpolatorLinear<float> m_yawTime;
		ValueInterpolatorLinear<float> m_pitchTime;

		float m_yaw;
		float m_pitch;
	};
}
