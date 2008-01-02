#	include "Camera3D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Camera3D );
	//////////////////////////////////////////////////////////////////////////
	Camera3D::Camera3D()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::yaw( float _degrees )
	{
		const mt::vec3f & axis = getLocalStrafe();

		mt::rotate_axis_m4( m_localMatrix, axis, _degrees);
		
		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::pitch( float _degrees )
	{
		mt::rotate_axis_m4( m_localMatrix, m_fixedUp, _degrees);

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::lookAt(const mt::vec3f& _targetPoint)
	{
		setDirection( _targetPoint - m_localMatrix.v3_3 );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera3D::getViewMatrix()
	{	
		_updateMatrix( m_parent );

		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_updateMatrix( Allocator3D * _parent )
	{
		const mt::mat4f & wm = getWorldMatrix();

		mt::inv_m4( m_viewMatrix, wm );

		recalc( m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f Camera3D::getDirectionFromMouse( float _xm, float _ym )
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
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::loader( XmlElement * _xml )
	{
		SceneNode3D::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
}
