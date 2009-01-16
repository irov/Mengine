#	include "Frustum.h"

#	include <ios>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Frustum::Frustum()
	: m_fov(3.1415f/4.0f)
	, m_near(0.01f)
	, m_far(2500.0f)
	, m_aspect(4.0f/3.0f)
	, m_focalLength( 1.0f )
	, m_frustumOffset( 0.0f, 0.0f )
	, m_recalcProjection(true)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Frustum::setFov( float _fov )
	{
		m_fov = _fov;

		m_recalcProjection = true;
	}
	//////////////////////////////////////////////////////////////////////////
	float Frustum::getFov() const
	{
		return m_fov;
	}
	//////////////////////////////////////////////////////////////////////////
	void Frustum::setNear( float _near )
	{
		m_near = _near;

		m_recalcProjection = true;
	}
	//////////////////////////////////////////////////////////////////////////
	float Frustum::getNear() const
	{
		return m_near;
	}
	//////////////////////////////////////////////////////////////////////////
	void Frustum::setFar( float _far )
	{
		m_far = _far;

		m_recalcProjection = true;
	}
	//////////////////////////////////////////////////////////////////////////
	float Frustum::getFar() const
	{
		return m_far;
	}
	//////////////////////////////////////////////////////////////////////////
	void Frustum::setAspect( float _aspect )
	{
		m_aspect = _aspect;

		m_recalcProjection = true;
	}
	//////////////////////////////////////////////////////////////////////////
	float Frustum::getAspect() const
	{
		return m_aspect;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::mat4f & Frustum::getProjectionMatrix()
	{
		recalcProjection();

		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	void Frustum::recalcProjection()
	{
		float thetaY = m_fov * 0.5f;
		float tanThetaY = ::tanf( thetaY );
		float tanThetaX = tanThetaY * m_aspect;

		float nearFocal = m_near / m_focalLength;
		float nearOffsetX = m_frustumOffset.x * nearFocal;
		float nearOffsetY = m_frustumOffset.y * nearFocal;
		float half_w = tanThetaX * m_near;
		float half_h = tanThetaY * m_near;

		float left   = - half_w + nearOffsetX;
		float right  = + half_w + nearOffsetX;
		float bottom = - half_h + nearOffsetY;
		float top    = + half_h + nearOffsetY;

		// The code below will dealing with general projection 
		// parameters, similar glFrustum and glOrtho.
		// Doesn't optimise manually except division operator, so the 
		// code more self-explaining.

		float inv_w = 1 / (right - left);
		float inv_h = 1 / (top - bottom);
		float inv_d = 1 / (m_far - m_near);

		// Recalc if frustum params changed

		// Calc matrix elements
		float A = 2 * m_near * inv_w;
		float B = 2 * m_near * inv_h;
		float C = (right + left) * inv_w;
		float D = (top + bottom) * inv_h;
		float q, qn;
		/*if( m_far == 0 )
		{
			// Infinite far plane
			q = Frustum::INFINITE_FAR_PLANE_ADJUST - 1;
			qn = mNearDist * (Frustum::INFINITE_FAR_PLANE_ADJUST - 2);
		}
		else*/
		{
			q = - (m_far + m_near) * inv_d;
			qn = -2 * (m_far * m_near) * inv_d;
		}

		// NB: This creates 'uniform' perspective projection matrix,
		// which depth range [-1,1], left-handed rules
		//
		// [ A   0   0   0  ]
		// [ 0   B   0   0  ]
		// [ C   D   q   -1 ]
		// [ 0   0   qn  0  ]
		//
		// A = 2 * near / (right - left)
		// B = 2 * near / (top - bottom)
		// C = (right + left) / (right - left)
		// D = (top + bottom) / (top - bottom)
		// q = - (far + near) / (far - near)
		// qn = - 2 * (far * near) / (far - near)
		mt::ident_v4(m_projectionMatrix.v0);
		mt::ident_v4(m_projectionMatrix.v1);
		mt::ident_v4(m_projectionMatrix.v2);
		mt::ident_v4(m_projectionMatrix.v3);
		m_projectionMatrix[0][0] = A;
		m_projectionMatrix[2][0] = C;
		m_projectionMatrix[1][1] = B;
		m_projectionMatrix[2][1] = D;
		m_projectionMatrix[2][2] = q;
		m_projectionMatrix[3][2] = qn;
		m_projectionMatrix[2][3] = -1;

		// Convert depth range from [-1,+1] to [0,1]
		m_projectionMatrix[0][2] = (m_projectionMatrix[0][2] + m_projectionMatrix[0][3]) * 0.5f;
		m_projectionMatrix[1][2] = (m_projectionMatrix[1][2] + m_projectionMatrix[1][3]) * 0.5f;
		m_projectionMatrix[2][2] = (m_projectionMatrix[2][2] + m_projectionMatrix[2][3]) * 0.5f;
		m_projectionMatrix[3][2] = (m_projectionMatrix[3][2] + m_projectionMatrix[3][3]) * 0.5f;

		m_projectionMatrix[2][0] = -m_projectionMatrix[2][0];
		m_projectionMatrix[2][1] = -m_projectionMatrix[2][1];
		m_projectionMatrix[2][2] = -m_projectionMatrix[2][2];
		m_projectionMatrix[2][3] = -m_projectionMatrix[2][3];
	}

	void Frustum::recalc( const mt::mat4f & wm )
	{
		recalcProjection();
	
		//в матрице View  дирекш обратный. по этому € немного модифицировал алгоритм,
		//что бы увеличить скорость.
		//mt::vec3f left	(wm[0][0],wm[1][0],wm[2][0]);
		//mt::vec3f up	(wm[0][1],wm[1][1],wm[2][1]);
		//mt::vec3f dir	(wm[0][2],wm[1][2],wm[2][2]);

		const mt::vec3f & left = wm.v0.to_vec3f();
		const mt::vec3f & up = wm.v1.to_vec3f();
		const mt::vec3f & dir =	wm.v2.to_vec3f();
		const mt::vec3f & pos =	wm.v3.to_vec3f();
	
		float fDdE = - mt::dot_v3_v3( dir , pos );
	
		enum EFrustumPlane
		{
			FRUSTUM_PLANE_NEAR   = 0,
			FRUSTUM_PLANE_FAR    = 1,
			FRUSTUM_PLANE_LEFT   = 2,
			FRUSTUM_PLANE_RIGHT  = 3,
			FRUSTUM_PLANE_TOP    = 4,
			FRUSTUM_PLANE_BOTTOM = 5
		};

		//m_planes[FRUSTUM_PLANE_LEFT].norm = m_coeffL[0] * left - m_coeffL[1] * dir;
		//m_planes[FRUSTUM_PLANE_LEFT].d =	- mt::dot_v3_v3( pos , m_planes[FRUSTUM_PLANE_LEFT].norm );
		mt::vec3f norm = m_coeffL[0] * left - m_coeffL[1] * dir;
		float d = - mt::dot_v3_v3( pos , norm );
		m_planes[FRUSTUM_PLANE_LEFT] = mt::planef( norm, d );
	
		// right plane
		//m_planes[FRUSTUM_PLANE_RIGHT].norm = m_coeffR[0]*left - m_coeffR[1]*dir;
		//m_planes[FRUSTUM_PLANE_RIGHT].d = - mt::dot_v3_v3( pos, m_planes[FRUSTUM_PLANE_RIGHT].norm );
		norm = m_coeffR[0]*left - m_coeffR[1]*dir;
		d = - mt::dot_v3_v3( pos, norm );
		m_planes[FRUSTUM_PLANE_RIGHT] = mt::planef( norm, d );
	
		// bottom plane
		//m_planes[FRUSTUM_PLANE_BOTTOM].norm = m_coeffB[0]*up - m_coeffB[1]*dir;
		//m_planes[FRUSTUM_PLANE_BOTTOM].d = - mt::dot_v3_v3( pos, m_planes[FRUSTUM_PLANE_BOTTOM].norm );
		norm = m_coeffB[0]*up - m_coeffB[1]*dir;
		d = - mt::dot_v3_v3( pos, norm );
		m_planes[FRUSTUM_PLANE_BOTTOM] = mt::planef( norm, d );
	
		// top plane
		//m_planes[FRUSTUM_PLANE_TOP].norm = m_coeffT[0]*up - m_coeffT[1]*dir;
		//m_planes[FRUSTUM_PLANE_TOP].d = - mt::dot_v3_v3( pos, m_planes[FRUSTUM_PLANE_TOP].norm );
		norm = m_coeffT[0]*up - m_coeffT[1]*dir;
		d = - mt::dot_v3_v3( pos, norm );
		m_planes[FRUSTUM_PLANE_TOP] = mt::planef( norm, d );
	
		// far plane
		//m_planes[FRUSTUM_PLANE_FAR].norm = dir;
		//m_planes[FRUSTUM_PLANE_FAR].d = fDdE + m_far;
		m_planes[FRUSTUM_PLANE_FAR] = mt::planef( dir, fDdE + m_far );
	
		// near plane
		//m_planes[FRUSTUM_PLANE_NEAR].norm = -dir;
		//m_planes[FRUSTUM_PLANE_NEAR].d = -(fDdE + m_near);
		m_planes[FRUSTUM_PLANE_NEAR] = mt::planef( -dir,  -(fDdE + m_near) );
	}
}
