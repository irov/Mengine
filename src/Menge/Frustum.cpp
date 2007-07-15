#	include "Frustum.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Frustum::Frustum()
	: m_fov(60.0f)
	, m_near(0.01f)
	, m_far(2500.0f)
	, m_aspect(4.0f/3.0f)
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
		if( m_recalcProjection )
		{
			// PERSPECTIVE transform
			mt::make_projection_m4(
				m_projectionMatrix,
				m_fov,
				m_aspect,
				m_near,
				m_far 
				);

			float thetaY = (m_fov / 2.0f) * 3.1415f / 180.f;
			float tanThetaY = tanf(thetaY);
			float tanThetaX = tanThetaY * m_aspect;

			float vpTop = tanThetaY * m_near;
			float vpRight = tanThetaX * m_near;
			float vpBottom = -vpTop;
			float vpLeft = -vpRight;

			float fNSqr = m_near * m_near;
			float fLSqr = vpRight * vpRight;
			float fRSqr = fLSqr;
			float fTSqr = vpTop * vpTop;
			float fBSqr = fTSqr;

			float fInvLength = 1.0f / sqrtf( fNSqr + fLSqr );
			m_coeffL[0] = m_near * fInvLength;
			m_coeffL[1] = -vpLeft * fInvLength;

			fInvLength = 1.0f / sqrtf( fNSqr + fRSqr );
			m_coeffR[0] = -m_near * fInvLength;
			m_coeffR[1] = vpRight * fInvLength;

			fInvLength = 1.0f / sqrtf( fNSqr + fBSqr );
			m_coeffB[0] = m_near * fInvLength;
			m_coeffB[1] = -vpBottom * fInvLength;

			fInvLength = 1.0f / sqrtf( fNSqr + fTSqr );
			m_coeffT[0] = -m_near * fInvLength;
			m_coeffT[1] = vpTop * fInvLength;

			m_recalcProjection = false;
		}		
	}

	void Frustum::recalc( const mt::mat4f & wm )
	{
		recalcProjection();
	
		//в матрице View  дирекш обратный. по этому € немного модифицировал алгоритм,
		//что бы увеличить скорость.
		//mt::vec3f left	(wm[0][0],wm[1][0],wm[2][0]);
		//mt::vec3f up	(wm[0][1],wm[1][1],wm[2][1]);
		//mt::vec3f dir	(wm[0][2],wm[1][2],wm[2][2]);

		const mt::vec3f & left =	wm.v3_0;
		const mt::vec3f & up =		wm.v3_1;
		const mt::vec3f & dir =		wm.v3_2;
		const mt::vec3f & pos =		wm.v3_3;
	
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

		m_planes[FRUSTUM_PLANE_LEFT].norm = m_coeffL[0] * left - m_coeffL[1] * dir;
		m_planes[FRUSTUM_PLANE_LEFT].d =	- mt::dot_v3_v3( pos , m_planes[FRUSTUM_PLANE_LEFT].norm );
	
		// right plane
		m_planes[FRUSTUM_PLANE_RIGHT].norm = m_coeffR[0]*left - m_coeffR[1]*dir;
		m_planes[FRUSTUM_PLANE_RIGHT].d = - mt::dot_v3_v3( pos, m_planes[FRUSTUM_PLANE_RIGHT].norm );
	
		// bottom plane
		m_planes[FRUSTUM_PLANE_BOTTOM].norm = m_coeffB[0]*up - m_coeffB[1]*dir;
		m_planes[FRUSTUM_PLANE_BOTTOM].d = - mt::dot_v3_v3( pos, m_planes[FRUSTUM_PLANE_BOTTOM].norm );
	
		// top plane
		m_planes[FRUSTUM_PLANE_TOP].norm = m_coeffT[0]*up - m_coeffT[1]*dir;
		m_planes[FRUSTUM_PLANE_TOP].d = - mt::dot_v3_v3( pos, m_planes[FRUSTUM_PLANE_TOP].norm );
	
		// far plane
		m_planes[FRUSTUM_PLANE_FAR].norm = dir;
		m_planes[FRUSTUM_PLANE_FAR].d = fDdE + m_far;
	
		// near plane
		m_planes[FRUSTUM_PLANE_NEAR].norm = -dir;
		m_planes[FRUSTUM_PLANE_NEAR].d = -(fDdE + m_near);
	}
}