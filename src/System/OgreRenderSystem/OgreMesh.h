#	pragma once

#	include "OgreRenderSystemInterface.h"
// в разработке
class	OgreMesh
	: public MeshInterface
{
public:
	OgreMesh(const Ogre::MeshPtr& _mesh);
	bool hasSkeleton() const;
	float getBoundingSphereRadius() const;
	const char * getName() const;
	const float * getBoneOrientation( const char * _name );
	const float * getBonePosition( const char * _name );

private:
	Ogre::MeshPtr m_mesh;
};