#	include "OgreCamera.h"
#	include "OgreEntity.h"
//////////////////////////////////////////////////////////////////////////
Ogre3dCamera::Ogre3dCamera(Ogre::Camera * _camera)
: m_camera(_camera)
{
}
//////////////////////////////////////////////////////////////////////////
Ogre3dCamera::~Ogre3dCamera()
{
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::setPosition( float _x, float _y, float _z )
{
	m_camera->setPosition( _x, _y, _z );
}
//////////////////////////////////////////////////////////////////////////
const float * Ogre3dCamera::getPosition() const
{
	return m_camera->getPosition().ptr();
}
//////////////////////////////////////////////////////////////////////////
const float * Ogre3dCamera::getOrient() const
{
	return m_camera->getOrientation().ptr();
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::rotate(float * _dir, float _angle)
{
	m_camera->rotate(Ogre::Vector3(_dir[0],_dir[1],_dir[2]),Ogre::Radian(_angle));
}
//////////////////////////////////////////////////////////////////////////
const float * Ogre3dCamera::getDirection() const
{
	return m_camera->getDirection().ptr();
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::setOrient(float * _q)
{
	m_camera->setOrientation(*(Ogre::Quaternion*)_q);
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::setDirection( float _x, float _y, float _z )
{
	m_camera->setDirection( _x, _y, _z );
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::lookAt( float _x, float _y, float _z )
{
	m_camera->lookAt( _x, _y, _z );
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::setNearClipDistance( float _dist )
{
	m_camera->setNearClipDistance(_dist);
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::setFarClipDistance( float _dist )
{
	m_camera->setFarClipDistance(_dist);
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::setAspectRatio( float _aspect )
{
	m_camera->setAspectRatio(_aspect);
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::yaw( float _angle )
{
	m_camera->yaw( Ogre::Degree( _angle ) );
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::pitch( float _angle )
{
	m_camera->pitch( Ogre::Degree( _angle ) );
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::roll( float _angle )
{
	m_camera->roll( Ogre::Degree( _angle ) );
}
//////////////////////////////////////////////////////////////////////////
Ogre::Camera * Ogre3dCamera::getOgreCamera()
{
	return m_camera;
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::getAABB( float * _min, float * _max ) const
{
	const Ogre::AxisAlignedBox & aabb = m_camera->getBoundingBox();
	_min[0] = aabb.getMinimum().x;
	_min[1] = aabb.getMinimum().y;
	_min[2] = aabb.getMinimum().z;

	_max[0] = aabb.getMaximum().x;
	_max[1] = aabb.getMaximum().y;
	_max[2] = aabb.getMaximum().z;
}
//////////////////////////////////////////////////////////////////////////
int Ogre3dCamera::isSphereIntersect( float x, float y, float z, float R, float * q )
{
//	Ogre::Vector3 ttt = Ogre::Plane(Ogre::Vector3(0,1,0),0).projectVector(Ogre::Vector3(1,0,0));

//	R *= 2;
	Ogre::Vector3 NNN;
	Ogre::Quaternion qq = (*(Ogre::Quaternion*)q);

	Ogre::Radian rrr;
	qq.ToAngleAxis(rrr,NNN);

	//printf("%f \n",rrr.valueDegrees());
	NNN = Ogre::Vector3(sinf(rrr.valueRadians()),0,cosf(rrr.valueRadians()));

	Ogre::Vector3 spherePos(x,y,z);


	// NEAR, FAR skipped
	for(int p = 2; p < 6; ++p)
	{
		const Ogre::Plane & plane = m_camera->getFrustumPlane(p);

		float dd = plane.getDistance(spherePos);
		if(dd >= R)
		{
			continue;
		}

		float l = plane.normal.length();

		float k = -plane.normal.dotProduct(spherePos);

		float depth = plane.d - k + R;

		if (depth >= 0) 
		{
			qq.normalise();
			Ogre::Vector3 Normal = NNN;//plane.normal;
			Ogre::Vector3 Pos = spherePos - Normal * R;
			Ogre::Vector3 CPos = spherePos/*Pos*/ - (m_camera->getPosition());

			CPos.y = 0;

//			printf("LENGTH = %f\n",CPos.length());

			CPos.normalise();

			Normal = CPos;

		//	printf("Sphere = %f %f %f\n",spherePos.x,spherePos.y,spherePos.z);
		//	printf("Cam = %f %f %f\n",m_camera->getPosition().x,m_camera->getPosition().y,m_camera->getPosition().z);
			//m_camera->setPosition(m_camera->getPosition() + Normal * depth);

			Ogre::Vector3 pos = m_camera->getPosition();
			pos+=Normal * 0.06;
			m_camera->setPosition(pos);
			return 1;
		}
		else 
		{
			return 0;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
int Ogre3dCamera::isAABBIntersect( float * _min, float * _max, float & _dx, float & _dz )
{
	Ogre::Vector3 max = *(Ogre::Vector3*)_max;
	Ogre::Vector3 min = *(Ogre::Vector3*)_min;

	Ogre::Vector3 dir = max - min;
	Ogre::Vector3 uvh[3];

	uvh[0] = Ogre::Vector3(dir.x, 0, 0);
	uvh[1] = Ogre::Vector3(0, dir.y, 0);
	uvh[2] = Ogre::Vector3(0, 0, dir.z);

	Ogre::Vector3 point[8];

	point[0] = min;
	point[1] = min + uvh[0];
	point[4] = min + uvh[2];
	point[5] = min + uvh[0] + uvh[2];

	point[2] = min + uvh[0] + uvh[1];
	point[3] = min + uvh[1];
	point[6] = min + uvh[0] + uvh[1] + uvh[2];
	point[7] = min + uvh[1] + uvh[2];

	int totalIn = 0;

	std::vector<int> planes;

	Ogre::Vector3 direction;


	m_camera->getBoundingRadius();

	Ogre::AxisAlignedBox box = m_camera->getBoundingBox();
	printf("%f \n",box.getMinimum().y);

	// NEAR, FAR skipped
	for(int p = 2; p < 6; ++p)
	{
		int inCount = 8;
		int pointIn = 1;	

		const Ogre::Plane & plane = m_camera->getFrustumPlane(p);
		
		for(int i = 0; i < 8; ++i)
		{
			Ogre::Vector3 & c = point[i];
	
			if(plane.getSide(c) == Ogre::Plane::NEGATIVE_SIDE)
			{
				pointIn = 0;
				--inCount;
				direction = plane.projectVector(c);
			}
		}

		if(pointIn == 0)
		{
			planes.push_back(p);
		}

		if(inCount == 0)
		{
			return -1;
		}

		totalIn += pointIn;
	}
	// вообще ни с одной.

	if(totalIn == 4)
	{
		return -2;
	}

	for(int i = 0; i < planes.size(); i++)
	{
		printf("%d ",planes[i]);
	}
	printf("\n");

	printf("false\n");

	//direction-=m_camera->getPosition();
	//direction.normalise();

	_dx = direction.x;
	_dz = direction.z;

	return *planes.begin();
}
//////////////////////////////////////////////////////////////////////////
void Ogre3dCamera::translate( float * _v )
{
	Ogre::Vector3 pos = m_camera->getPosition();
	pos.x+=_v[0];
	pos.y+=_v[1];
	pos.z+=_v[2];
	m_camera->setPosition(pos);
}