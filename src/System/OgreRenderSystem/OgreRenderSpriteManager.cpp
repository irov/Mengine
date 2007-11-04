#	include "OgreRenderSpriteManager.h"

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
	
		m_layers.clear();
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::renderBuffer()
{
	Ogre::RenderSystem * rs = Ogre::Root::getSingleton().getRenderSystem();

//	printf("# \n");
	for( std::list<Layer>::iterator it = m_layers.begin(); 
		it != m_layers.end(); ++it)
	{
		RenderSpriteList & spriteList = it->m_sprites;

//		printf("size = %d \n",it->m_sprites.size());

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

		float z = -1;

		QuadVertex * buffer  = (QuadVertex*)hardwareBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

		endSpr = spriteList.end();
		currSpr = spriteList.begin();
		thisChunk.texHandle = currSpr->texHandle;
		thisChunk.vertexCount = 0;

		while (currSpr != endSpr)
		{
			buffer->point = currSpr->point[0];
			buffer->point.z = z;
			buffer->color = currSpr->color[0];
			buffer->tcoord = currSpr->tcoord[0];

			++buffer;

			buffer->point = currSpr->point[1];
			buffer->point.z = z;
			buffer->color = currSpr->color[1];
			buffer->tcoord = currSpr->tcoord[1];


			++buffer;

			buffer->point = currSpr->point[2];
			buffer->point.z = z;
			buffer->color = currSpr->color[2];
			buffer->tcoord = currSpr->tcoord[2];

			++buffer;

			buffer->point = currSpr->point[0];
			buffer->point.z = z;
			buffer->color = currSpr->color[0];
			buffer->tcoord = currSpr->tcoord[0];

			++buffer;

			buffer->point = currSpr->point[2];
			buffer->point.z = z;
			buffer->color = currSpr->color[2];
			buffer->tcoord = currSpr->tcoord[2];

			++buffer;
		
			buffer->point = currSpr->point[3];
			buffer->point.z = z;
			buffer->color = currSpr->color[3];
			buffer->tcoord = currSpr->tcoord[3];

			++buffer;

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
	Ogre::RenderSystem * d_render_sys = m_renderSys;

	Ogre::LayerBlendModeEx		d_colourBlendMode;	//!< Controls colour blending mode used.
	Ogre::LayerBlendModeEx		d_alphaBlendMode;	//!< Controls alpha blending mode used.
	Ogre::TextureUnitState::UVWAddressingMode d_uvwAddressMode;
	// Initialise blending modes to be used.
	d_colourBlendMode.blendType	= Ogre::LBT_COLOUR;
	d_colourBlendMode.source1	= Ogre::LBS_TEXTURE;
	d_colourBlendMode.source2	= Ogre::LBS_DIFFUSE;
	d_colourBlendMode.operation	= Ogre::LBX_MODULATE;

	d_alphaBlendMode.blendType	= Ogre::LBT_ALPHA;
	d_alphaBlendMode.source1	= Ogre::LBS_TEXTURE;
	d_alphaBlendMode.source2	= Ogre::LBS_DIFFUSE;
	d_alphaBlendMode.operation	= Ogre::LBX_MODULATE;

	d_uvwAddressMode.u = Ogre::TextureUnitState::TAM_CLAMP;
	d_uvwAddressMode.v = Ogre::TextureUnitState::TAM_CLAMP;
	d_uvwAddressMode.w = Ogre::TextureUnitState::TAM_CLAMP;

		// set-up matrices
	d_render_sys->_setWorldMatrix(Ogre::Matrix4::IDENTITY);
	d_render_sys->_setViewMatrix(Ogre::Matrix4::IDENTITY);
	d_render_sys->_setProjectionMatrix(Ogre::Matrix4::IDENTITY);

	// initialise render settings
	d_render_sys->setLightingEnabled(false);
	d_render_sys->_setDepthBufferParams(false, false);
	d_render_sys->_setDepthBias(0, 0);
	d_render_sys->_setCullingMode(Ogre::CULL_NONE);
	d_render_sys->_setFog(Ogre::FOG_NONE);
	d_render_sys->_setColourBufferWriteEnabled(true, true, true, true);
	d_render_sys->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
	d_render_sys->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
	d_render_sys->setShadingType(Ogre::SO_GOURAUD);
	d_render_sys->_setPolygonMode(Ogre::PM_SOLID);

	// initialise texture settings
	d_render_sys->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
	d_render_sys->_setTextureCoordSet(0, 0);
	d_render_sys->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
	d_render_sys->_setTextureAddressingMode(0, d_uvwAddressMode);
	d_render_sys->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
	d_render_sys->_setAlphaRejectSettings(Ogre::CMPF_ALWAYS_PASS, 0);
	d_render_sys->_setTextureBlendMode(0, d_colourBlendMode);
	d_render_sys->_setTextureBlendMode(0, d_alphaBlendMode);
	d_render_sys->_disableTextureUnitsFrom(1);

	// enable alpha blending
	d_render_sys->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
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

	vd->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);

	vd->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	
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
				   const Ogre::Vector2 & _size,
				   unsigned int _color)
{
	RenderSprite spr;

	spr.point[0] = Ogre::Vector3(_offset.x, _offset.y, 1.0f);
	spr.point[1] = Ogre::Vector3(_offset.x + _size.x, _offset.y, 1.0f);
	spr.point[2] = Ogre::Vector3(_offset.x + _size.x, _offset.y + _size.y, 1.0f);
	spr.point[3] = Ogre::Vector3(_offset.x, _offset.y + _size.y, 1.0f);

	Ogre::ColourValue col;
	col.setAsARGB(_color);
	
	Ogre::ARGB converted_color;

	m_renderSys->convertColourValue(col,&converted_color);
	spr.color[0] = spr.color[1] = spr.color[2] = spr.color[3] = converted_color;
	
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