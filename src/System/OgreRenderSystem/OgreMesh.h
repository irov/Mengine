#	pragma once

#	include "OgreRenderSystemInterface.h"
// в разработке

class	OgreMesh
	: public MeshInterface
{
public:
	OgreMesh(const Ogre::MeshPtr& _mesh);

public:
	bool hasSkeleton() const override;
	float getBoundingSphereRadius() const override;
	const char * getName() const override;
	const float * getBoneOrientation( const char * _name ) override;
	const float * getBonePosition( const char * _name ) override;

private:
	Ogre::MeshPtr m_mesh;
};