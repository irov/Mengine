#include "OgreRenderSpriteManager.h"
#include <Ogre.h>
#include <OgreMesh.h>
#include <OgreHardwareBuffer.h>


#define OGRE2D_MINIMAL_HARDWARE_BUFFER_SIZE 120


OgreRenderSpriteManager::OgreRenderSpriteManager()
{
}

OgreRenderSpriteManager::~OgreRenderSpriteManager()
{
}

void OgreRenderSpriteManager::init(Ogre::SceneManager* sceneMan, Ogre::RenderSystem * renderSys, Ogre::uint8 targetQueue, bool afterQueue)
{
	OgreRenderSpriteManager::sceneMan=sceneMan;
	OgreRenderSpriteManager::afterQueue=afterQueue;
	OgreRenderSpriteManager::targetQueue=targetQueue;

	m_renderSys = renderSys;

	hardwareBuffer.setNull();

	sceneMan->addRenderQueueListener(this);
}

void OgreRenderSpriteManager::end()
{
	if (!hardwareBuffer.isNull())
		destroyHardwareBuffer();

	sceneMan->removeRenderQueueListener(this);
}

void OgreRenderSpriteManager::renderQueueStarted(
									   Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation)
{
	if (!afterQueue && queueGroupId==targetQueue)
		renderBuffer();
}

void OgreRenderSpriteManager::renderQueueEnded(
									 Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation)
{
	if (afterQueue && queueGroupId==targetQueue)
		renderBuffer();
}


void OgreRenderSpriteManager::renderBuffer()
{
	Ogre::RenderSystem* rs=Ogre::Root::getSingleton().getRenderSystem();
	std::list<RenderSprite>::iterator currSpr, endSpr;

	RenderVertexChunk thisChunk;
	std::list<RenderVertexChunk> chunks;

	unsigned int newSize;

	newSize=sprites.size()*6;
	if (newSize<OGRE2D_MINIMAL_HARDWARE_BUFFER_SIZE)
		newSize=OGRE2D_MINIMAL_HARDWARE_BUFFER_SIZE;

	// grow hardware buffer if needed
	if (hardwareBuffer.isNull() || hardwareBuffer->getNumVertices()<newSize)
	{
		if (!hardwareBuffer.isNull())
			destroyHardwareBuffer();

		createHardwareBuffer(newSize);
	}

	if (sprites.empty()) return;

	// write quads to the hardware buffer, and remember chunks
	float* buffer;
	float z=-1;

	buffer=(float*)hardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

	endSpr=sprites.end();
	currSpr=sprites.begin();
	thisChunk.texHandle=currSpr->texHandle;
	thisChunk.vertexCount=0;
	while (currSpr!=endSpr)
	{
		// 1st point (left bottom)
		*buffer=currSpr->x1; buffer++;
		*buffer=currSpr->y2; buffer++;
		*buffer=z; buffer++;
		*buffer=currSpr->tx1; buffer++;
		*buffer=currSpr->ty2; buffer++;
		// 2st point (right top)
		*buffer=currSpr->x2; buffer++;
		*buffer=currSpr->y1; buffer++;
		*buffer=z; buffer++;
		*buffer=currSpr->tx2; buffer++;
		*buffer=currSpr->ty1; buffer++;
		// 3rd point (left top)
		*buffer=currSpr->x1; buffer++;
		*buffer=currSpr->y1; buffer++;
		*buffer=z; buffer++;
		*buffer=currSpr->tx1; buffer++;
		*buffer=currSpr->ty1; buffer++;

		// 4th point (left bottom)
		*buffer=currSpr->x1; buffer++;
		*buffer=currSpr->y2; buffer++;
		*buffer=z; buffer++;
		*buffer=currSpr->tx1; buffer++;
		*buffer=currSpr->ty2; buffer++;
		// 5th point (right bottom)
		*buffer=currSpr->x2; buffer++;
		*buffer=currSpr->y1; buffer++;
		*buffer=z; buffer++;
		*buffer=currSpr->tx2; buffer++;
		*buffer=currSpr->ty1; buffer++;
		// 6th point (right top)
		*buffer=currSpr->x2; buffer++;
		*buffer=currSpr->y2; buffer++;
		*buffer=z; buffer++;
		*buffer=currSpr->tx2; buffer++;
		*buffer=currSpr->ty2; buffer++;

		// remember this chunk
		thisChunk.vertexCount+=6;
		currSpr++;
		if (currSpr==endSpr || thisChunk.texHandle!=currSpr->texHandle)
		{
			chunks.push_back(thisChunk);
			if (currSpr!=endSpr)
			{
				thisChunk.texHandle=currSpr->texHandle;
				thisChunk.vertexCount=0;
			}
		}
	}

	hardwareBuffer->unlock();



	// set up...
	prepareForRender();


	// do the real render!
	Ogre::TexturePtr tp;
	std::list<RenderVertexChunk>::iterator currChunk, endChunk;

	endChunk=chunks.end();
	renderOp.vertexData->vertexStart=0;
	for (currChunk=chunks.begin(); currChunk!=endChunk; currChunk++)
	{
		renderOp.vertexData->vertexCount=currChunk->vertexCount;
		tp=Ogre::TextureManager::getSingleton().getByHandle(currChunk->texHandle);
		rs->_setTexture(0, true, tp->getName());
		rs->_render(renderOp);
		renderOp.vertexData->vertexStart+=currChunk->vertexCount;
	}

	// sprites go home!
	sprites.clear();
}

