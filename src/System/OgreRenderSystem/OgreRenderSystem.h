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

	void setProjectionMatrix( const float * _projection ) override;
	void setViewMatrix( const float * _view ) override;
	void setWorldMatrix( const float * _world ) override;

	RenderImageInterface* loadImage( const TextureDesc&	_desc ) override;
	void releaseImage( RenderImageInterface* _image ) override;
	void renderImage(		
		const float * _transform, 
		const float * _offset,
		const float * _uv,
		const float * _size,
		unsigned int _color, 
		const RenderImageInterface* _image ) override;

	void	beginLayer() override;
	void	endLayer() override;

private:
	Ogre::Root * m_root;
	Ogre::RenderWindow * m_renderWindow;
	Ogre::Viewport * m_viewport;

	Ogre::RenderSystem * m_renderSys;
	Ogre::SceneManager * m_sceneMgr;

	OgreRenderSpriteManager * m_spriteMgr;
};