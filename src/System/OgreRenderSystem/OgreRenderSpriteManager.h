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

// FIXME: уменьшить кол памяти на вершины
struct QuadInfo
{
	Ogre::TexturePtr	texture;

	Ogre::Vector3		points[4];
	Ogre::Vector2		tcoord[4];

    unsigned int        color;

	bool operator<(const QuadInfo& other) const
	{
		return points[0].z > other.points[0].z;
	}
};

class OgreRenderSpriteManager
	: public Ogre::RenderQueueListener
{
public:
	void addQuad1(const Ogre::Vector4 & _uv,const Ogre::Matrix3 & _transform, const Ogre::Vector2 & _offset, const Ogre::Vector2 & _size, float z, const OgreRenderImage * image, unsigned int _color);

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

private:
	void prepareForRender();
	void doRender();

	float currentZ;
	Ogre::SceneManager * sceneMan;
	Ogre::RenderSystem * m_renderSys;
	Ogre::Viewport * m_viewport;
	Ogre::uint8 targetQueue;
	bool afterQueue;
	bool isSorted;
	size_t underusedFramecount; 
	Ogre::HardwareVertexBufferSharedPtr	vertexBuffer;
	Ogre::TexturePtr			currTexture;	
	Ogre::RenderOperation		renderOp;
	typedef std::multiset<QuadInfo>		QuadList;
	QuadList quadList;
};
