#	include "Layer2DAccumulator.h"

#	include "ObjectImplement.h"
#	include "VisitorAdapter.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"
#	include "Scene.h"
#	include "Player.h"
#	include "Camera2D.h"

#	include "Math/box2.h"

#	include <sstream>

#	define DEFAULT_GRID_SIZE 512.f

#	include "Texture.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Layer2DAccumulator );
	//////////////////////////////////////////////////////////////////////////
	Layer2DAccumulator::Layer2DAccumulator()
		: m_gridSize( DEFAULT_GRID_SIZE )
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DAccumulator::loader( XmlElement * _xml )
	{
		Layer2D::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE( "GridSize", "Value", m_gridSize );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	class VisitorRenderLayer2DPool
		: public VisitorAdapter<VisitorRenderLayer2DPool>
	{
	public:
		VisitorRenderLayer2DPool( const Layer2DAccumulator::TRenderImageVector& _images )
			: m_surfaces( _images )
		{
		}
	public:
		void visit( Node* _node )
		{
			if( _node->isRenderable() == false )
			{
				return;
			}
			RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
			const mt::box2f & nbbox = _node->getBoundingBox();
			for( Layer2DAccumulator::TRenderImageVector::iterator it = m_surfaces.begin(), it_end = m_surfaces.end();
				it != it_end;
				it++ )
			{
				if( mt::is_intersect( nbbox, it->rect ) )
				{
					renderEngine->setRenderTarget( it->image->getName(), false );
					renderEngine->setActiveCamera( it->camera );
					//renderEngine->setViewMatrix( camera->getViewMatrix() );
					_node->_render( 0 );
					_node->visitChildren( this );
				}
			}
		}
	protected:
		Layer2DAccumulator::TRenderImageVector m_surfaces;
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer2DAccumulator::render( unsigned int _debugMask )
	{
		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		VisitorRenderLayer2DPool visitorRender( m_surfaces );

		visitChildren( &visitorRender );

		const String& renderTarget = m_scene->getRenderTarget();
		Holder<RenderEngine>::hostage()
			->setRenderTarget( renderTarget );

		Holder<RenderEngine>::hostage()
			->setActiveCamera( m_camera2D );

		Layer::_render( _debugMask );
		_render( _debugMask );

		m_children.clear();

		Holder<RenderEngine>::hostage()
			->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2DAccumulator::_compile()
	{
		if( Layer2D::_compile() == false )
		{
			return false;
		}
		std::size_t countX = (std::size_t)::ceilf( m_size.x / m_gridSize );
		std::size_t countY = (std::size_t)::ceilf( m_size.y / m_gridSize );

		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();

		String layer2DAccumulatorName = "Layer2DAccumulator_" + m_name + "_image_";

		for( std::size_t i = 0; i < countX; i++ )
		{
			for( std::size_t j = 0; j < countY; j++ )
			{
				Stringstream accumulatorStreamName;
				accumulatorStreamName << layer2DAccumulatorName;
				accumulatorStreamName << i;
				accumulatorStreamName << j;

				String name = accumulatorStreamName.str();

				mt::vec2f renderTargetResolution( m_gridSize, m_gridSize );
				
				Texture* image = Holder<RenderEngine>::hostage()->createRenderTargetTexture( name, renderTargetResolution );

				ImageRect imageRect;
				imageRect.image = image;
				imageRect.rect = mt::box2f( mt::vec2f( float(i) * m_gridSize, float(j) * m_gridSize ), mt::vec2f( float(i+1) * m_gridSize, float(j+1) * m_gridSize ) );
				imageRect.camera = new Camera2D();
				const Viewport & vp = imageRect.camera->getViewport();
				mt::vec2f vp_size = vp.end - vp.begin;
				imageRect.camera->setLocalPosition( imageRect.rect.minimum + vp_size * 0.5f );
				//imageRect.camera->setRenderTarget( name );

				m_surfaces.push_back( imageRect );

				RenderObject* ro = renderEngine->createRenderObject();
				ro->vertices.reserve( 4 );
				ro->vertices[0].pos[0] = imageRect.rect.minimum.x;
				ro->vertices[0].pos[1] = imageRect.rect.minimum.y;
				ro->vertices[1].pos[0] = imageRect.rect.minimum.x + m_gridSize;
				ro->vertices[1].pos[1] = imageRect.rect.minimum.y;
				ro->vertices[1].uv[0] = 1.0f;
				ro->vertices[2].pos[0] = imageRect.rect.minimum.x + m_gridSize;
				ro->vertices[2].pos[1] = imageRect.rect.minimum.y + m_gridSize;
				ro->vertices[2].uv[0] = 1.0f;
				ro->vertices[2].uv[1] = 1.0f;
				ro->vertices[3].pos[0] = imageRect.rect.minimum.x;
				ro->vertices[3].pos[1] = imageRect.rect.minimum.y + m_gridSize;
				ro->vertices[3].uv[1] = 1.0f;
				ro->material.textureStages = 1;
				ro->material.textureStage[0].texture = image;

				m_ros.push_back( ro );
				// clear target
				//renderEngine->setRenderTarget( name, true );

			}
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DAccumulator::_release()
	{
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();

		for( std::vector<RenderObject*>::iterator it = m_ros.begin(), it_end = m_ros.end();
			it != it_end;
			++it )
		{
			renderEngine->releaseRenderObject( (*it) );
		}
		m_ros.clear();

		for( TRenderImageVector::iterator it = m_surfaces.begin(), it_end = m_surfaces.end();
			it != it_end;
			it++ )
		{
			renderEngine->releaseTexture( it->image );
			delete it->camera;
		}
		m_surfaces.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DAccumulator::_render( unsigned int _debugMask )
	{
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
	
		for( std::vector<RenderObject*>::iterator it = m_ros.begin(), it_end = m_ros.end();
			it != it_end;
			++it )
		{
			renderEngine->renderObject( (*it) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