void OgreRenderSpriteManager::prepareForRender()
{
	Ogre::LayerBlendModeEx colorBlendMode;
	Ogre::LayerBlendModeEx alphaBlendMode;
	Ogre::TextureUnitState::UVWAddressingMode uvwAddressMode;

	Ogre::RenderSystem* rs = m_renderSys;

	colorBlendMode.blendType=Ogre::LBT_COLOUR;
	colorBlendMode.source1=Ogre::LBS_TEXTURE;
	colorBlendMode.operation=Ogre::LBX_SOURCE1;

	alphaBlendMode.blendType=Ogre::LBT_ALPHA;
	alphaBlendMode.source1=Ogre::LBS_TEXTURE;
	alphaBlendMode.operation=Ogre::LBX_SOURCE1;

	uvwAddressMode.u=Ogre::TextureUnitState::TAM_CLAMP;
	uvwAddressMode.v=Ogre::TextureUnitState::TAM_CLAMP;
	uvwAddressMode.w=Ogre::TextureUnitState::TAM_CLAMP;

	rs->_setWorldMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setViewMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setProjectionMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
	rs->_setTextureCoordSet(0, 0);
	rs->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
	rs->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
	rs->_setTextureBlendMode(0, colorBlendMode);
	rs->_setTextureBlendMode(0, alphaBlendMode);
	rs->_setTextureAddressingMode(0, uvwAddressMode);
	rs->_disableTextureUnitsFrom(1);
	rs->setLightingEnabled(false);
	rs->_setFog(Ogre::FOG_NONE);
	rs->_setCullingMode(Ogre::CULL_NONE);
	rs->_setDepthBufferParams(false, false);
	rs->_setColourBufferWriteEnabled(true, true, true, false);
	rs->setShadingType(Ogre::SO_GOURAUD);
	rs->_setPolygonMode(Ogre::PM_SOLID);
	rs->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
	rs->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
	rs->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
	rs->_setAlphaRejectSettings(Ogre::CMPF_ALWAYS_PASS, 0);
}


void OgreRenderSpriteManager::createHardwareBuffer(unsigned int size)
{
	Ogre::VertexDeclaration* vd;

	renderOp.vertexData=new Ogre::VertexData;
	renderOp.vertexData->vertexStart=0;

	vd=renderOp.vertexData->vertexDeclaration;
	vd->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	vd->addElement(0, Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3),
		Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

	hardwareBuffer=Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		vd->getVertexSize(0),
		size,// buffer size
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
		false);// use shadow buffer? no

	renderOp.vertexData->vertexBufferBinding->setBinding(0, hardwareBuffer);

	renderOp.operationType=Ogre::RenderOperation::OT_TRIANGLE_LIST;
	renderOp.useIndexes=false;


}

void OgreRenderSpriteManager::destroyHardwareBuffer()
{
	delete renderOp.vertexData;
	renderOp.vertexData=0;
	hardwareBuffer.setNull();
}

void OgreRenderSpriteManager::spriteBltFull(
								  std::string textureName,
								  double x1, double y1, double x2, double y2,
								  double tx1, double ty1, double tx2, double ty2)
{
	Ogre::TexturePtr tp;
	RenderSprite spr;

	spr.x1=x1;
	spr.y1=y1;
	spr.x2=x2;
	spr.y2=y2;

	spr.tx1=tx1;
	spr.ty1=ty1;
	spr.tx2=tx2;
	spr.ty2=ty2;

	tp = Ogre::TextureManager::getSingleton().getByName(textureName);
	spr.texHandle=tp->getHandle();

	sprites.push_back(spr);
}