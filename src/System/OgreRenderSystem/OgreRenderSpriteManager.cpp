#	include "OgreRenderSpriteManager.h"
#	include "OgreRenderImage.h"

const size_t	VERTEX_PER_QUAD			= 6;
const size_t	VERTEXBUFFER_INITIAL_CAPACITY	= 70000;
const size_t    UNDERUSED_FRAME_THRESHOLD = 70000;
const size_t    VECTOR_CAPACITY = 40000;

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

/*	renderOp.indexData = new Ogre::IndexData;
	renderOp.indexData->indexStart = 0;

	renderOp.indexData->indexCount = nquads;

	renderOp.indexData->indexBuffer = 
		Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
		Ogre::HardwareIndexBuffer::IT_16BIT, 
		nquads, //??
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
*/
//	renderOp.useIndexes = true;
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

	quadList.reserve( VECTOR_CAPACITY );
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
		//prepareForRender();
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

	//d_uvwAddressMode.u = Ogre::TextureUnitState::TAM_WRAP;
	//d_uvwAddressMode.v = Ogre::TextureUnitState::TAM_WRAP;
	//d_uvwAddressMode.w = Ogre::TextureUnitState::TAM_WRAP;

	m_renderSys->_setWorldMatrix(Ogre::Matrix4::IDENTITY);
	m_renderSys->_setViewMatrix(Ogre::Matrix4::IDENTITY);
	m_renderSys->_setProjectionMatrix(Ogre::Matrix4::IDENTITY);

	m_renderSys->_setCullingMode(Ogre::CULL_NONE);
	m_renderSys->setLightingEnabled(false);
	m_renderSys->_setDepthBufferParams(false, false);
	m_renderSys->_setDepthBias(0, 0);
	m_renderSys->_setFog(Ogre::FOG_NONE);
	m_renderSys->_setColourBufferWriteEnabled(true, true, true, true);
	m_renderSys->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
	m_renderSys->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
	m_renderSys->setShadingType(Ogre::SO_GOURAUD);
	m_renderSys->_setPolygonMode(Ogre::PM_SOLID);

	//m_renderSys->_setPolygonMode(Ogre::PM_WIREFRAME);

	m_renderSys->_setTextureCoordCalculation(0, Ogre::TEXCALC_NONE);
	m_renderSys->_setTextureCoordSet(0, 0);
	m_renderSys->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_POINT, Ogre::FO_POINT );
	m_renderSys->_setTextureAddressingMode(0, d_uvwAddressMode);
	m_renderSys->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
	m_renderSys->_setAlphaRejectSettings(Ogre::CMPF_ALWAYS_PASS, 0);
	m_renderSys->_setTextureBlendMode(0, d_colourBlendMode);
	m_renderSys->_setTextureBlendMode(0, d_alphaBlendMode);
	m_renderSys->_disableTextureUnitsFrom(1);

	m_renderSys->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::addQuad1( Ogre::Viewport* _viewport,
									   const Ogre::Vector2 & _contentRes,
									   const Ogre::Vector4 & _uv,
									   const Ogre::Matrix3 & _transform,
									   const Ogre::Vector2 & _offset,
									   const Ogre::Vector2 & _size,
									   float z,
									   const OgreRenderImage * image,
									   unsigned int _color,
									   Ogre::SceneBlendFactor _src,
									   Ogre::SceneBlendFactor _dst)

