#	pragma once

#	include "OgreRenderSystemInterface.h"

#	include "Ogre.h"
#	include "TheoraVideoController.h"
#	include "TheoraMovieClip.h"

class OgreRenderSpriteManager;
class OgreSceneNode;

namespace CEGUI
{
	class System;
	class OgreCEGUIRenderer;
};


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
	void render() override;

	void setContentResolution( const float * _resolution );

	void setProjectionMatrix( const float * _projection ) override;
	void setViewMatrix( const float * _view ) override;
	void setWorldMatrix( const float * _world ) override;

	RenderImageInterface* createImage( const char* _name, unsigned int _width, unsigned int _height ) override;
	RenderImageInterface* createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height ) override;
	RenderImageInterface* loadImage( const TextureDesc&	_desc ) override;
	RenderVideoStreamInterface* loadImageVideoStream( const char* _filename ) override;
	void releaseImageVideoStream( RenderVideoStreamInterface* _image ) override;

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

	void setFullscreenMode( bool _fullscreen ) override;
	void loadResource( Ogre::Resource* _resource );
	void setViewportDimensions( float _width, float _height, float _renderFactor ) override;

	///////////////////////////////////////////////////////////////////////////////////
	CameraInterface * createCamera(const char * _name) override;
	LightInterface * createLight(const char * _name) override;
	EntityInterface * createEntity(const char * _name, const char * _mesh ) override;
	SceneNodeInterface * createSceneNode( const std::string & _name ) override;
	
	void releaseCamera( CameraInterface * _camera ) override;
	void releaseEntity( EntityInterface * _entity ) override;
	void releaseLight( LightInterface * _light ) override;
	void releaseSceneNode( SceneNodeInterface * _interface ) override;

private:

	OgreSceneNode * m_rootSceneNode;

	Ogre::Vector2	m_contentResolution;
	Ogre::Root * m_root;
	Ogre::RenderWindow * m_renderWindow;
	Ogre::Viewport * m_viewport;

	Ogre::RenderSystem * m_renderSys;
	Ogre::SceneManager * m_sceneMgr;

	OgreRenderSpriteManager * m_spriteMgr;

	Ogre::TheoraVideoController* m_videoControl;

	typedef std::map< const RenderImageInterface* , Ogre::TheoraMovieClip* > TMovieMap;
	TMovieMap m_videoTexturesMap;

	CEGUI::OgreCEGUIRenderer* m_GUIRenderer;
	CEGUI::System*	m_GUISystem;
};