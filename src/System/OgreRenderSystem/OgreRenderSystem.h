#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class OgreRenderSpriteManager;

class OgreRenderSystem
	: public OgreRenderSystemInterface
{
public:
	OgreRenderSystem();
	~OgreRenderSystem();

public:
	bool init( Ogre::Root * _root, Ogre::RenderWindow * _renderWindow ) override;
	void render();

	void setProjectionMatrix(const /*mat4f*/ float * _projection) override;
	void setViewMatrix(const /*mat4f*/ float * _view) override;
	void setWorldMatrix(const /*mat4f*/ float * _world) override;

	RenderImageInterface* loadImage( const TextureDesc&	_desc ) override;
	void releaseImage( RenderImageInterface* _rii ) override;
	void renderImage(		
		const /*mat3*/ float * _transform, 
		const /*vec2f*/ float * _offset,
		const /*vec4f*/ float * _uv,
		const /*vec2f*/ float * _size,
		unsigned int _mixedColor, 
		RenderImageInterface* _rii) override;

private:
	Ogre::Root * m_root;
	Ogre::RenderWindow * m_renderWindow;

	Ogre::RenderSystem * m_renderSys;
	Ogre::SceneManager * m_sceneMgr;

	OgreRenderSpriteManager * m_spriteMgr;
};
