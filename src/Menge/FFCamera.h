#	pragma once
#	include "Math/vec2.h"
#	include "Math/quat.h"

class CameraInterface;

namespace Menge
{
	class Actor;

	class FFCamera3D
	{
	public:

		FFCamera3D();
		~FFCamera3D();

		void update(float _timing);
		void forward( float s );
		void left( float s );
		bool activate();
		void setActor(Actor * _actor);

		void yaw( float _yaw );
		void pitch( float _pitch );
		void zoom( float _dist );

	private:

		float m_yawAngle;
		float m_pitchAngle;
		float m_pitchMinAngle;
		float m_pitchMaxAngle;

		mt::vec3f m_translate;
		mt::quatf m_transOrient;

		void _updateCamera( bool _isOrient = true );

		Actor * m_actor;

		CameraInterface * m_interface;
	};
}