{
	isSorted = false;
	QuadInfo quad;
	
	float width = _contentRes.x;
  	float heigth = _contentRes.y;

	//FIXME:

	quad.viewport = _viewport;

	quad.points[0].x = _transform[0][0] * _offset[0] + _transform[1][0] * _offset[1] + _transform[2][0];
	quad.points[0].y = _transform[0][1] * _offset[0] + _transform[1][1] * _offset[1] + _transform[2][1];

	quad.points[1].x = quad.points[0].x + _transform[0][0] * _size[0];
	quad.points[1].y = quad.points[0].y + _transform[0][1] * _size[0];

	quad.points[2].x = quad.points[1].x + _transform[1][0] * _size[1];
	quad.points[2].y = quad.points[1].y + _transform[1][1] * _size[1];

	quad.points[3].x = _transform[0][0] * (_offset[0]) + _transform[1][0] * (_offset[1] + _size[1]) + _transform[2][0];
	quad.points[3].y = _transform[0][1] * (_offset[0]) + _transform[1][1] * (_offset[1] + _size[1]) + _transform[2][1];

	quad.points[0].x = CONVERT_MENGE_TO_OGRE_X(quad.points[0].x,width);
   	quad.points[0].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[0].y,heigth);

	quad.points[1].x = CONVERT_MENGE_TO_OGRE_X(quad.points[1].x,width);
   	quad.points[1].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[1].y,heigth);

	quad.points[2].x = CONVERT_MENGE_TO_OGRE_X(quad.points[2].x,width);
   	quad.points[2].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[2].y,heigth);

	quad.points[3].x = CONVERT_MENGE_TO_OGRE_X(quad.points[3].x,width);
   	quad.points[3].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[3].y,heigth);

	quad.z = z - 1;

	quad.uv = _uv;

	quad.texture = image->m_texture;

	quad.color = _color;
	quad.source = _src;
	quad.dest = _dst;

	quadList.push_back(quad);
}

void OgreRenderSpriteManager::addQuad2( Ogre::Viewport* _viewport,
									   const Ogre::Vector2 & _contentRes,
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
									   Ogre::SceneBlendFactor _dst)
{
	isSorted = false;
	QuadInfo quad;
	
	float width = _contentRes.x;
  	float heigth = _contentRes.y;

	//FIXME:

	quad.viewport = _viewport;

	quad.points[0].x = _transform[0][0] * _a[0] + _transform[1][0] * _a[1] + _transform[2][0];
	quad.points[0].y = _transform[0][1] * _a[0] + _transform[1][1] * _a[1] + _transform[2][1];

	quad.points[1].x = _transform[0][0] * _b[0] + _transform[1][0] * _b[1] + _transform[2][0];
	quad.points[1].y = _transform[0][1] * _b[0] + _transform[1][1] * _b[1] + _transform[2][1];

	quad.points[2].x = _transform[0][0] * _c[0] + _transform[1][0] * _c[1] + _transform[2][0];
	quad.points[2].y = _transform[0][1] * _c[0] + _transform[1][1] * _c[1] + _transform[2][1];

	quad.points[3].x = _transform[0][0] * _d[0] + _transform[1][0] * _d[1] + _transform[2][0];
	quad.points[3].y = _transform[0][1] * _d[0] + _transform[1][1] * _d[1] + _transform[2][1];

	quad.points[0].x = CONVERT_MENGE_TO_OGRE_X(quad.points[0].x,width);
   	quad.points[0].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[0].y,heigth);

	quad.points[1].x = CONVERT_MENGE_TO_OGRE_X(quad.points[1].x,width);
   	quad.points[1].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[1].y,heigth);

	quad.points[2].x = CONVERT_MENGE_TO_OGRE_X(quad.points[2].x,width);
   	quad.points[2].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[2].y,heigth);

	quad.points[3].x = CONVERT_MENGE_TO_OGRE_X(quad.points[3].x,width);
   	quad.points[3].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[3].y,heigth);

	quad.z = z - 1;

	quad.uv = _uv;

	quad.texture = image->m_texture;

	quad.color = _color;
	
	quad.source = _src;
	quad.dest = _dst;

	quadList.push_back(quad);
}

