//#	include "Layer2DAccumulator.h"
//
//#	include "VisitorAdapter.h"
//
//#	include "BinParser.h"
//#	include "RenderEngine.h"
//#	include "Scene.h"
//#	include "Player.h"
//#	include "Camera2D.h"
//
//#	include "Consts.h"
//
//#	include "Math/box2.h"
//
//#	include <sstream>
//
//#	define DEFAULT_GRID_SIZE 512.f
//
//#	include "Texture.h"
//
//#	include "NodeManager.h"
//
//namespace Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	class Layer2DAccumulator::VisitorRenderLayer2DPool
//		: public VisitorAdapter<VisitorRenderLayer2DPool>
//	{
//	public:
//		VisitorRenderLayer2DPool( const Layer2DAccumulator::TVectorRenderImage& _images )
//			: m_surfaces( _images )
//		{
//		}
//
//	public:
//		void visit( Node* _node )
//		{
//			if( _node->isRenderable() == false )
//			{
//				return;
//			}
//
//			const mt::box2f & nbbox = _node->getBoundingBox();
//
//			for( Layer2DAccumulator::TVectorRenderImage::iterator 
//				it = m_surfaces.begin(), 
//				it_end = m_surfaces.end();
//			it != it_end;
//			++it )
//			{
//				if( mt::is_intersect( nbbox, it->rect ) )
//				{
//					RenderEngine::get()
//						->setRenderTarget( it->image->getName(), false );
//
//					RenderEngine::get()
//						->setActiveCamera( it->camera );
//
//					//renderEngine->setViewMatrix( camera->getViewMatrix() );
//					_node->_render( it->camera );
//					_node->visitChildren( this );
//				}
//			}
//		}
//	protected:
//		Layer2DAccumulator::TVectorRenderImage m_surfaces;
//	};
//	//////////////////////////////////////////////////////////////////////////
//	Layer2DAccumulator::Layer2DAccumulator()
//		: m_gridSize( DEFAULT_GRID_SIZE )
//	{
//
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Layer2DAccumulator::loader( BinParser * _parser )
//	{
//		Layer2D::loader( _parser );
//
//		BIN_SWITCH_ID( _parser )
//		{
//			BIN_CASE_ATTRIBUTE( Protocol::GridSize_Value, m_gridSize );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Layer2DAccumulator::render( Camera2D * _camera )
//	{
//		RenderEngine::get()
//			->beginLayer2D();
//
//		VisitorRenderLayer2DPool visitorRender( m_surfaces );
//
//		visitChildren( &visitorRender );
//
//		const ConstString& renderTarget = m_scene->getRenderTarget();
//
//		RenderEngine::get()
//			->setRenderTarget( renderTarget );
//
//		//RenderEngine::get()
//		//	->setActiveCamera( _camera2D );
//
//		//this->_render( _camera );
//
//		m_child.clear();
//
//		RenderEngine::get()
//			->endLayer2D();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool Layer2DAccumulator::_compile()
//	{
//		if( Layer2D::_compile() == false )
//		{
//			return false;
//		}
//		std::size_t countX = (std::size_t)::ceilf( m_size.x / m_gridSize );
//		std::size_t countY = (std::size_t)::ceilf( m_size.y / m_gridSize );
//
//		RenderEngine* renderEngine = RenderEngine::get();
//
//		const ConstString & name = this->getName();
//		String layer2DAccumulatorName = "Layer2DAccumulator_" + Helper::to_str(name) + "_image_";
//
//		m_materials.reserve( countX * countY );
//		m_vertices.resize( countX * countY * 4 );
//		for( std::size_t i = 0; i < countX; i++ )
//		{
//			for( std::size_t j = 0; j < countY; j++ )
//			{
//				Stringstream accumulatorStreamName;
//				accumulatorStreamName << layer2DAccumulatorName;
//				accumulatorStreamName << i;
//				accumulatorStreamName << j;
//
//				String accumulatorName = accumulatorStreamName.str();
//				ConstString c_accumulatorName(accumulatorName);
//
//				mt::vec2f renderTargetSize( m_gridSize, m_gridSize );
//
//				Texture* image = RenderEngine::get()
//					->createRenderTargetTexture( c_accumulatorName, renderTargetSize );
//
//				ImageRect imageRect;
//				imageRect.image = image;
//				imageRect.rect = mt::box2f( mt::vec2f( float(i) * m_gridSize, float(j) * m_gridSize ), mt::vec2f( float(i+1) * m_gridSize, float(j+1) * m_gridSize ) );
//
//				ConstString c_imageCamera("imageCamera");
//				imageRect.camera = NodeManager::get()
//					->createNodeT<Camera2D>( c_imageCamera, Consts::get()->c_Camera2D, Consts::get()->c_builtin_empty );
//
//				imageRect.camera->setViewportSize( renderTargetSize );
//
//				const Viewport & vp = imageRect.camera->getViewport();
//				mt::vec2f vp_size = vp.end - vp.begin;
//				imageRect.camera->setLocalPosition( imageRect.rect.minimum + vp_size * 0.5f );
//				//imageRect.camera->setRenderTarget( name );
//
//				m_surfaces.push_back( imageRect );
//
//				const MaterialGroup * mg_accumulator = renderEngine->getMaterialGroup( CONST_STRING(Accumulator) );
//				
//				const Material * material = mg_accumulator->getMaterial( TAM_CLAMP, TAM_CLAMP );
//
//				m_materials.push_back( material );
//
//				Vertex2D* vertices = &(m_vertices[(i*countY + j)*4]);
//				vertices[0].pos[0] = imageRect.rect.minimum.x;
//				vertices[0].pos[1] = imageRect.rect.minimum.y;
//				vertices[0].uv[0] = 0.0f;
//				vertices[0].uv[1] = 0.0f;
//				vertices[0].color = 0xFFFFFFFF;
//
//				vertices[1].pos[0] = imageRect.rect.minimum.x + m_gridSize;
//				vertices[1].pos[1] = imageRect.rect.minimum.y;
//				vertices[1].uv[0] = 1.0f;
//				vertices[1].uv[1] = 0.0f;
//				vertices[1].color = 0xFFFFFFFF;
//
//				vertices[2].pos[0] = imageRect.rect.minimum.x + m_gridSize;
//				vertices[2].pos[1] = imageRect.rect.minimum.y + m_gridSize;
//				vertices[2].uv[0] = 1.0f;
//				vertices[2].uv[1] = 1.0f;
//				vertices[2].color = 0xFFFFFFFF;
//
//				vertices[3].pos[0] = imageRect.rect.minimum.x;
//				vertices[3].pos[1] = imageRect.rect.minimum.y + m_gridSize;
//				vertices[3].uv[0] = 0.0f;
//				vertices[3].uv[1] = 1.0f;
//				vertices[3].color = 0xFFFFFFFF;
//
//			}
//		}
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Layer2DAccumulator::_release()
//	{
//		RenderEngine* renderEngine = RenderEngine::get();
//
//		m_vertices.clear();
//
//		m_materials.clear();
//
//		for( TVectorRenderImage::iterator 
//			it = m_surfaces.begin(), 
//			it_end = m_surfaces.end();
//		it != it_end;
//		it++ )
//		{
//			renderEngine->releaseTexture( it->image );
//			it->camera->destroy();
//		}
//
//		m_surfaces.clear();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Layer2DAccumulator::_render( Camera2D * _camera )
//	{
//		Layer::_render( _camera );
//
//		RenderEngine* renderEngine = RenderEngine::get();
//		size_t count = 0;
//
//		for( TVectorMaterial::iterator 
//			it = m_materials.begin(), 
//			it_end = m_materials.end();
//		it != it_end;
//		++it, ++count )
//		{
//			renderEngine->renderObject2D( (*it), &(m_surfaces[count].image),NULL, 1, &(m_vertices[count*4]), 4, LPT_QUAD, false );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//}	// namespace Menge
