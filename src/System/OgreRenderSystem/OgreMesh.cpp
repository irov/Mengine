#	include "OgreMesh.h"
//////////////////////////////////////////////////////////////////////////
OgreMesh::OgreMesh(const Ogre::MeshPtr& _mesh)
	: m_mesh(_mesh)
{
}
//////////////////////////////////////////////////////////////////////////
bool	OgreMesh::hasSkeleton() const
{
	return m_mesh->hasSkeleton();
}
//////////////////////////////////////////////////////////////////////////
float	OgreMesh::getBoundingSphereRadius() const
{
	return m_mesh->getBoundingSphereRadius();
}
//////////////////////////////////////////////////////////////////////////
const char * OgreMesh::getName() const
{
	return m_mesh->getName().c_str();
}
//////////////////////////////////////////////////////////////////////////
void OgreMesh::getBoneOrientation( const char * _name, float & w, float & x, float & y, float & z ) const
{
	if( m_mesh->hasSkeleton() == true )
	{
		Ogre::SkeletonPtr skeleton = m_mesh->getSkeleton();
		Ogre::Bone * b = skeleton->getBone( _name );
		Ogre::Quaternion q = b->getOrientation();

		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;
	}
	else
	{
		assert(0);
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreMesh::getBonePos( const char * _name, float & x, float & y, float & z ) const
{
	if( m_mesh->hasSkeleton() == true )
	{
		Ogre::SkeletonPtr skeleton = m_mesh->getSkeleton();
		Ogre::Bone * b = skeleton->getBone( _name );
		Ogre::Vector3 p = b->getPosition();

		x = p.x;
		y = p.y;
		z = p.z;
	}
	else
	{
		assert(0);
	}
}
//////////////////////////////////////////////////////////////////////////