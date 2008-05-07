#	include "OgreRenderSpriteManager.h"
#	include "OgreRenderImage.h"

const unsigned int VERTEX_PER_QUAD			= 6;
const unsigned int VERTEXBUFFER_INITIAL_CAPACITY	= 70000;
const unsigned int UNDERUSED_FRAME_THRESHOLD = 70000;
const unsigned int VECTOR_CAPACITY = 40000;

//////////////////////////////////////////////////////////////////////////
OgreRenderSpriteManager::OgreRenderSpriteManager()
: currentZ(1.0f)
, isSorted(false)
, m_hTexelOffset( 0.0f )
, m_vTexelOffset( 0.0f )
, m_textureFiltering( false )
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
    Ogre::HardwareVertexBufferSharedPtr &vertexBuffer, unsigned int nquads)
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
//	OgreRenderSpriteManager::targetQueue = targetQueue;

	m_renderSys = renderSys;
	m_viewport = viewport;

	sceneMan->addRenderQueueListener(this);

	createQuadRenderOp(renderOp, vertexBuffer, VERTEXBUFFER_INITIAL_CAPACITY);
    underusedFramecount = 0;

	m_renderSys = Ogre::Root::getSingleton().getRenderSystem();

	m_hTexelOffset = m_renderSys->getHorizontalTexelOffset();
	m_vTexelOffset = m_renderSys->getVerticalTexelOffset();
	//m_quadMap[ Ogre::RENDER_QUEUE_MAIN ].reserve( VECTOR_CAPACITY );
	//m_quadMap[ Ogre::RENDER_QUEUE_OVERLAY ].reserve( VECTOR_CAPACITY );
	//quadList.reserve( VECTOR_CAPACITY );
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
	//if (!afterQueue && queueGroupId == targetQueue)
	{
	}
	if( queueGroupId != Ogre::RENDER_QUEUE_OVERLAY ) return;

	currentZ = 1.0f;
	TQuadMap::iterator it = m_quadMap.find( currViewport );
	if( it != m_quadMap.end() && !it->second.empty() )
	{
		prepareForRender();
		doRender( it->second );
		isSorted = true;
		it->second.clear();
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::renderQueueEnded(
									 Ogre::uint8 queueGroupId, const Ogre::String &invocation, bool &repeatThisInvocation)
{
	/*TQuadMap::iterator it = m_quadMap.find( queueGroupId );
	//if (afterQueue && queueGroupId == targetQueue)
	if( it != m_quadMap.end() && !it->second.empty() )
	{
		prepareForRender();
		doRender( it->second );
		isSorted = true;
		it->second.clear();
	}*/
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

	//d_uvwAddressMode.u = Ogre::TextureUnitState::TAM_CLAMP;
	//d_uvwAddressMode.v = Ogre::TextureUnitState::TAM_CLAMP;
	//d_uvwAddressMode.w = Ogre::TextureUnitState::TAM_CLAMP;

	//d_uvwAddressMode.u = Ogre::TextureUnitState::TAM_BORDER;
	//d_uvwAddressMode.v = Ogre::TextureUnitState::TAM_BORDER;
	//d_uvwAddressMode.w = Ogre::TextureUnitState::TAM_BORDER;

	d_uvwAddressMode.u = Ogre::TextureUnitState::TAM_WRAP;
	d_uvwAddressMode.v = Ogre::TextureUnitState::TAM_WRAP;
	d_uvwAddressMode.w = Ogre::TextureUnitState::TAM_WRAP;

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
	m_renderSys->_setTextureCoordSet(0.f, 0.f);
	//m_renderSys->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_POINT, Ogre::FO_POINT );
	//m_renderSys->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_LINEAR );
	if( m_textureFiltering )
	{
		m_renderSys->_setTextureUnitFiltering(0, Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT );
	}
	else
	{
		m_renderSys->_setTextureUnitFiltering(0, Ogre::FO_POINT, Ogre::FO_POINT, Ogre::FO_POINT );
	}
	m_renderSys->_setTextureAddressingMode(0, d_uvwAddressMode);
	m_renderSys->_setTextureMatrix(0, Ogre::Matrix4::IDENTITY);
	m_renderSys->_setAlphaRejectSettings(Ogre::CMPF_ALWAYS_PASS, 0);
	m_renderSys->_setTextureBlendMode(0, d_colourBlendMode);
	m_renderSys->_setTextureBlendMode(0, d_alphaBlendMode);
	m_renderSys->_disableTextureUnitsFrom(1);
	//static const Ogre::TexturePtr s_nullTexture;
	//m_renderSys->_setVertexTexture( 0, s_nullTexture );

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
									   Ogre::SceneBlendFactor _dst,
									   Ogre::uint8 _renderQueue /* = Ogre::RENDER_QUEUE_OVERLAY */ )

{
	if( m_quadMap.find(_viewport) == m_quadMap.end() )
	{
		m_quadMap[_viewport].reserve( VECTOR_CAPACITY );
	}

	if( m_quadMap[ _viewport ].capacity() == m_quadMap[ _viewport ].size() ) return;

	isSorted = false;
	QuadInfo quad;
	
	float width = _contentRes.x;
  	float height = _contentRes.y;
	//float width = _viewport->getActualWidth();
	//float height = _viewport->getActualHeight();

	//FIXME:
	float vOffset = 0.0f;
	float hOffset = 0.0f;

	/*if( _viewport->getCamera()->getName() == "defaultCamera" )
	{
		vOffset = m_vTexelOffset / ( 0.5f * _viewport->getActualWidth() );
		hOffset = m_hTexelOffset / ( 0.5f * _viewport->getActualHeight() );
	}*/

	quad.points[0].x = _transform[0][0] * _offset[0] + _transform[1][0] * _offset[1] + _transform[2][0];
	quad.points[0].y = _transform[0][1] * _offset[0] + _transform[1][1] * _offset[1] + _transform[2][1];

	quad.points[1].x = quad.points[0].x + _transform[0][0] * _size[0];
	quad.points[1].y = quad.points[0].y + _transform[0][1] * _size[0];

	quad.points[2].x = quad.points[1].x + _transform[1][0] * _size[1];
	quad.points[2].y = quad.points[1].y + _transform[1][1] * _size[1];

	quad.points[3].x = _transform[0][0] * (_offset[0]) + _transform[1][0] * (_offset[1] + _size[1]) + _transform[2][0];
	quad.points[3].y = _transform[0][1] * (_offset[0]) + _transform[1][1] * (_offset[1] + _size[1]) + _transform[2][1];

	quad.points[0].x = CONVERT_MENGE_TO_OGRE_X(quad.points[0].x,width) + hOffset;
   	quad.points[0].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[0].y,height) - vOffset;

	quad.points[1].x = CONVERT_MENGE_TO_OGRE_X(quad.points[1].x,width) + hOffset;
   	quad.points[1].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[1].y,height) - vOffset;

	quad.points[2].x = CONVERT_MENGE_TO_OGRE_X(quad.points[2].x,width) + hOffset;
   	quad.points[2].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[2].y,height) - vOffset;

	quad.points[3].x = CONVERT_MENGE_TO_OGRE_X(quad.points[3].x,width) + hOffset;
   	quad.points[3].y = CONVERT_MENGE_TO_OGRE_Y(quad.points[3].y,height) - vOffset;

	quad.z = z - 1;

	quad.uv = _uv;

	quad.texture = image->m_texture;

	quad.color = _color;
	quad.source = _src;
	quad.dest = _dst;

	//quadList.push_back(quad);
	m_quadMap[ _viewport ].push_back( quad );
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
									   Ogre::SceneBlendFactor _dst,
									   Ogre::uint8 _renderQueue /* = Ogre::RENDER_QUEUE_OVERLAY */)
{
	if( m_quadMap.find(_viewport) == m_quadMap.end() )
	{
		m_quadMap[_viewport].reserve( VECTOR_CAPACITY );
	}

	if( m_quadMap[ _viewport ].capacity() == m_quadMap[ _viewport ].size() ) return;

	isSorted = false;
	QuadInfo quad;
	
	float width = _contentRes.x;
  	float heigth = _contentRes.y;

	//FIXME:

	quad.points[0].x = _transform[0][0] * _a[0] + _transform[1][0] * _a[1] + _transform[2][0] + m_hTexelOffset;
	quad.points[0].y = _transform[0][1] * _a[0] + _transform[1][1] * _a[1] + _transform[2][1] + m_vTexelOffset;

	quad.points[1].x = _transform[0][0] * _b[0] + _transform[1][0] * _b[1] + _transform[2][0] + m_hTexelOffset;
	quad.points[1].y = _transform[0][1] * _b[0] + _transform[1][1] * _b[1] + _transform[2][1] + m_vTexelOffset;

	quad.points[2].x = _transform[0][0] * _c[0] + _transform[1][0] * _c[1] + _transform[2][0] + m_hTexelOffset;
	quad.points[2].y = _transform[0][1] * _c[0] + _transform[1][1] * _c[1] + _transform[2][1] + m_vTexelOffset;

	quad.points[3].x = _transform[0][0] * _d[0] + _transform[1][0] * _d[1] + _transform[2][0] + m_hTexelOffset;
	quad.points[3].y = _transform[0][1] * _d[0] + _transform[1][1] * _d[1] + _transform[2][1] + m_vTexelOffset;

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

	//quadList.push_back(quad);
	m_quadMap[ _viewport ].push_back( quad );
}

