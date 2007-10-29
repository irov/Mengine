#	pragma once

#	include <Ogre.h>

#	include <string>
#	include <list>

class	OgreRenderFont;

#	define CONVERT_MENGE_TO_OGRE_X(x,width) (2.0f * x / width - 1)
#	define CONVERT_MENGE_TO_OGRE_Y(y,heigth) (1 - 2.0f * y / heigth)

struct RenderSprite
{
	Ogre::Vector3 point[4];
	unsigned int  color[4];
	Ogre::Vector2 tcoord[4];
	Ogre::ResourceHandle texHandle;
};

struct QuadVertex
{
	Ogre::Vector3 point;
	unsigned int  color;
	Ogre::Vector2 tcoord;
};

struct RenderVertexChunk {
	Ogre::ResourceHandle texHandle;
	unsigned int vertexCount;
};

class OgreRenderSpriteManager
	: public Ogre::RenderQueueListener
{
public:
	OgreRenderSpriteManager();
	~OgreRenderSpriteManager();

	/// Initializes this 2d Manager
	/** and registers it as the render queue listener.*/
	void init( 
		Ogre::SceneManager* sceneMan, 
		Ogre::RenderSystem * renderSys, 
		Ogre::Viewport * viewport,
		Ogre::uint8 targetQueue, 
		bool afterQueue);

	/// Finishes Ogre 2d Manager
	void end();

	/// Called by Ogre, for being a render queue listener
	virtual void renderQueueStarted(
		Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation);
	/// Called by Ogre, for being a render queue listener
	virtual void renderQueueEnded(
		Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation);


	/// Buffers a sprite to be sent to the screen at render time.
	/**
	Sprite coordinates are in screen space: top left pixel is (-1, 1), and bottom right
	is (1, -1). The texture space, instead, ranges from (0, 0) to (1, 1).

	/param textureName Name of the texture to use in this sprite (remember: texture
	name, not material name!). The texture has to be already loaded by Ogre for this
	to work.
	/param x1 x coordinate for the top left point in the sprite.
	/param y1 y coordinate for the top left point in the sprite.
	/param x2 x coordinate for the bottom right point in the sprite.
	/param y2 y coordinate for the bottom right point in the sprite.
	/param tx1 u coordinate for the texture, in the top left point of the sprite.
	/param ty1 v coordinate for the texture, in the top left point of the sprite.
	/param tx2 u coordinate for the texture, in the bottom right point of the sprite.
	/param ty2 u coordinate for the texture, in the bottom right point of the sprite.
	*/
	void spriteBltFull(
		Ogre::Texture * _texture, 
		const Ogre::Matrix3 & _transform,
		const Ogre::Vector2 & _offset,
		const Ogre::Vector4 & _uv,
		const Ogre::Vector2 & _size,
		unsigned int _color);
 
	/// 
	void beginLayer();

	///
	void endLayer();

private:
	/// Render all the 2d data stored in the hardware buffers.
	void renderBuffer();

	/// Create a new hardware buffer
	/**
	/param size Vertex count for the new hardware buffer.
	*/
	void createHardwareBuffer( size_t size );
	/// Destroy the hardware buffer
	void destroyHardwareBuffer();
	/// Set Ogre for rendering
	void prepareForRender();

private:
	bool	firstInit;
	Ogre::SceneManager * sceneMan;
	Ogre::RenderSystem * m_renderSys;
	Ogre::Viewport * m_viewport;

	Ogre::uint8 targetQueue;
	bool afterQueue;

	// ogre specifics
	Ogre::RenderOperation renderOp;
	Ogre::HardwareVertexBufferSharedPtr hardwareBuffer;

	// sprite buffer

	typedef std::list<RenderSprite> RenderSpriteList;

	struct	Layer
	{
		RenderSpriteList	m_sprites;
	
		Layer()
		{
		}

		void	addSprite(const RenderSprite& spr)
		{
			m_sprites.push_back(spr);
		}

		void	clear()
		{
			m_sprites.clear();
		}
	};

	void	addLayer()
	{
		Layer layer;

		m_layers.push_back( layer );
	}

	std::list<Layer>	m_layers;
	std::list<Layer>::iterator	m_currentLayer;
};
