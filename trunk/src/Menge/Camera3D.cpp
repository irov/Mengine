#	include "Camera3D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(Camera3D);
	//////////////////////////////////////////////////////////////////////////
	Camera3D::Camera3D()
		: m_at( 0.0f, 0.0f, 0.0f )
		, m_renderArea( 0.0f, 0.0f, 0.0f, 0.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::lookAt(const mt::vec3f& _targetPoint)
	{
		setDirection3D( _targetPoint - m_localMatrix3D.v3.to_vec3f() );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Camera3D::getViewMatrix()
	{	
		SceneNode3D * sceneNode = dynamic_cast<SceneNode3D*>(m_parent);
		if( sceneNode == 0 )
		{
			mt::mat4f mat;
			mt::ident_m4( mat );
			updateMatrix3D( mat );

			return m_viewMatrix;
		}

		const mt::mat4f & wm = sceneNode->getWorldMatrix3D();

		updateMatrix3D( wm );

		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_updateMatrix3D()
	{
		mt::inv_m4( m_viewMatrix, m_worldMatrix3D );

		m_viewMatrix[0][0] = -m_viewMatrix[0][0];
		m_viewMatrix[1][0] = -m_viewMatrix[1][0];
		m_viewMatrix[2][0] = -m_viewMatrix[2][0];
		m_viewMatrix[3][0] = -m_viewMatrix[3][0];

		recalc( m_worldMatrix3D );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec3f Camera3D::getDirectionFromMouse( float _xm, float _ym )
	{
		const mt::mat4f &projMatrix = getProjectionMatrix();
		const mt::mat4f &viewMatrix = getViewMatrix();
		
		mt::vec3f v(
			( 1.f - 2.f * _xm ) / projMatrix.v0.x,
			-( 1.f - 2.f * _ym ) / projMatrix.v1.y,
			1.f
			);
		
		mt::vec3f out;

		mt::mul_v3_m4( out, v, m_worldMatrix3D );

		return -out;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::loader( XmlElement * _xml )
	{
		SceneNode3D::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Aspect", "Value", m_aspect );
			XML_CASE_ATTRIBUTE_NODE( "Near", "Value", m_near);
			XML_CASE_ATTRIBUTE_NODE( "Far", "Value", m_far);
			XML_CASE_ATTRIBUTE_NODE( "At", "Value", m_at );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_render( Camera2D * _camera )
	{		
		Holder<RenderEngine>::hostage()
			->setActiveCamera( this );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera3D::_activate()
	{
		lookAt( m_at );

		getViewMatrix();
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera3D::_compile()
	{
		if( SceneNode3D::_compile() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Camera3D::_release()
	{
		SceneNode3D::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f& Camera3D::getProjectionMatrix()
	{
		return getProjectionMtx();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f& Camera3D::getRenderArea()
	{
		return m_renderArea;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Camera3D::is3D() const 
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}