//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::doRender(void)
{
	if( !isSorted )
	{
		std::sort( quadList.begin(), quadList.end() );
		isSorted = true;

		#ifdef VBVB
			size_t size = vertexBuffer->getNumVertices();
			size_t requestedSize = quadList.size() * VERTEX_PER_QUAD;

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
		#endif

		QuadVertex * buffmem = static_cast<QuadVertex*>( vertexBuffer->lock( Ogre::HardwareVertexBuffer::HBL_DISCARD ) );
		
		for ( QuadList::iterator it = quadList.begin(); it != quadList.end(); ++it )
		{
			const QuadInfo& quad = (*it);
		
			buffmem->point.x = quad.points[0].x;
			buffmem->point.y = quad.points[0].y;
			buffmem->point.z = quad.z;

			buffmem->texcoord.x = quad.uv.x;
			buffmem->texcoord.y = quad.uv.y;

			buffmem->diffuse = quad.color;
			++buffmem;
			
			buffmem->point.x = quad.points[1].x;
			buffmem->point.y = quad.points[1].y;
			buffmem->point.z = quad.z;

			buffmem->texcoord.x = quad.uv.z;
			buffmem->texcoord.y = quad.uv.y;

			buffmem->diffuse = quad.color;
			++buffmem;

			buffmem->point.x = quad.points[2].x;
			buffmem->point.y = quad.points[2].y;
			buffmem->point.z = quad.z;

			buffmem->texcoord.x = quad.uv.z;
			buffmem->texcoord.y = quad.uv.w;

			buffmem->diffuse = quad.color;
			++buffmem;

			buffmem->point.x = quad.points[0].x;
			buffmem->point.y = quad.points[0].y;
			buffmem->point.z = quad.z;

			buffmem->texcoord.x = quad.uv.x;
			buffmem->texcoord.y = quad.uv.y;

			buffmem->diffuse = quad.color;
			++buffmem;

			buffmem->point.x = quad.points[2].x;
			buffmem->point.y = quad.points[2].y;
			buffmem->point.z = quad.z;

			buffmem->texcoord.x = quad.uv.z;
			buffmem->texcoord.y = quad.uv.w;

			buffmem->diffuse = quad.color;
			++buffmem;

			buffmem->point.x = quad.points[3].x;
			buffmem->point.y = quad.points[3].y;
			buffmem->point.z = quad.z;
			
			buffmem->texcoord.x = quad.uv.x;
			buffmem->texcoord.y = quad.uv.w;

			buffmem->diffuse = quad.color;
			++buffmem;
		}

		vertexBuffer->unlock();
	}

	size_t bufferPos = 0;
	bool first = true;
	bool changeViewport = false;
	//Ogre::Viewport* main = m_renderSys->_getViewport();
	//bool debug = false;

	QuadList::iterator it = quadList.begin();

	while( it != quadList.end() )
	{
		currTexture = it->texture;
		currViewport = it->viewport;
		//currViewport = m_renderSys->_getViewport();
		renderOp.vertexData->vertexStart = bufferPos;
		Ogre::SceneBlendFactor src = it->source;
		Ogre::SceneBlendFactor dst = it->dest;

		for ( ;it != quadList.end(); ++it )
		{
			const QuadInfo& quad = (*it);

			if( currViewport != quad.viewport )
			{
				changeViewport = true;
				//currViewport = quad.viewport;
			}

			if ( (currTexture != quad.texture) || changeViewport )
			{
				break;
			}

			bufferPos += VERTEX_PER_QUAD;
		}

		renderOp.vertexData->vertexCount = bufferPos - renderOp.vertexData->vertexStart;
		
		//if( debug )
		//	m_renderSys->_setViewport( main );
		//else
			m_renderSys->_setViewport( currViewport );
		//debug = !debug;
		changeViewport = false;

		m_renderSys->_setSceneBlending( src, dst );

		m_renderSys->_setTexture( 0, true, currTexture );

		if ( first )
		{
			prepareForRender();
			first = false;
		}		

		m_renderSys->_render( renderOp );
	}

	#ifdef VBVB

		if( bufferPos < vertexBuffer->getNumVertices()/2 )
		{
		   underusedFramecount++;
		}
		else
		{
		   underusedFramecount = 0;
		} 
	
	#endif
}