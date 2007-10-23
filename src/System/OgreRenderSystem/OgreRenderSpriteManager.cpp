#	include "OgreRenderSpriteManager.h"

#	include "OgreRenderFont.h"

#	define OGRE2D_MINIMAL_HARDWARE_BUFFER_SIZE 120

//////////////////////////////////////////////////////////////////////////
OgreRenderSpriteManager::OgreRenderSpriteManager()
{
}
//////////////////////////////////////////////////////////////////////////
OgreRenderSpriteManager::~OgreRenderSpriteManager()
{
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::init(Ogre::SceneManager* sceneMan, Ogre::RenderSystem * renderSys, Ogre::Viewport * viewport, Ogre::uint8 targetQueue, bool afterQueue)
{
	OgreRenderSpriteManager::sceneMan = sceneMan;
	OgreRenderSpriteManager::afterQueue = afterQueue;
	OgreRenderSpriteManager::targetQueue = targetQueue;

	m_renderSys = renderSys;
	m_viewport = viewport;

	hardwareBuffer.setNull();

	sceneMan->addRenderQueueListener(this);

	firstInit = true;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::end()
{
	if (!hardwareBuffer.isNull())
	{
		destroyHardwareBuffer();
	}

	sceneMan->removeRenderQueueListener(this);
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::renderQueueStarted(
									   Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation)
{
	if (!afterQueue && queueGroupId == targetQueue)
	{
		renderBuffer();
	}

	firstInit = true;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::renderQueueEnded(
									 Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation)
{
	if (afterQueue && queueGroupId == targetQueue)
	{
		renderBuffer();
	
		for( std::list<Layer>::iterator itList = m_layers.begin(); 
		itList != m_layers.end(); ++itList)
		{
			itList->clear();
		}

		m_layers.clear();
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::renderBuffer()
{
	Ogre::RenderSystem * rs = Ogre::Root::getSingleton().getRenderSystem();

	for( std::list<Layer>::iterator it = m_layers.begin(); 
		it != m_layers.end(); ++it)
	{
		RenderSpriteList & spriteList = it->m_sprites;

		RenderSpriteList::iterator currSpr;
		RenderSpriteList::iterator endSpr;

		RenderVertexChunk thisChunk;
		std::list<RenderVertexChunk> chunks;

		size_t newSize = spriteList.size() * 6;

		if (newSize < OGRE2D_MINIMAL_HARDWARE_BUFFER_SIZE)
		{
			newSize = OGRE2D_MINIMAL_HARDWARE_BUFFER_SIZE;
		}

		// grow hardware buffer if needed
		if (hardwareBuffer.isNull() || hardwareBuffer->getNumVertices()<newSize)
		{
			if (!hardwareBuffer.isNull())
			{
				destroyHardwareBuffer();
			}

			createHardwareBuffer(newSize);
		}

		if (spriteList.empty()) 
		{
			continue;
		}

		// write quads to the hardware buffer, and remember chunks
		
		float z = -1;

		float* buffer = (float*)hardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

		endSpr = spriteList.end();
		currSpr = spriteList.begin();
		thisChunk.texHandle = currSpr->texHandle;
		thisChunk.vertexCount = 0;

		while (currSpr != endSpr)
		{
			// 1st point (left bottom)
			*buffer = currSpr->point[0].x; buffer++;
			*buffer = currSpr->point[0].y; buffer++;
			*buffer = z; buffer++;

			#ifdef COLOR
			*buffer = currSpr->color[0]; buffer++;
			#endif

			*buffer = currSpr->tcoord[0].x; buffer++;
			*buffer = currSpr->tcoord[0].y; buffer++;

			// 2st point (right top)
			*buffer = currSpr->point[1].x; buffer++;
			*buffer = currSpr->point[1].y; buffer++;
			*buffer = z; buffer++;

			#ifdef COLOR
			*buffer = currSpr->color[1]; buffer++;
			#endif

			*buffer = currSpr->tcoord[1].x; buffer++;
			*buffer = currSpr->tcoord[1].y; buffer++;



			// 3rd point (left top)
			*buffer=currSpr->point[2].x; buffer++;
			*buffer=currSpr->point[2].y; buffer++;
			*buffer=z; buffer++;

			#ifdef COLOR
			*buffer = currSpr->color[2]; buffer++;
			#endif

			*buffer=currSpr->tcoord[2].x; buffer++;
			*buffer=currSpr->tcoord[2].y; buffer++;

			// 4th point (left bottom)
			*buffer=currSpr->point[0].x; buffer++;
			*buffer=currSpr->point[0].y; buffer++;
			*buffer=z; buffer++;

			#ifdef COLOR
			*buffer = currSpr->color[0]; buffer++;
			#endif

			*buffer=currSpr->tcoord[0].x; buffer++;
			*buffer=currSpr->tcoord[0].y; buffer++;

			// 5th point (right bottom)
			*buffer=currSpr->point[2].x; buffer++;
			*buffer=currSpr->point[2].y; buffer++;
			*buffer=z; buffer++;

			#ifdef COLOR
			*buffer = currSpr->color[2]; buffer++;
			#endif

			*buffer=currSpr->tcoord[2].x; buffer++;
			*buffer=currSpr->tcoord[2].y; buffer++;

			// 6th point (right top)
			*buffer=currSpr->point[3].x; buffer++;
			*buffer=currSpr->point[3].y; buffer++;
			*buffer=z; buffer++;

			#ifdef COLOR
			*buffer = currSpr->color[3]; buffer++;
			#endif

			*buffer=currSpr->tcoord[3].x; buffer++;
			*buffer=currSpr->tcoord[3].y; buffer++;

			// remember this chunk
			thisChunk.vertexCount+=6;
			currSpr++;

			if (currSpr == endSpr || thisChunk.texHandle != currSpr->texHandle)
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

		prepareForRender();

		// do the real render!
		Ogre::TexturePtr tp;

		std::list<RenderVertexChunk>::iterator currChunk;
		std::list<RenderVertexChunk>::iterator endChunk;

		endChunk = chunks.end();
		renderOp.vertexData->vertexStart=0;

		for (currChunk = chunks.begin(); currChunk != endChunk; currChunk++)
		{
			renderOp.vertexData->vertexCount=currChunk->vertexCount;
			tp=Ogre::TextureManager::getSingleton().getByHandle(currChunk->texHandle);

			if(tp.isNull() == false )
			rs->_setTexture(0, true, tp->getName());

			rs->_render(renderOp);
			renderOp.vertexData->vertexStart += currChunk->vertexCount;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::prepareForRender()
{
	Ogre::LayerBlendModeEx colorBlendMode;
	Ogre::LayerBlendModeEx alphaBlendMode;
	Ogre::TextureUnitState::UVWAddressingMode uvwAddressMode;

	Ogre::RenderSystem * rs = m_renderSys;

	colorBlendMode.blendType=Ogre::LBT_COLOUR;
	colorBlendMode.source1=Ogre::LBS_TEXTURE;
	colorBlendMode.operation=Ogre::LBX_SOURCE1;

	alphaBlendMode.blendType=Ogre::LBT_ALPHA;
	alphaBlendMode.source1=Ogre::LBS_TEXTURE;
	alphaBlendMode.operation=Ogre::LBX_SOURCE1;

	uvwAddressMode.u = Ogre::TextureUnitState::TAM_CLAMP;
	uvwAddressMode.v = Ogre::TextureUnitState::TAM_CLAMP;
	uvwAddressMode.w = Ogre::TextureUnitState::TAM_CLAMP;

	rs->_setWorldMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setViewMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setProjectionMatrix(Ogre::Matrix4::IDENTITY);
	rs->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
	rs->_setTextureCoordSet(0, 0);
	rs->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
	rs->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);

	#ifndef COLOR
	rs->_setTextureBlendMode(0, colorBlendMode);
	#endif

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
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::createHardwareBuffer( size_t size)
{
	renderOp.vertexData = new Ogre::VertexData;
	renderOp.vertexData->vertexStart = 0;

	Ogre::VertexDeclaration * vd = renderOp.vertexData->vertexDeclaration;

	size_t offset = 0;
		
	vd->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

#ifdef COLOR
	vd->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
#endif

	vd->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
	
	hardwareBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		vd->getVertexSize(0),
		size,
		Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE,
		false);

	renderOp.vertexData->vertexBufferBinding->setBinding(0, hardwareBuffer);

	renderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
	renderOp.useIndexes = false;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::destroyHardwareBuffer()
{
	delete renderOp.vertexData;
	renderOp.vertexData = 0;
	hardwareBuffer.setNull();
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::spriteBltFull(
				   Ogre::Texture * _texture, 
				   const Ogre::Matrix3 & _transform,
				   const Ogre::Vector2 & _offset,
				   const Ogre::Vector4 & _uv,
				   const Ogre::Vector2 & _size)
{
	RenderSprite spr;

	spr.point[0] = Ogre::Vector3(_offset.x, _offset.y, 1.0f);
	spr.point[1] = Ogre::Vector3(_offset.x + _size.x, _offset.y, 1.0f);
	spr.point[2] = Ogre::Vector3(_offset.x + _size.x, _offset.y + _size.y, 1.0f);
	spr.point[3] = Ogre::Vector3(_offset.x, _offset.y + _size.y, 1.0f);

	#ifdef COLOR
	spr.color[0] = spr.color[1] = spr.color[2] = spr.color[3] = Ogre::ColourValue(0,0,1,0).getAsRGBA();
	#endif

	Ogre::Vector3 transformed_pos;

	float width = m_viewport->getActualWidth();
	float heigth = m_viewport->getActualHeight();

	for( size_t i = 0; i < 4; ++i )
	{
		transformed_pos = spr.point[i] * _transform;

		transformed_pos.x = CONVERT_MENGE_TO_OGRE_X(transformed_pos.x,width);
		transformed_pos.y = CONVERT_MENGE_TO_OGRE_Y(transformed_pos.y,heigth);

		spr.point[i] = transformed_pos;
	}

	spr.tcoord[0] = Ogre::Vector2(_uv[0], _uv[1]);
	spr.tcoord[1] = Ogre::Vector2(_uv[2], _uv[1]);
	spr.tcoord[2] = Ogre::Vector2(_uv[2], _uv[3]);
	spr.tcoord[3] = Ogre::Vector2(_uv[0], _uv[3]);

	spr.texHandle = _texture->getHandle();

	m_currentLayer->addSprite(spr);
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::beginLayer()
{
	addLayer();

	if( firstInit )
	{
		firstInit = false;
		m_currentLayer = m_layers.begin();
		return;
	}

	m_currentLayer++;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::endLayer()
{
}
//////////////////////////////////////////////////////////////////////////