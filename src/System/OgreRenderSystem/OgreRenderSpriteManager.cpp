#	include "OgreRenderSpriteManager.h"
#	include "OgreRenderImage.h"

const size_t	VERTEX_PER_QUAD			= 6;
const size_t	VERTEXBUFFER_INITIAL_CAPACITY	= 256;
const size_t    UNDERUSED_FRAME_THRESHOLD = 50000;

//////////////////////////////////////////////////////////////////////////
OgreRenderSpriteManager::OgreRenderSpriteManager()
: currentZ(1.0f)
, isSorted(false)
{
}
//////////////////////////////////////////////////////////////////////////
OgreRenderSpriteManager::~OgreRenderSpriteManager()
{
}
//////////////////////////////////////////////////////////////////////////
float OgreRenderSpriteManager::getCurrentZ()
{
	return currentZ;
}
//////////////////////////////////////////////////////////////////////////
void  OgreRenderSpriteManager::diffZ()
{
	//FIXME:
	currentZ -= 0.001;
}
//////////////////////////////////////////////////////////////////////////
static void createQuadRenderOp(Ogre::RenderOperation &renderOp, 
    Ogre::HardwareVertexBufferSharedPtr &vertexBuffer, size_t nquads)
{
	renderOp.vertexData = new Ogre::VertexData;
	renderOp.vertexData->vertexStart = 0;

	Ogre::VertexDeclaration* vd = renderOp.vertexData->vertexDeclaration;
	size_t vd_offset = 0;
	vd->addElement(0, vd_offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	vd_offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
	vd->addElement(0, vd_offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
	vd_offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);
	vd->addElement(0, vd_offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);

	vertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(vd->getVertexSize(0), nquads,  
        Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE, false);

	renderOp.vertexData->vertexBufferBinding->setBinding(0, vertexBuffer);

	renderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;
	renderOp.useIndexes = false;
}

static void destroyQuadRenderOp(Ogre::RenderOperation &renderOp, 
    Ogre::HardwareVertexBufferSharedPtr &vertexBuffer)
{
    delete renderOp.vertexData;
    renderOp.vertexData = 0;
    vertexBuffer.setNull();
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::init(Ogre::SceneManager* sceneMan, Ogre::RenderSystem * renderSys, Ogre::Viewport * viewport, Ogre::uint8 targetQueue, bool afterQueue)
{
	OgreRenderSpriteManager::sceneMan = sceneMan;
	OgreRenderSpriteManager::afterQueue = afterQueue;
	OgreRenderSpriteManager::targetQueue = targetQueue;

	m_renderSys = renderSys;
	m_viewport = viewport;

	sceneMan->addRenderQueueListener(this);

	createQuadRenderOp(renderOp, vertexBuffer, VERTEXBUFFER_INITIAL_CAPACITY);
    underusedFramecount = 0;

	m_renderSys = Ogre::Root::getSingleton().getRenderSystem();
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::end()
{
	destroyQuadRenderOp( renderOp, vertexBuffer );
	sceneMan->removeRenderQueueListener( this );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::renderQueueStarted(
									   Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &skipThisInvocation)
{
	if (!afterQueue && queueGroupId == targetQueue)
	{
	}
	currentZ = 1.0f;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::renderQueueEnded(
									 Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation)
{
	if (afterQueue && queueGroupId == targetQueue)
	{
		doRender();
		isSorted = true;
		quadList.clear();
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::prepareForRender()
{
	Ogre::LayerBlendModeEx		d_colourBlendMode;	
	Ogre::LayerBlendModeEx		d_alphaBlendMode;	
	Ogre::TextureUnitState::UVWAddressingMode d_uvwAddressMode;

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

	m_renderSys->_setWorldMatrix(Ogre::Matrix4::IDENTITY);
	m_renderSys->_setViewMatrix(Ogre::Matrix4::IDENTITY);
	m_renderSys->_setProjectionMatrix(Ogre::Matrix4::IDENTITY);

	m_renderSys->setLightingEnabled(false);
	m_renderSys->_setDepthBufferParams(false, false);
	m_renderSys->_setDepthBias(0, 0);
	m_renderSys->_setCullingMode(Ogre::CULL_NONE);
	m_renderSys->_setFog(Ogre::FOG_NONE);
	m_renderSys->_setColourBufferWriteEnabled(true, true, true, true);
	m_renderSys->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
	m_renderSys->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
	m_renderSys->setShadingType(Ogre::SO_GOURAUD);
	m_renderSys->_setPolygonMode(Ogre::PM_SOLID);

	m_renderSys->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
	m_renderSys->_setTextureCoordSet(0, 0);
	m_renderSys->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
	m_renderSys->_setTextureAddressingMode(0, d_uvwAddressMode);
	m_renderSys->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
	m_renderSys->_setAlphaRejectSettings(Ogre::CMPF_ALWAYS_PASS, 0);
	m_renderSys->_setTextureBlendMode(0, d_colourBlendMode);
	m_renderSys->_setTextureBlendMode(0, d_alphaBlendMode);
	m_renderSys->_disableTextureUnitsFrom(1);

	m_renderSys->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::addQuad1(const Ogre::Vector4 & _uv,const Ogre::Matrix3 & _transform, const Ogre::Vector2 & _offset, const Ogre::Vector2 & _size, float z,  const OgreRenderImage * image, unsigned int _color)
{
	isSorted = false;
	QuadInfo quad;
	
	quad.points[0] = Ogre::Vector3(_offset[0], _offset[1], 1.0f) * _transform;
	quad.points[1] = Ogre::Vector3(_offset[0] + _size[0], _offset[1], 1.0f) * _transform;
	quad.points[2] = Ogre::Vector3(_offset[0] + _size[0], _offset[1] + _size[1], 1.0f) * _transform;
	quad.points[3] = Ogre::Vector3(_offset[0], _offset[1] + _size[1], 1.0f) * _transform;

	quad.tcoord[0] = Ogre::Vector2(_uv[0], _uv[1]);
    quad.tcoord[1] = Ogre::Vector2(_uv[2], _uv[1]);
    quad.tcoord[2] = Ogre::Vector2(_uv[2], _uv[3]);
    quad.tcoord[3] = Ogre::Vector2(_uv[0], _uv[3]);

	float width = m_viewport->getActualWidth();
  	float heigth = m_viewport->getActualHeight();

	for( size_t i = 0; i < 4; ++i )
   	{
   		quad.points[i].x = CONVERT_MENGE_TO_OGRE_X(quad.points[i].x,width);
   		quad.points[i].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[i].y,heigth);
		quad.points[i].z = -1 + z;
   	}

	quad.texture = image->m_texture;

	quad.color = _color;
	
	quadList.insert(quad);
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::doRender(void)
{
	if( !isSorted )
	{
		isSorted = true;

		size_t size = vertexBuffer->getNumVertices();
		size_t requestedSize = quadList.size()*VERTEX_PER_QUAD;

		if( size < requestedSize )
		{
			while(size < requestedSize)
			{
				size *= 2;
			}
			
			destroyQuadRenderOp( renderOp, vertexBuffer );
			createQuadRenderOp( renderOp, vertexBuffer, size );
		}
		else if( requestedSize < size/2 && underusedFramecount >= UNDERUSED_FRAME_THRESHOLD )
		{
			size /= 2;
			destroyQuadRenderOp( renderOp, vertexBuffer );
			createQuadRenderOp( renderOp, vertexBuffer, size );
		
			underusedFramecount = 0;
		}
		
		QuadVertex * buffmem = static_cast<QuadVertex*>( vertexBuffer->lock( Ogre::HardwareVertexBuffer::HBL_DISCARD ) );
		
		for ( QuadList::iterator it = quadList.begin(); it != quadList.end(); ++it )
		{
			const QuadInfo& quad = (*it);
		
			buffmem->point = quad.points[0];
			buffmem->texcoord = quad.tcoord[0];
			buffmem->diffuse = quad.color;
			++buffmem;
			
			buffmem->point = quad.points[1];
			buffmem->texcoord = quad.tcoord[1];
			buffmem->diffuse = quad.color;
			++buffmem;

			buffmem->point = quad.points[2];
			buffmem->texcoord = quad.tcoord[2];
			buffmem->diffuse = quad.color;
			++buffmem;

			buffmem->point = quad.points[0];
			buffmem->texcoord = quad.tcoord[0];
			buffmem->diffuse = quad.color;
			++buffmem;

			buffmem->point = quad.points[2];
			buffmem->texcoord = quad.tcoord[2];
			buffmem->diffuse = quad.color;
			++buffmem;

			buffmem->point = quad.points[3];
			buffmem->texcoord = quad.tcoord[3];
			buffmem->diffuse = quad.color;
			++buffmem;
		}

		vertexBuffer->unlock();
	}

	size_t bufferPos = 0;
	bool first = true;

	QuadList::iterator it = quadList.begin();

	while( it != quadList.end() )
	{

		currTexture = it->texture;
		renderOp.vertexData->vertexStart = bufferPos;

		for ( ;it != quadList.end(); ++it )
		{
			const QuadInfo& quad = (*it);

			if ( currTexture != quad.texture )
			{
				break;
			}

			bufferPos += VERTEX_PER_QUAD;
		}

		renderOp.vertexData->vertexCount = bufferPos - renderOp.vertexData->vertexStart;

		m_renderSys->_setTexture(0, true, currTexture);

		if ( first )
		{
			prepareForRender();
			first = false;
		}

		m_renderSys->_render(renderOp);
	}

    if( bufferPos < vertexBuffer->getNumVertices()/2 )
	{
       underusedFramecount++;
	}
    else
	{
       underusedFramecount = 0;
	} 
}