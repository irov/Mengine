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
int Ogre3dCamera::isAABBIntersect( EntityInterface * _entity )
{
	OgreEntity * ogreEntity = static_cast<OgreEntity*>(_entity);
	Ogre::Entity * entity = ogreEntity->getOgreEntity();

	const Ogre::AxisAlignedBox & aabb = entity->getWorldBoundingBox();

	if(aabb.isNull()) 
		return -1;

	const Ogre::Vector3 * corner = aabb.getAllCorners();

	int iTotalIn = 0;

	std::vector<int> planes;

	for(int p = 0; p < 6; ++p)
	{
		int iInCount = 8;
		int iPtIn = 1;	

		const Ogre::Plane & plane = m_camera->getFrustumPlane(p);
		
		for(int i = 0; i < 8; ++i)
		{
			Ogre::Vector3 c = corner[i];
			c.x *= 1.2f;
	
			if(plane.getSide(c) == Ogre::Plane::NEGATIVE_SIDE)
			{
				iPtIn = 0;
				--iInCount;
			}
		}

		if(iPtIn == 0)
		planes.push_back(p);


		if(iInCount == 0)
		{
			return -1;
		}

		iTotalIn += iPtIn;
	}

	if(iTotalIn == 6)
	{
		return -2;
	}

	for(int i = 0; i < planes.size(); i++)
	{
		printf("%d ",planes[i]);
	}
	printf("\n");

	printf("false\n");
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