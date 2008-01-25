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
	void getBoneOrientation( const char * _name, float & w, float & x, float & y, float & z ) const;
	void getBonePos( const char * _name, float & x, float & y, float & z ) const;

private:
	Ogre::MeshPtr m_mesh;
};