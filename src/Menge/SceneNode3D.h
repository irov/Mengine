#	pragma once

#	include "Node.h"
#	include "NodeChildren.h"

#	include "Allocator3D.h"
#	include "Renderable3D.h"

#	include "ValueInterpolator.h"

namespace Menge
{
	class SceneNode3D
		: public virtual Node
		, public NodeChildren<SceneNode3D>
		, public Allocator3D
		, public Renderable3D
	{
		OBJECT_DECLARE( SceneNode3D );
	public:

		SceneNode3D();

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
		void _setListener() override;


		ValueInterpolator<float> m_yawTime;
		ValueInterpolator<float> m_pitchTime;

		float m_yaw;
		float m_pitch;
	};
}