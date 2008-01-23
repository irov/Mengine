#	include "Camera3D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "RenderEngine.h"

#	include "Interface/RenderSystemInterface.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Camera3D );
	//////////////////////////////////////////////////////////////////////////
	Camera3D::Camera3D()
		: m_interface(0)
	{
		m_interface = Holder<RenderEngine>::hostage()->createCamera("default");
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
	void Camera3D::loader( XmlElement * _xml )
	{
		SceneNode3D::loader( _xml );
	}
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
