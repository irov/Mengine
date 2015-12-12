#	include "BurritoCameraTarget.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BurritoCameraTarget::BurritoCameraTarget()
		: m_camera(nullptr)
		, m_speed(0.f)
		, m_maxScale(2.0f)
		, m_horizont(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	BurritoCameraTarget::~BurritoCameraTarget()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoCameraTarget::setCamera2D( Camera2D * _camera )
	{
		m_camera = _camera;
	}
	//////////////////////////////////////////////////////////////////////////
	Camera2D * BurritoCameraTarget::getCamera2D() const
	{
		return m_camera;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoCameraTarget::setSpeed( float _speed )
	{
		m_speed = _speed;
	}
	//////////////////////////////////////////////////////////////////////////
	float BurritoCameraTarget::getSpeed() const
	{
		return m_speed;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoCameraTarget::setMaxScale( float _scale )
	{
		m_maxScale = _scale;
	}
	//////////////////////////////////////////////////////////////////////////
	float BurritoCameraTarget::getMaxScale() const
	{
		return m_maxScale;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoCameraTarget::setRenderport( const Viewport & _renderport )
	{
		m_renderport = _renderport;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & BurritoCameraTarget::getRenderport() const
	{
		return m_renderport;
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoCameraTarget::_invalidateWorldMatrix()
	{
		if( m_camera == nullptr )
		{
			return;
		}

		const mt::vec3f & target_wp = this->getWorldPosition();

		const mt::vec3f & camera_lp = m_camera->getLocalPosition();		
		const Viewport & camera_renderport = m_camera->getCameraRenderport();

		Viewport camera_renderport_wm;
		camera_renderport_wm.begin = camera_renderport.begin + target_wp.to_vec2f();
		camera_renderport_wm.end = camera_renderport.end + target_wp.to_vec2f();

		mt::vec2f camera_renderport_wm_center;
		camera_renderport_wm.getCenter( camera_renderport_wm_center );

		float camera_height = camera_renderport.getHeight();
		float camera_height_half = camera_height * 0.5f;

		float renderport_height = m_renderport.getHeight();

		float pr = (renderport_height - (target_wp.y - 300.f) * 0.35f) / renderport_height;

		mt::vec3f new_camera_lp;
		Viewport new_camera_renderport;

		if( pr < 1.f )
		{
			new_camera_lp.x = 0.f;
			new_camera_lp.y = 0.f;
			new_camera_lp.z = 0.f;

			new_camera_renderport = m_renderport;
		}
		else if( pr > m_maxScale )
		{
			new_camera_lp.x = 0.f;
			new_camera_lp.y = target_wp.y - 300.f;
			new_camera_lp.z = 0.f;

			new_camera_renderport = m_renderport;
			new_camera_renderport.scale( mt::vec2f( m_maxScale, m_maxScale ) );
		}
		else
		{
			new_camera_lp.x = 0.f;
			new_camera_lp.y = target_wp.y - 300.f;
			new_camera_lp.z = 0.f;

			new_camera_renderport = m_renderport;
			new_camera_renderport.scale( mt::vec2f( pr, pr ) );
		}

		//printf( "%f %f\n"
		//	, new_camera_lp.y
		//	, pr
		//	);

		//Viewport camera_renderport_wm2;
		//camera_renderport_wm2.begin = new_camera_renderport.begin + new_camera_lp.to_vec2f();
		//camera_renderport_wm2.end = new_camera_renderport.end + new_camera_lp.to_vec2f();

		//mt::vec2f camera_renderport_wm_center2;
		//camera_renderport_wm2.getCenter( camera_renderport_wm_center2 );

		m_camera->setLocalPosition( new_camera_lp );
		m_camera->setRenderport( new_camera_renderport );
	}
	//////////////////////////////////////////////////////////////////////////
	void BurritoCameraTarget::setHorizont( float _horizont )
	{
		m_horizont = _horizont;
	}
	//////////////////////////////////////////////////////////////////////////
	float BurritoCameraTarget::getHorizont() const
	{
		return m_horizont;
	}
}
