#	include "CameraTarget2D.h"
#	include "Camera2D.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CameraTarget2D::CameraTarget2D()
		: m_camera(nullptr)
		, m_speed(0.f)
		, m_fixedHorizont(false)
		, m_horizont(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CameraTarget2D::~CameraTarget2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CameraTarget2D::setCamera2D( RenderCameraInterface * _camera )
	{
		m_camera = dynamic_cast<Camera2D *>(_camera);
	}
	//////////////////////////////////////////////////////////////////////////
	RenderCameraInterface * CameraTarget2D::getCamera2D() const
	{
		return m_camera;
	}
	//////////////////////////////////////////////////////////////////////////
	void CameraTarget2D::setSpeed( float _speed )
	{
		m_speed = _speed;
	}
	//////////////////////////////////////////////////////////////////////////
	float CameraTarget2D::getSpeed() const
	{
		return m_speed;
	}
	//////////////////////////////////////////////////////////////////////////
	void CameraTarget2D::_update( float _current, float _timing )
	{
		(void)_current;

		if( m_camera == nullptr )
		{
			return;
		}

		const mt::mat4f & camera_wm = m_camera->getWorldMatrix();
		const Viewport & camera_vp = m_camera->getCameraRenderport();
		
		Viewport camera_vpwm;
		mt::mul_v2_m4( camera_vpwm.begin, camera_vp.begin, camera_wm );
		mt::mul_v2_m4( camera_vpwm.end, camera_vp.end, camera_wm );
		
		mt::vec2f camera_vpwm_center;
		camera_vpwm.getCenter( camera_vpwm_center );

		const mt::vec3f & target_wp = this->getWorldPosition();
		
		mt::vec2f target2d_wp;
		target2d_wp.x = target_wp.x;

		if( m_fixedHorizont == false )
		{
			target2d_wp.y = target_wp.y;
		}
		else
		{
			target2d_wp.y = m_horizont;
		}

		float length = mt::length_v2_v2( camera_vpwm_center, target2d_wp );

		if( length < 0.1f )
		{
			return;
		}

		mt::vec2f dir;
		mt::norm_v2( dir, target2d_wp - camera_vpwm_center );

		float way_length = m_speed * _timing;

		float real_way = (way_length > length) ? length : way_length;

		mt::vec2f way = dir * real_way;

		const mt::vec3f & camera_lp = m_camera->getLocalPosition();

		mt::vec3f new_lp;
		new_lp.x = camera_lp.x + way.x;
		new_lp.y = camera_lp.y + way.y;
		new_lp.z = camera_lp.z;

		m_camera->setLocalPosition( new_lp );
	}
	//////////////////////////////////////////////////////////////////////////
	void CameraTarget2D::setFixedHorizont( float _horizont )
	{
		m_fixedHorizont = true;
		m_horizont = _horizont;
	}
	//////////////////////////////////////////////////////////////////////////
	float CameraTarget2D::getFixedHorizont() const
	{
		return m_horizont;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CameraTarget2D::isFixedHorizont() const
	{
		return m_fixedHorizont;
	}

}
