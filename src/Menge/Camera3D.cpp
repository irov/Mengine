#	include "Camera3D.h"

#	include "ObjectImplement.h"

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT( Camera3D );
//////////////////////////////////////////////////////////////////////////
Camera3D::Camera3D()
{

}
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
const mt::mat4f & Camera3D::getProjectionMatrix()
{
	return m_projMatrix;
}
//////////////////////////////////////////////////////////////////////////
const mt::mat4f & Camera3D::getViewMatrix()
{
	return m_viewMatrix;
}
//////////////////////////////////////////////////////////////////////////
void Camera3D::_updateMatrix( Allocator3D * _parent )
{
	mt::inv_m4( m_viewMatrix, m_worldMatrix );
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
void Camera3D::loader( TiXmlElement * _xml )
{
	SceneNode3D::loader( _xml );
}

//// ***************************************************************
////  ObjectCamera.cpp   version:  1.0   ·  date: 03/20/2006
////  -------------------------------------------------------------
////  IROV
////  -------------------------------------------------------------
////  Copyright (C) 2006 - All Rights Reserved
//// ***************************************************************
//// 
//// ***************************************************************
//
//
//#	include "ObjectCamera.h"
//#	include "ObjectImplement.h"
//
//#	include "SceneNode.h"
//#	include "ObjectWorld.h"
//
//#	include "Math/Side.h"
//#	include "Math/mat4_function_uncommon.h"
//
//
////////////////////////////////////////////////////////////////////////////
//enum EFrustumPlane
//{
//	FRUSTUM_PLANE_NEAR   = 0,
//	FRUSTUM_PLANE_FAR    = 1,
//	FRUSTUM_PLANE_LEFT   = 2,
//	FRUSTUM_PLANE_RIGHT  = 3,
//	FRUSTUM_PLANE_TOP    = 4,
//	FRUSTUM_PLANE_BOTTOM = 5
//};
//
//
////////////////////////////////////////////////////////////////////////////
//#	define OBJECT_CLASS_DECLARE (CObjectCamera,CObjectAllocatorModify)
//
////////////////////////////////////////////////////////////////////////////
//OBJECT_SUPER_CONSTRUCTOR_IMPLEMENT()
//, m_bYawFixed(true)
//, m_YawFixedAxis(0.f,1.f,0.f)
//, m_bRecalc(true)
//, m_Fov(60.0f)
//, m_NearDist(0.01f)
//, m_FarDist(2500.0f)
//, m_Aspect(4.0f/3.0f)
//, m_ProjType(PROJECTION_PERSPECTIVE)
//, m_RecalProjection(true)
//, m_OrthoSize(1.0f)
//OBJECT_CONSTRUCTOR_BODY_IMPLEMENT()
//{
//	mt::Identity(m_ViewMatrix);
//}
////////////////////////////////////////////////////////////////////////////
//float CObjectCamera::GetSquarLengthToObject(
//	const CObjectWorld *ObjectWorld)const
//{
//	const mt::vec3f &CameraPosition = GetPosition();
//	const mt::vec3f &ObjectPosition = ObjectWorld->GetPosition();
//
//	float SquarLength = mt::SqrLength(CameraPosition,ObjectPosition);
//
//	return SquarLength;
//}
////////////////////////////////////////////////////////////////////////////
//float CObjectCamera::GetSquarLengthToPosition(
//	const mt::vec3f &Position)const
//{
//	const mt::vec3f &CameraPosition = GetPosition();
//
//	float SquarLength = mt::SqrLength(CameraPosition,Position);
//
//	return SquarLength;
//}
////////////////////////////////////////////////////////////////////////////
//size_t CObjectCamera::GetObjects(TVectorObject &PushBackVector)const
//{
//	size_t NumOfObject = m_Scene->GetObjectsFromCamera(this,PushBackVector);
//
//	return NumOfObject;
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::SetScene(CSceneNode *Scene)
//{
//	Scene->AddCamera(this);
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::_ChangePivot()
//{
//	OBJECT_BASE::_ChangePivot();
//
//	m_bRecalc = true;
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::Yaw(float degrees)
//{
//	const mt::vec3f & Axis = GetLocalStrafeAxis();
//
//	mt::RotateMatrixAxisSelf(m_LocalWorldMatrix,Axis,degrees);
//
//	ChangePivot();
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::Pitch(float degrees)
//{
//	mt::RotateMatrixAxisSelf(m_LocalWorldMatrix,m_YawFixedAxis,degrees);
//
//	ChangePivot();
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::LookAt(const mt::vec3f& targetPoint)
//{
//	const mt::vec3f &Position = GetPosition();
//	SetDirection(targetPoint - Position);
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::SetFixedYawAxis( bool useFixed, const mt::vec3f& fixedAxis )
//{
//	m_bYawFixed = useFixed;
//	m_YawFixedAxis = fixedAxis;
//}
////////////////////////////////////////////////////////////////////////////
//const mt::mat4f & CObjectCamera::GetProjectionMatrix()const
//{
//	Recalc();
//	return m_ProjMatrix;
//}
////////////////////////////////////////////////////////////////////////////
//const mt::mat4f & CObjectCamera::GetViewMatrix()const	
//{
//	Recalc();
//	return m_ViewMatrix;
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::GetDirectionFromMouse(mt::vec3f &out, float fx,float fy) const
//{
//	const mt::mat4f &ProjMatrix = GetProjectionMatrix();
//	const mt::mat4f &ViewMatrix = GetViewMatrix();
//
//	mt::vec3f v(
//		(1.f-2.f*fx)/ProjMatrix[0][0],
//		-(1.f-2.f*fy)/ProjMatrix[1][1],
//		1.f
//		);
//
//	mt::mat4f mt;
//	mt::Inverse(mt,ViewMatrix);
//	mt::Mul(out,v,mt);
//
//	out = -out;
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::GetRayFromMouse(mt::rayf &out, float xm,float ym) const
//{
//	out.origin = GetPosition();
//	GetDirectionFromMouse(out.direction,xm,ym);
//}
////////////////////////////////////////////////////////////////////////////
//bool CObjectCamera::IsVisibleBox(const mt::boxf & BoundBox)const
//{
//	Recalc();
//	mt::vec3f pCorners[8];
//	mt::GetPointFromBox(pCorners, BoundBox);
//
//	return IsVisibleCorners(pCorners);
//}
////////////////////////////////////////////////////////////////////////////
//bool CObjectCamera::IsVisibleCorners(const mt::vec3f *pCorners)const
//{
//	Recalc();
//	bool Done = true;
//	for (int plane = 0; plane < 6; ++plane)
//	{			
//		for( int point = 0; point < 8; ++point)
//		{
//			if( mt::Distance( m_FrustumPlanes[plane], pCorners[point] ) > 0.f )
//			{
//				Done = false;
//				break; 
//			}
//		}
//		if( Done )
//		{
//			return false;
//		}
//		Done = true;
//	}
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//bool CObjectCamera::IsVisiblePoints(const mt::vec3f &Point)const
//{
//	Recalc();
//	for (int plane = 0; plane < 6; ++plane)
//	{
//		if( mt::Distance( m_FrustumPlanes[plane], Point ) < 0.f )
//		{
//			return false;
//		}
//	}
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//bool CObjectCamera::IsVisibleSphere(const mt::spheref& sphere)const
//{
//	Recalc();
//	for (int plane = 0; plane < 6; ++plane)
//	{
//		if ( mt::Distance(m_FrustumPlanes[plane],sphere.pos) < -sphere.radius)
//		{
//			return false;
//		}
//	}
//
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//bool CObjectCamera::IsVisibleSphere(const mt::vec3f& Position, float Radius)const
//{
//	Recalc();
//	for (int plane = 0; plane < 6; ++plane)
//	{
//		if ( mt::Distance(m_FrustumPlanes[plane],Position) < -Radius)
//		{
//			return false;
//		}
//	}
//
//	return true;
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::RecalcProjection() const
//{
//	if( m_RecalProjection )
//	{
//		if (m_ProjType == PROJECTION_PERSPECTIVE)
//		{
//			// PERSPECTIVE transform
//			mt::MakeProjectionMatrix(
//				m_ProjMatrix,
//				m_Fov,
//				m_Aspect,
//				m_NearDist,
//				m_FarDist 
//				);
//
//			float thetaY = (m_Fov / 2.0f) * 3.1415f / 180.f;
//			float tanThetaY = tanf(thetaY);
//			float tanThetaX = tanThetaY * m_Aspect;
//
//			float vpTop = tanThetaY * m_NearDist;
//			float vpRight = tanThetaX * m_NearDist;
//			float vpBottom = -vpTop;
//			float vpLeft = -vpRight;
//
//			float fNSqr = m_NearDist * m_NearDist;
//			float fLSqr = vpRight * vpRight;
//			float fRSqr = fLSqr;
//			float fTSqr = vpTop * vpTop;
//			float fBSqr = fTSqr;
//
//			float fInvLength = 1.0f / sqrtf( fNSqr + fLSqr );
//			m_CoeffL[0] = m_NearDist * fInvLength;
//			m_CoeffL[1] = -vpLeft * fInvLength;
//
//			fInvLength = 1.0f / sqrtf( fNSqr + fRSqr );
//			m_CoeffR[0] = -m_NearDist * fInvLength;
//			m_CoeffR[1] = vpRight * fInvLength;
//
//			fInvLength = 1.0f / sqrtf( fNSqr + fBSqr );
//			m_CoeffB[0] = m_NearDist * fInvLength;
//			m_CoeffB[1] = -vpBottom * fInvLength;
//
//			fInvLength = 1.0f / sqrtf( fNSqr + fTSqr );
//			m_CoeffT[0] = -m_NearDist * fInvLength;
//			m_CoeffT[1] = vpTop * fInvLength;
//
//		}
//		else if (m_ProjType == PROJECTION_ORTHOGRAPHIC)
//		{
//			float thetaY = (m_Fov / 2.0f) * 3.1415f / 180.f;
//			float sinThetaY = sinf(thetaY);
//			float thetaX = thetaY * m_Aspect;
//			float sinThetaX = sinf(thetaX);
//			float w = 1.0f / (sinThetaX * m_NearDist);
//			float h = 1.0f / (sinThetaY * m_NearDist);
//			float q = 1.0f / (m_FarDist - m_NearDist);
//
//			// ORTHOGRAPHIC transform
//			mt::Identity( m_ProjMatrix );
//			m_ProjMatrix[0][0] = w;
//			m_ProjMatrix[1][1] = h;
//			m_ProjMatrix[2][2] =-q;
//			m_ProjMatrix[3][3] = 1;
//			/*
//			MakeOrthoMatrix(
//			m_ProjMatrix,
//			vpLeft,
//			vpRight,
//			vpTop,
//			vpBottom,
//			m_NearDist,
//			m_FarDist
//			);
//			*/
//			float vpTop    = sinThetaY * m_NearDist;
//			float vpRight  = sinThetaX * m_NearDist;
//			float vpBottom = -vpTop;
//			float vpLeft   = -vpRight;
//
//			float fNSqr = m_NearDist * m_NearDist;
//			float fLSqr = vpRight * vpRight;
//			float fRSqr = fLSqr;
//			float fTSqr = vpTop * vpTop;
//			float fBSqr = fTSqr;
//
//			float fInvLength = 1.0f / sqrtf( fNSqr + fLSqr );
//			m_CoeffL[0] = m_NearDist * fInvLength;
//			m_CoeffL[1] = -vpLeft * fInvLength;
//
//			fInvLength = 1.0f / sqrtf( fNSqr + fRSqr );
//			m_CoeffR[0] = -m_NearDist * fInvLength;
//			m_CoeffR[1] = vpRight * fInvLength;
//
//			fInvLength = 1.0f / sqrtf( fNSqr + fBSqr );
//			m_CoeffB[0] = m_NearDist * fInvLength;
//			m_CoeffB[1] = -vpBottom * fInvLength;
//
//			fInvLength = 1.0f / sqrtf( fNSqr + fTSqr );
//			m_CoeffT[0] = -m_NearDist * fInvLength;
//			m_CoeffT[1] = vpTop * fInvLength;
//
//		}
//		m_RecalProjection = false;
//	}
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::RecalcFrustum() const
//{
//	RecalcProjection();
//
//	const mt::mat4f &wm = 
//		GetWorldMatrix();
//
//	//в матрице View  дирекш обратный. по этому я немного модифицировал алгоритм,
//	//что бы увеличить скорость.
//	mt::vec3f left	(wm[0][0],wm[1][0],wm[2][0]);
//	mt::vec3f up	(wm[0][1],wm[1][1],wm[2][1]);
//	mt::vec3f dir	(wm[0][2],wm[1][2],wm[2][2]);
//
//	const mt::vec3f &pos = GetPosition();
//
//	float fDdE = - mt::Dot( dir , pos );
//
//	m_FrustumPlanes[FRUSTUM_PLANE_LEFT].norm = m_CoeffL[0]*left - m_CoeffL[1]*dir;
//	m_FrustumPlanes[FRUSTUM_PLANE_LEFT].d =	- mt::Dot( pos , m_FrustumPlanes[FRUSTUM_PLANE_LEFT].norm );
//
//	// right plane
//	m_FrustumPlanes[FRUSTUM_PLANE_RIGHT].norm = m_CoeffR[0]*left - m_CoeffR[1]*dir;
//	m_FrustumPlanes[FRUSTUM_PLANE_RIGHT].d = - mt::Dot( pos, m_FrustumPlanes[FRUSTUM_PLANE_RIGHT].norm );
//
//	// bottom plane
//	m_FrustumPlanes[FRUSTUM_PLANE_BOTTOM].norm = m_CoeffB[0]*up - m_CoeffB[1]*dir;
//	m_FrustumPlanes[FRUSTUM_PLANE_BOTTOM].d = - mt::Dot( pos, m_FrustumPlanes[FRUSTUM_PLANE_BOTTOM].norm );
//
//	// top plane
//	m_FrustumPlanes[FRUSTUM_PLANE_TOP].norm = m_CoeffT[0]*up - m_CoeffT[1]*dir;
//	m_FrustumPlanes[FRUSTUM_PLANE_TOP].d = - mt::Dot( pos, m_FrustumPlanes[FRUSTUM_PLANE_TOP].norm );
//
//	// far plane
//	m_FrustumPlanes[FRUSTUM_PLANE_FAR].norm = dir;
//	m_FrustumPlanes[FRUSTUM_PLANE_FAR].d = fDdE + m_FarDist;
//
//	// near plane
//	m_FrustumPlanes[FRUSTUM_PLANE_NEAR].norm = -dir;
//	m_FrustumPlanes[FRUSTUM_PLANE_NEAR].d = -(fDdE + m_NearDist);
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::SetProjectionType(EProjectionType pt)
//{
//	if( m_ProjType != pt )
//	{
//		m_ProjType = pt;
//		m_RecalProjection = true;
//	}
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::SetFOVy(float fovy)
//{
//	if( m_Fov != fovy )
//	{
//		m_Fov = fovy;
//		m_RecalProjection = true;
//	}
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::SetNearClipDistance(float nearDist)
//{
//	if( m_NearDist != nearDist )
//	{
//		if (nearDist <= 0)
//		{
//			return;
//		}
//
//		m_NearDist = nearDist;
//		m_RecalProjection = true;
//	}
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::SetFarClipDistance(float farDist)
//{
//	if( m_FarDist != farDist )
//	{
//		m_FarDist = farDist;
//		m_RecalProjection = true;
//	}
//}
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::SetAspectRatio(float ratio)
//{
//	if( m_Aspect != ratio )
//	{
//		m_Aspect = ratio;
//		m_RecalProjection = true;
//	}
//}
//
//#	include <d3dx9math.h>
//
//#	pragma comment (lib,"d3dx9d.lib")
//
////////////////////////////////////////////////////////////////////////////
//void CObjectCamera::Recalc() const
//{
//	if( m_bRecalc == true || m_RecalProjection == true)
//	{
//		const mt::mat4f &wm = GetWorldMatrix();
//
//		mt::Inverse(m_ViewMatrix,wm);
//
//		RecalcFrustum();
//
//		m_bRecalc = false;
//	}
//}