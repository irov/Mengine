#	pragma once
#	include "Interface/RenderSystemInterface.h"
#	include "libs/Math/vec2.h"
#	include "libs/Math/box2.h"

#	include "HGE.h"

#	include <map>

class HGETexture;
class HGERenderFont;

class HGERenderSystem 
	: public Menge::RenderSystemInterface
	, public Gfx_Listener
{
public:
	HGERenderSystem();
	~HGERenderSystem();

	bool initialize( Menge::LogSystemInterface* _logSystem ) override;
	bool createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, Menge::WindowHandle _winHandle,
		int _FSAAType, int _FSAAQuality ) override;
	const std::vector<int> & getResolutionList() override;
	void screenshot( Menge::RenderImageInterface* _image, const float * rect = 0 ) override;
	void render() override; 
	void setContentResolution( const float * _resolution ) override;
	void setProjectionMatrix( const float * _projection ) override;
	void setViewMatrix( const float * _view ) override;
	void setWorldMatrix( const float * _world ) override;
	Menge::RenderImageInterface * createImage( const Menge::String & _name, float _width, float _height ) override;
	Menge::RenderImageInterface * createRenderTargetImage( const Menge::String & _name, float _width, float _height ) override;
	Menge::RenderImageInterface * loadImage( const Menge::TextureDesc& _desc ) override;
	void releaseImage( Menge::RenderImageInterface * _image ) override;
	Menge::RenderImageInterface* getImage( const Menge::String& _desc ) const override;

	void renderImage(		
		const float * _renderVertex,
		const float * _uv,
		unsigned int _color, 
		const Menge::RenderImageInterface * _image,
		Menge::EBlendFactor _src,
		Menge::EBlendFactor _dst) override;

	void renderTriple(const float * _transform,  
								  const float * _a, 
								  const float * _b, 
								  const float * _c, 
								  const float * _uv0, 
								  const float * _uv1,
								  const float * _uv2, 
								  unsigned int _color,  
								  const Menge::RenderImageInterface * _image, 
								  Menge::EBlendFactor _src, 
								  Menge::EBlendFactor _dst ) override;

	void renderMesh( const Menge::TVertex* _vertices, std::size_t _verticesNum,
					const Menge::uint16 * _indices, std::size_t _indicesNum,
					Menge::TMaterial* _material ) override;

	void	renderLine( unsigned int _color, const float * _begin, const float * _end ) override;

	void	beginScene() override;
	void	endScene()	override;
	void	beginLayer2D() override;
	void	endLayer2D() override;
	void	beginLayer3D() override;
	void	endLayer3D() override;

	void	setRenderArea( const float* _renderArea ) override;

	void	setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen ) override;
	void	setRenderTarget( const Menge::String& _name, bool _clear ) override;

	Menge::CameraInterface * createCamera( const Menge::String & _name ) override;
	Menge::EntityInterface * createEntity( const Menge::String & _name, const Menge::String & _meshName ) override;
	Menge::LightInterface * createLight( const Menge::String & _name ) override;
	Menge::SceneNodeInterface * createSceneNode( const Menge::String & _name ) override;

	void releaseCamera( Menge::CameraInterface * _camera ) override;
	void releaseEntity( Menge::EntityInterface * _entity ) override;
	void releaseLight( Menge::LightInterface * _light ) override;
	void releaseSceneNode( Menge::SceneNodeInterface * _interface ) override;

	void setTextureFiltering( bool _filter ) override;
	Menge::SceneNodeInterface * getRootSceneNode() const override;
	void setEventListener( Menge::RenderSystemListener* _listener ) override;
	void onWindowMovedOrResized() override;
	void onWindowActive( bool _active ) override;
	void onWindowClose() override;

	int  getNumDIP() const override;

	void onRestoreDevice() override;

	void renderText(const Menge::String & _text, const float * _pos, unsigned long _color) override;

private:
	HGERenderFont * m_systemFont;
	Menge::LogSystemInterface* m_logSystem;
	mt::vec2f m_contentResolution;
	HGE* m_hge;
	float m_layer;
	mt::box2f m_viewport;
	bool m_inRender;
	Menge::String m_currentRenderTarget;

	struct RenderTargetInfo
	{
		HTARGET handle;
		HGETexture* texture;
		bool dirty;
	};

	typedef std::map<Menge::String, RenderTargetInfo> TTargetMap;
	TTargetMap m_targetMap;

	typedef std::map< Menge::String, Menge::RenderImageInterface* > TTextureMap;
	TTextureMap m_textureMap;

	bool m_layer3D;
	unsigned int m_clearColor;

	float m_renderX;
	float m_renderY;
};