#	pragma once
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
		void set2Direction(int x, int y);
		bool activate();
		void rotate( float t );

		void setActor(Actor * _actor);

	private:

		Actor * m_actor;

		float m_fRotateSpeed;
		float m_fMaxYawAngle;
		float m_fMinYawAngle;

		CameraInterface * m_interface;

		mt::vec3f m_cameraPos;
		mt::vec3f m_cameraForward;
		mt::vec3f m_cameraRight;
	};
}