#	pragma once

#	include <Ogre.h>
#	include <OgreRenderQueueListener.h>

#	include <string>
#	include <list>

struct RenderSprite
{
	Ogre::Vector3 point[4];
	Ogre::Vector2 tcoord[4];
	//float x1, y1, x2, y2;// sprite coordinates
	//float tx1, ty1, tx2, ty2;// texture coordinates
	Ogre::ResourceHandle texHandle;// texture handle
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
		const Ogre::Vector2 & _size);

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
	Ogre::SceneManager * sceneMan;
	Ogre::RenderSystem * m_renderSys;
	Ogre::Viewport * m_viewport;

	Ogre::uint8 targetQueue;
	bool afterQueue;

	// ogre specifics
	Ogre::RenderOperation renderOp;
	Ogre::HardwareVertexBufferSharedPtr hardwareBuffer;

	// sprite buffer
	std::list<RenderSprite> sprites;
};
