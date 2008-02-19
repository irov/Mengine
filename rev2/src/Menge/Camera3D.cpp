#	include "Camera3D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Player.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Camera3D );
	//////////////////////////////////////////////////////////////////////////
	Camera3D::Camera3D()
		: m_interface(0)
		, m_near(10.0f)
		, m_far(1000.0f)
		, m_position(0.f, 0.f, 0.f)
		, m_at(0.f, 0.f, 0.f)
		, m_main(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Camera3D::~Camera3D()
	{
		Holder<RenderEngine>::hostage()->releaseCamera( m_interface );
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
	void Camera3D::loader( XmlElement * _xml )
	{
		//SceneNode3D::loader( _xml );

		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Main", "Value", m_main );
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
		m_interface = Holder<RenderEngine>::hostage()->createCamera( m_name );

		this->setAspect( m_aspect );
		this->setNear( m_near );
		this->setFar( m_far );
		this->setPosition( m_position );
		this->lookAt( m_at );

		if( m_main == true )
		{
			Holder<Player>::hostage()->setRenderCamera3D( this );
		}

		return true;
	};
	//////////////////////////////////////////////////////////////////////////
	/*mt::vec3f Camera3D::getDirectionFromMouse( float _xm, float _ym )
	{
		const mt::mat4f &projMatrix = getProjectionMatrix();
		const mt::mat4f &viewMatrix = getViewMatrix();
		
		mt::vec3f v(
			( 1.f - 2.f * _xm ) / projMatrix[0][0],
			-( 1.f - 2.f * _ym ) / projMatrix[1][1],
			1.f
			);
		
		mt::vec3f out;

		mt::mul_v3_m4( out, v, m_worldMatrix );

		return -out;
	}*/
}
