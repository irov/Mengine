#	include "FFCamera.h"
#	include "Interface/RenderSystemInterface.h"
#	include "RenderEngine.h"
#	include "Holder.h"

namespace Menge
{
	FFCamera3D::FFCamera3D()
	: gCameraPos(0,20,0)
	, gCameraForward(0,0,-1)
	, gCameraRight(1,0,0)
	, mx(0)
	, my(0)
	{
	}

	FFCamera3D::~FFCamera3D()
	{}

	void FFCamera3D::update()
	{
		m_interface->lookAt(gCameraPos.x+gCameraForward.x,gCameraPos.y+gCameraForward.y,gCameraPos.z+gCameraForward.z);
	}

	void FFCamera3D::forward( float s )
	{
		gCameraPos += gCameraForward * s;
		m_interface->setPosition(gCameraPos.x, gCameraPos.y, gCameraPos.z);
	}

	void FFCamera3D::left( float s )
	{
		gCameraPos += gCameraRight * s;
		m_interface->setPosition(gCameraPos.x, gCameraPos.y, gCameraPos.z);
	}

	void FFCamera3D::set2Direction(int x, int y)
	{
		int dx = mx - x;
		int dy = my - y;

		mt::norm_v3(gCameraForward);
		gCameraRight.cross(gCameraForward,mt::vec3f(0,1,0));

		mt::quatf qx(3.141592653589793f * dx * 20 / 180.0f,mt::vec3f(0,1,0));
		qx.rotate(gCameraForward);

		mt::quatf qy(3.141592653589793f* dy * 20 / 180.0f,gCameraRight);
		qy.rotate(gCameraForward);

		mx = x;
		my = y;
	}

	bool FFCamera3D::activate()
	{
		std::string m_name = "FreeCamera";
		m_interface = Holder<RenderEngine>::hostage()->createCamera(
			m_name );

		m_interface->setAspectRatio( 1.3f );
		m_interface->setNearClipDistance( 10.0f );
		m_interface->setFarClipDistance( 1000.0f );
		m_interface->setPosition( 0,20,0 );

		return true;
	}
}