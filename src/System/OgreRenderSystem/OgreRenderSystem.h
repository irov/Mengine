#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"

class OgreRenderSpriteManager;

class OgreRenderSystem
	: public OgreRenderSystemInterface
	, public Ogre::ManualResourceLoader
{
public:
	OgreRenderSystem();
	~OgreRenderSystem();

public:
	bool init( Ogre::Root * _root, Ogre::RenderWindow * _renderWindow ) override;
	void render( RenderImageInterface* _outImage, const int* rect = NULL ) override;

	void setContentResolution( const float * _resolution );

	void setProjectionMatrix( const float * _projection ) override;
	void setViewMatrix( const float * _view ) override;
	void setWorldMatrix( const float * _world ) override;

	RenderImageInterface* createImage( const char* _name, unsigned int _width, unsigned int _height ) override;
	RenderImageInterface* createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height ) override;
	RenderImageInterface* loadImage( const TextureDesc&	_desc ) override;
	void releaseImage( RenderImageInterface* _image ) override;
	void renderImage(		
		const float * _transform, 
		const float * _offset,
		const float * _uv,
		const float * _size,
		unsigned int _color, 
		const RenderImageInterface* _image, 
		EBlendFactor _src,
		EBlendFactor _dst) override;

	void renderImage(		
		const float * _transform, 
		const float * _a,
		const float * _b,
		const float * _c,
		const float * _d,
		const float * _uv,
		unsigned int _color, 
		const RenderImageInterface * _image,
		EBlendFactor _src,
		EBlendFactor _dst) override;

	void	beginLayer() override;
	void	endLayer() override;

	void setFullscreenMode( bool _fullscreen );
	void loadResource( Ogre::Resource* _resource );

private:
	Ogre::Vector2	m_contentResolution;
	Ogre::Root * m_root;
	Ogre::RenderWindow * m_renderWindow;
	Ogre::Viewport * m_viewport;

	Ogre::RenderSystem * m_renderSys;
	Ogre::SceneManager * m_sceneMgr;

	OgreRenderSpriteManager * m_spriteMgr;
};