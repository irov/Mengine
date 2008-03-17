#	include "Camera3D.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Player.h"

#	include "ObjectImplement.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Camera3D);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	Camera3D::Camera3D()
	: m_camera( NULL )
	, m_near(10.0f)
	, m_far(1000.0f)
	, m_position(0.f, 0.f, 0.f)
	, m_at(0.f, 0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Camera3D::Camera3D( const std::string & _name )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Camera3D::~Camera3D()
	{
		Holder<RenderEngine>::hostage()->releaseCamera( m_camera );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::lookAt(const mt::vec3f& _targetPoint)
	{
		m_camera->lookAt(_targetPoint.x, _targetPoint.y, _targetPoint.z);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setPosition(const mt::vec3f& _pos)
	{
		m_camera->setPosition(_pos.x, _pos.y, _pos.z);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setNear( float _dist )
	{
		m_camera->setNearClipDistance(_dist);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setFar( float _dist )
	{
		m_camera->setFarClipDistance(_dist);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setAspect( float _aspect )
	{
		m_camera->setAspectRatio(_aspect);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::yaw( float _angle )
	{
		m_camera->yaw(_angle);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::pitch( float _angle )
	{
		m_camera->pitch(_angle);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::roll( float _angle )
	{
		m_camera->roll(_angle);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Aspect", "Value", m_aspect );
			XML_CASE_ATTRIBUTE_NODE( "Near", "Value", m_near);
			XML_CASE_ATTRIBUTE_NODE( "Far", "Value", m_far);
			XML_CASE_ATTRIBUTE_NODE( "Position", "Value", m_position);
			XML_CASE_ATTRIBUTE_NODE( "At", "Value", m_at );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera3D::_activate()
	{

		m_name = "MainCamera";
		m_camera = Holder<RenderEngine>::hostage()->createCamera(
			m_name );

		this->setAspect( m_aspect );
		this->setNear( m_near );
		this->setFar( m_far );
		this->setPosition( m_position );
		this->lookAt( m_at );

		//this->getParentNode()->attachCamera(this);

		return true;
	};
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_deactivate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_update( float _timing )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera3D::_compile()
	{
		return true;
	}
}

