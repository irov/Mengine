#	include "Camera3D-649.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Camera3D_ );
	//////////////////////////////////////////////////////////////////////////
	Camera3D_::Camera3D_()
	{
		setPosition( mt::vec3f( 0.0f, 5.0f, -3.0f ) );
		lookAt( mt::vec3f( 0, 0, 0 ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D_::yaw( float _degrees )
	{
		const mt::vec3f & axis = getLocalStrafe();

		mt::rotate_axis_m4( m_localMatrix, axis, _degrees);
		
		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D_::pitch( float _degrees )
	{
		mt::rotate_axis_m4( m_localMatrix, m_fixedUp, _degrees);

		changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D_::lookAt(const mt::vec3f& _targetPoint)
	{
		setDirection( _targetPoint - m_localMatrix.v3_3 );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera3D_::getViewMatrix()
	{	
		_updateMatrix( m_parent );

		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D_::_updateMatrix( Allocator3D_ * _parent )
	{
		const mt::mat4f & wm = getWorldMatrix();

		mt::inv_m4( m_viewMatrix, wm );

		recalc( m_worldMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f Camera3D_::getDirectionFromMouse( float _xm, float _ym )
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
	void Camera3D_::loader( XmlElement * _xml )
	{
		SceneNode3D_::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera3D_::_renderBegin()
	{		
		Holder<RenderEngine>::hostage()->setViewMatrix( getViewMatrix() );
		Holder<RenderEngine>::hostage()->setProjectionMatrix( getProjectionMatrix() );
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
}
