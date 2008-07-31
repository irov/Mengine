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

#	define DEFAULT_GRID_SIZE 512

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
			XML_CASE_ATTRIBUTE_NODE("GridSize", "Value", m_gridSize );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	class VisitorRenderLayer2DPool
		: public VisitorAdapter<VisitorRenderLayer2DPool>
	{
	public:
		VisitorRenderLayer2DPool( const Viewport & _viewport, const Layer2DAccumulator::TRenderImageVector& _images )
			: m_viewport( _viewport )
			, m_surfaces( _images )
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
			mt::box2f nbbox = _node->getLocalBoundingBox();
			const mt::mat3f& wm = _node->getWorldMatrix();
			mt::box2f bbox;
			mt::set_box_from_oriented_extent( bbox, mt::vec2f( 0.0f, 0.0f ), nbbox.max - nbbox.min, wm );
			Viewport vp;
			for( Layer2DAccumulator::TRenderImageVector::iterator it = m_surfaces.begin(), it_end = m_surfaces.end();
				it != it_end;
				it++ )
			{
				if( mt::is_intersect( bbox, it->rect ) )
				{
					Viewport vp = Holder<Player>::hostage()->getRenderCamera2D()->getViewport();
					mt::vec2f vp_size = vp.end - vp.begin;

					renderEngine->setRenderTarget( it->image->getDescription(), false );
					Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( it->rect.min + vp_size * 0.5f );
					_node->_render( vp, false );
					_node->visitChildren( this );
				}
			}
		}
	protected:
		Viewport m_viewport;
		Layer2DAccumulator::TRenderImageVector m_surfaces;
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer2DAccumulator::render( const Viewport & _viewport, bool _enableDebug )
	{

		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		mt::vec2f camPos = 
			Holder<Player>::hostage()->getRenderCamera2D()->getLocalPosition();

		Viewport viewport = Holder<Player>::hostage()->getRenderCamera2D()->getViewport();
		
		VisitorRenderLayer2DPool visitorRender( _viewport, m_surfaces );

		visitChildren( &visitorRender );

		const String& renderTarget = m_scene->getRenderTarget();
		Holder<RenderEngine>::hostage()
			->setRenderTarget( renderTarget );

		mt::vec2f viewport_size = viewport.end - viewport.begin;
		viewport.begin.x *= m_factorParallax.x;
		viewport.begin.y *= m_factorParallax.y;

		mt::vec2f plxCamPos = viewport.begin + viewport_size * 0.5f;

		Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( plxCamPos );

		Layer::_render( viewport, _enableDebug );
		_render( viewport, _enableDebug );

		m_children.clear();

		Holder<RenderEngine>::hostage()
			->endLayer2D();

		Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( camPos );

	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2DAccumulator::_compile()
	{
		int countX = ::ceilf( m_size.x / m_gridSize );
		int countY = ::ceilf( m_size.y / m_gridSize );
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();

		for( int i = 0; i < countX; i++ )
		{
			for( int j = 0; j < countY; j++ )
			{
				String name = String( "Layer2DAccumulator_" + m_name + "_image_" + toString( i ) + toString( j ) );
				RenderImageInterface* image = Holder<RenderEngine>::hostage()->createRenderTargetImage( name, m_gridSize, m_gridSize );

				ImageRect imageRect;
				imageRect.image = image;
				imageRect.rect = mt::box2f( mt::vec2f( i * m_gridSize, j * m_gridSize ), mt::vec2f( (i+1) * m_gridSize, (j+1) * m_gridSize ) );
				
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
	void Layer2DAccumulator::_render( const Viewport & _viewport, bool _enableDebug )
	{
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
		mt::vec2f a( 0.0f, 0.0f );
		mt::vec2f b( m_gridSize, 0.0f );
		mt::vec2f c( m_gridSize, m_gridSize );
		mt::vec2f d( 0.0f, m_gridSize );
		mt::mat3f wm;
		mt::ident_m3( wm );
		//int count = 0;
		for( TRenderImageVector::iterator it = m_surfaces.begin(), it_end = m_surfaces.end();
			it != it_end;
			it++ )
		{
			//if ( count == 1 ) break;
			wm.v2.v2 = it->rect.min;
			renderEngine->renderImage( wm, a, b, c, d, mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f ), 0xFFFFFFFF, it->image );
			//count++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge