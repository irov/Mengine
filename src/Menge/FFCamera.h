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
		int mx;
		int my;

		CameraInterface * m_interface;

		mt::vec3f gCameraPos;
		mt::vec3f gCameraForward;
		mt::vec3f gCameraRight;
	};
}