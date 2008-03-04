#	include "Camera3D.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Player.h"

namespace	Menge
{
	Camera3D::Camera3D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Camera3D::Camera3D( const std::string & _name )
	: m_interface(0)
	, m_near(10.0f)
	, m_far(1000.0f)
	, m_position(0.f, 0.f, 0.f)
	, m_at(0.f, 0.f, 0.f)
	, m_name(_name)
	, m_parent(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Camera3D::~Camera3D()
	{
		Holder<RenderEngine>::hostage()->releaseCamera( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & Camera3D::getName()
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::lookAt(const mt::vec3f& _targetPoint)
	{
		m_interface->lookAt(_targetPoint.x, _targetPoint.y, _targetPoint.z);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setPosition(const mt::vec3f& _pos)
	{
		m_interface->setPosition(_pos.x, _pos.y, _pos.z);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setNear( float _dist )
	{
		m_interface->setNearClipDistance(_dist);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setFar( float _dist )
	{
		m_interface->setFarClipDistance(_dist);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setAspect( float _aspect )
	{
		m_interface->setAspectRatio(_aspect);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::yaw( float _angle )
	{
		m_interface->yaw(_angle);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::pitch( float _angle )
	{
		m_interface->pitch(_angle);
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::roll( float _angle )
	{
		m_interface->roll(_angle);
	}
	//////////////////////////////////////////////////////////////////////////
	SceneNode3D * Camera3D::getParentNode() const
	{
		return m_parent;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::setParentNode( SceneNode3D * _node ) 
	{
		m_parent = _node;
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
	bool Camera3D::activate()
	{
		m_name = "MainCamera";
		m_interface = Holder<RenderEngine>::hostage()->createCamera(
			m_name );

		this->setAspect( m_aspect );
		this->setNear( m_near );
		this->setFar( m_far );
		this->setPosition( m_position );
		this->lookAt( m_at );

//		this->getParentNode()->attachCamera(this);

		return true;
	};
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::deactivate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::release()
	{
	}
}
