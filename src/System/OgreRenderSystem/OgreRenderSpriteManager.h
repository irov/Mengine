#	pragma once

#	include <Ogre.h>

#	include <string>
#	include <list>

class	OgreRenderImage;

#	define CONVERT_MENGE_TO_OGRE_X(x,width) (2.0f * x / width - 1)
#	define CONVERT_MENGE_TO_OGRE_Y(y,heigth) (1 - 2.0f * y / heigth)

struct QuadVertex 
{
	Ogre::Vector3 point;
	Ogre::RGBA diffuse;	
	Ogre::Vector2 texcoord;
};

struct QuadInfo
{
	Ogre::TexturePtr	texture;

	Ogre::Vector2	points[4];
	Ogre::Vector4	uv;

	Ogre::SceneBlendFactor source;
	Ogre::SceneBlendFactor dest;

	float z;

    unsigned int        color;

	bool operator<(const QuadInfo& other) const
	{
		return z > other.z;
	}
};

class OgreRenderSpriteManager
	: public Ogre::RenderQueueListener
	, public Ogre::RenderTargetListener
{
public:
	void addQuad1( Ogre::Viewport* _viewport, const Ogre::Vector2 & _contentRes,
		const Ogre::Vector4 & _uv,const Ogre::Matrix3 & _transform,
		const Ogre::Vector2 & _offset,
		const Ogre::Vector2 & _size, float z,
		const OgreRenderImage * image,
		unsigned int _color,
		Ogre::SceneBlendFactor _src,
		Ogre::SceneBlendFactor _dst,
		Ogre::uint8 _renderQueue = Ogre::RENDER_QUEUE_OVERLAY );

	void addQuad2( Ogre::Viewport* _viewport, const Ogre::Vector2 & _contentRes,
		const Ogre::Vector4 & _uv,
		const Ogre::Matrix3 & _transform,
		const Ogre::Vector2 & _a,
		const Ogre::Vector2 & _b,
		const Ogre::Vector2 & _c,
		const Ogre::Vector2 & _d,
		float z,
		const OgreRenderImage * image,
		unsigned int _color,
		Ogre::SceneBlendFactor _src,
		Ogre::SceneBlendFactor _dst,
		Ogre::uint8 _renderQueue = Ogre::RENDER_QUEUE_OVERLAY );

	OgreRenderSpriteManager();
	~OgreRenderSpriteManager();

	void init( 
		Ogre::SceneManager* sceneMan, 
		Ogre::RenderSystem * renderSys, 
		Ogre::Viewport * viewport,
		Ogre::uint8 targetQueue, 
		bool afterQueue);

	void end();

	virtual void renderQueueStarted(
		Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation);
	
	virtual void renderQueueEnded(
		Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation);

	float getCurrentZ();
	void  diffZ();

	void prepareForRender();

	typedef std::vector<QuadInfo> TQuadList;
	void doRender( TQuadList& _quadList );


	virtual void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) { }
	virtual void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) { }
	virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
	virtual void postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) { }
	virtual void viewportAdded(const Ogre::RenderTargetViewportEvent& evt) {}
	virtual void viewportRemoved(const Ogre::RenderTargetViewportEvent& evt) {}


private:

	float currentZ;
	Ogre::SceneManager * sceneMan;
	Ogre::RenderSystem * m_renderSys;
	Ogre::Viewport * m_viewport;
	//Ogre::uint8 targetQueue;
	bool afterQueue;
	bool isSorted;
	size_t underusedFramecount; 
	Ogre::HardwareVertexBufferSharedPtr	vertexBuffer;
	Ogre::TexturePtr			currTexture;	
	Ogre::Camera*				currCamera;
	Ogre::Viewport*				currViewport;
	Ogre::RenderOperation		renderOp;
	float m_vTexelOffset;
	float m_hTexelOffset;
	//TQuadList m_quadList;
	typedef std::map< Ogre::Viewport*, TQuadList > TQuadMap;
	TQuadMap m_quadMap;
	//TQuadList m_quadListPostrender;
};