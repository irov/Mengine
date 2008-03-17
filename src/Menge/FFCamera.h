#	pragma once
#	include "Math/vec2.h"
#	include "Math/quat.h"

#	include "Camera3D.h"

namespace Menge
{
	class Actor;

	class FFCamera3D : public Camera3D
	{
		OBJECT_DECLARE(FFCamera3D);
	public:

		FFCamera3D();
		~FFCamera3D();

	public:

		void forward( float s );
		void left( float s );

		void setActor(Actor * _actor);

		void yaw( float _yaw );
		void pitch( float _pitch );
		void zoom( float _dist );

	public:
		void loader( XmlElement * _xml );

	protected:
		bool	_activate() override;
		void	_deactivate() override;
		bool	_compile() override;
		void	_release() override;
		void	_update( float _timing ) override;

	private:

		float m_yawAngle;
		float m_pitchAngle;
		float m_scrollSpeed;
		float m_scaleBoundingR;

		mt::vec3f m_translate;
		mt::quatf m_transOrient;

		Actor * m_actor;

		void _updateCamera( bool _isOrient = true );
	};
}