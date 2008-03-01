#	include "FFCamera.h"
#	include "Interface/RenderSystemInterface.h"
#	include "RenderEngine.h"
#	include "Holder.h"

namespace Menge
{
	FFCamera3D::FFCamera3D()
	: m_cameraPos(0,20,0)
	, m_cameraForward(0,0,-1)
	, m_cameraRight(1,0,0)
	{
	}

	FFCamera3D::~FFCamera3D()
	{}

	void FFCamera3D::update()
	{
		m_interface->lookAt(
			m_cameraPos.x + m_cameraForward.x,
			m_cameraPos.y + m_cameraForward.y, 
			m_cameraPos.z + m_cameraForward.z);
	}

	void FFCamera3D::forward( float s )
	{
		m_cameraPos += m_cameraForward * s;
		m_interface->setPosition(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
	}

	void FFCamera3D::left( float s )
	{
		m_cameraPos += m_cameraRight * s;
		m_interface->setPosition(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
	}

	void FFCamera3D::set2Direction(int x, int y)
	{
		static int mx = 0;
		static int my = 0;

		int dx = mx - x;
		int dy = my - y;

		mt::norm_v3(m_cameraForward);
		m_cameraRight.cross(m_cameraForward,mt::vec3f(0,1,0));

		mt::quatf qx(3.1415f * dx * 20.0f / 180.0f,mt::vec3f(0,1,0));
		qx.rotate(m_cameraForward);

		mt::quatf qy(3.1415f * dy * 20.0f / 180.0f,m_cameraRight);
		qy.rotate(m_cameraForward);

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
		m_interface->setPosition( 0, 20, 0 );

		return true;
	}
}