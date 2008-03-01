#	pragma once
#	include "Math/quat.h"

class CameraInterface;

namespace Menge
{
	class FFCamera3D
	{
	public:

		FFCamera3D();
		~FFCamera3D();

		void update();
		void forward( float s );
		void left( float s );
		void set2Direction(int x, int y);
		bool activate();

	private:
		CameraInterface * m_interface;

		mt::vec3f m_cameraPos;
		mt::vec3f m_cameraForward;
		mt::vec3f m_cameraRight;
	};
}