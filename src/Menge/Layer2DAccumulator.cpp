#	include "Layer2DAccumulator.h"

#	include "ObjectImplement.h"
#	include "VisitorAdapter.h"

#	include "XmlEngine.h"
#	include "RenderEngine.h"
#	include "Scene.h"
#	include "Player.h"
#	include "Camera2D.h"

#	include "Math/box2.h"
#	include "Utils.h"

#	include <sstream>

#	define DEFAULT_GRID_SIZE 512.f

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
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("GridSize"), MENGE_TEXT("Value"), m_gridSize );
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
					const Viewport & vp = Holder<Player>::hostage()->getRenderCamera2D()->getViewport();
					mt::vec2f vp_size = vp.end - vp.begin;

					renderEngine->setRenderTarget( it->image->getDescription(), false );
					Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( it->rect.min + vp_size * 0.5f );
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

		mt::vec2f camPos = 
			Holder<Player>::hostage()->getRenderCamera2D()->getLocalPosition();

		Viewport viewport = Holder<Player>::hostage()->getRenderCamera2D()->getViewport();
		
		VisitorRenderLayer2DPool visitorRender( m_surfaces );

		visitChildren( &visitorRender );

		const String& renderTarget = m_scene->getRenderTarget();
		Holder<RenderEngine>::hostage()
			->setRenderTarget( renderTarget );

		mt::vec2f viewport_size = viewport.end - viewport.begin;
		viewport.begin.x *= m_factorParallax.x;
		viewport.begin.y *= m_factorParallax.y;

		mt::vec2f plxCamPos = viewport.begin + viewport_size * 0.5f;

		Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( plxCamPos );

		Layer::_render( _debugMask );
		_render( _debugMask );

		m_children.clear();

		Holder<RenderEngine>::hostage()
			->endLayer2D();

		Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( camPos );

	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2DAccumulator::_compile()
	{
		std::size_t countX = (std::size_t)::ceilf( m_size.x / m_gridSize );
		std::size_t countY = (std::size_t)::ceilf( m_size.y / m_gridSize );

		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();

		String layer2DAccumulatorName = MENGE_TEXT("Layer2DAccumulator_") + m_name + MENGE_TEXT("_image_");

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
				
				RenderImageInterface* image = Holder<RenderEngine>::hostage()->createRenderTargetImage( name, renderTargetResolution );

				ImageRect imageRect;
				imageRect.image = image;
				imageRect.rect = mt::box2f( mt::vec2f( float(i) * m_gridSize, float(j) * m_gridSize ), mt::vec2f( float(i+1) * m_gridSize, float(j+1) * m_gridSize ) );
				
				m_surfaces.push_back( imageRect );

				// clear target
				renderEngine->setRenderTarget( name, true );

			}
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DAccumulator::_release()
	{
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
		for( TRenderImageVector::iterator it = m_surfaces.begin(), it_end = m_surfaces.end();
			it != it_end;
			it++ )
		{
			renderEngine->releaseImage( it->image );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DAccumulator::_render( unsigned int _debugMask )
	{
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
	
		//int count = 0;
		for( TRenderImageVector::iterator it = m_surfaces.begin(), it_end = m_surfaces.end();
			it != it_end;
			it++ )
		{
			//if ( count == 1 ) break;
			mt::vec2f offset = it->rect.min; 

			mt::vec2f vertices[4] =
			{
				mt::vec2f( 0.0f, 0.0f ) + offset,
				mt::vec2f( m_gridSize, 0.0f ) + offset,
				mt::vec2f( m_gridSize, m_gridSize ) + offset,
				mt::vec2f( 0.0f, m_gridSize ) + offset
			};
			renderEngine->renderImage( vertices,
										mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f ),
										0xFFFFFFFF, it->image 
										);
			//count++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
