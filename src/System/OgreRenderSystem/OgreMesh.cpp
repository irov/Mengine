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
const float * OgreMesh::getBoneOrientation( const char * _name )
{
	Ogre::SkeletonPtr skeleton = m_mesh->getSkeleton();
	Ogre::Bone * b = skeleton->getBone( _name );
	Ogre::Quaternion q = b->getOrientation();
	return q.ptr();
}
//////////////////////////////////////////////////////////////////////////
const float *  OgreMesh::getBonePosition( const char * _name )
{
	Ogre::SkeletonPtr skeleton = m_mesh->getSkeleton();
	Ogre::Bone * b = skeleton->getBone( _name );
	Ogre::Vector3 p = b->getPosition();
	return p.ptr();
}
//////////////////////////////////////////////////////////////////////////