//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::doRender( TQuadList& _quadList )
{

	//if( !isSorted )
	{
		std::sort( _quadList.begin(), _quadList.end() );
		//isSorted = true;

		#ifdef VBVB
			size_t size = vertexBuffer->getNumVertices();
			TQuadList::size_type requestedSize = quadList.size() * VERTEX_PER_QUAD;

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
		//QuadVertex * buffmem = static_cast<QuadVertex*>( vertexBuffer->lock( Ogre::HardwareVertexBuffer::HBL_NORMAL ) );
		
		for ( TQuadList::iterator 
			it = _quadList.begin(),
			it_end = _quadList.end(); 
		it != it_end; 
		++it )
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
	bool changeViewport = true;

	TQuadList::iterator it = _quadList.begin();
	TQuadList::iterator it_end = _quadList.end();

	while( it != it_end )
	{
		currTexture = it->texture;
		//currViewport = m_renderSys->_getViewport();
		renderOp.vertexData->vertexStart = bufferPos;
		Ogre::SceneBlendFactor src = it->source;
		Ogre::SceneBlendFactor dst = it->dest;

		for ( ;it != it_end; ++it )
		{
			const QuadInfo& quad = (*it);


			if ( currTexture != quad.texture )
			{
				break;
			}

			bufferPos += VERTEX_PER_QUAD;
		}
		//m_renderSys->_beginFrame();
		renderOp.vertexData->vertexCount = bufferPos - renderOp.vertexData->vertexStart;
		
		//m_renderSys->_setViewport( currCamera->getViewport() );

		m_renderSys->_setCullingMode(Ogre::CULL_NONE);

		//m_renderSys->_setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);


		m_renderSys->_setSceneBlending( src, dst );

		m_renderSys->_setTexture( 0, true, currTexture );

		changeViewport = false;

		m_renderSys->_render( renderOp );
		//m_renderSys->_endFrame();
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

//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::preViewportUpdate( const Ogre::RenderTargetViewportEvent& evt )
{
	currViewport = evt.source;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSpriteManager::setTextureFiltering( bool _filter )
{
	m_textureFiltering = _filter;
}
//////////////////////////////////////////////////////////////////////////