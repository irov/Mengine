#	include "Layer2DPool.h"

#	include "ObjectImplement.h"
#	include "VisitorAdapter.h"

#	include "RenderEngine.h"

#	include "Math/box2.h"
#	include "Utils.h"

static const unsigned int ImageSize = 512;

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Layer2DPool );
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class VisitorRenderLayer2DPool
		: public VisitorAdapter<VisitorRenderLayer2DPool>
	{
	public:
		VisitorRenderLayer2DPool( const Viewport & _viewport, const Layer2DPool::TRenderImageVector& _images )
			: m_viewport( _viewport )
			, m_surfaces( _images )
		{
		}
	public:
		void visit( Node* _node )
		{
			RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
			const mt::box2f& bbox = _node->getWorldBoundingBox();
			for( Layer2DPool::TRenderImageVector::iterator it = m_surfaces.begin(), it_end = m_surfaces.end();
				it != it_end;
				it++ )
			{
				if( mt::is_intersect( bbox, it->rect ) )
				{
					renderEngine->setRenderTarget( it->image->getDescription() );
					_node->translate( -it->rect.min );
					_node->_render( Viewport( it->rect.min, it->rect.max ), false );
					_node->translate( it->rect.min );
					mt::mat3f ident;
					mt::ident_m3( ident );
					_node->updateWorldMatrix( ident );
				}
			}
		}
	protected:
		Viewport m_viewport;
		Layer2DPool::TRenderImageVector m_surfaces;
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer2DPool::render( const Viewport & _viewport, bool _enableDebug )
	{

		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		mt::vec2f viewport_size = _viewport.end - _viewport.begin;

		Viewport viewport;

		viewport.begin = _viewport.begin;

		viewport.begin.x *= m_factorParallax.x;
		viewport.begin.y *= m_factorParallax.y;

		viewport.end = viewport.begin + viewport_size;

		Holder<RenderEngine>::hostage()
			->setRenderViewport( viewport );
		
		VisitorRenderLayer2DPool visitorRender( viewport, m_surfaces );

		visitChildren( &visitorRender );

		Layer::_render( viewport, _enableDebug );
		_render( viewport, _enableDebug );

		m_children.clear();

		Holder<RenderEngine>::hostage()
			->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2DPool::_compile()
	{
		int countX = m_size.x / ImageSize;
		int d = static_cast<int>( m_size.x ) % ImageSize;
		if( d != 0 )
		{
			countX += 1;
		}
		int countY = m_size.y / ImageSize;
		d = static_cast<int>( m_size.y ) % ImageSize;
		if( d != 0 )
		{
			countY += 1;
		}
		for( int i = 0; i < countX; i++ )
		{
			for( int j = 0; j < countY; j++ )
			{
				String name = String( "Layer2DLoop_" + m_name + "_image_" + toString( i ) + toString( j ) );
				RenderImageInterface* image = Holder<RenderEngine>::hostage()->createRenderTargetImage( name,ImageSize, ImageSize );

				ImageRect imageRect;
				imageRect.image = image;
				imageRect.rect = mt::box2f( mt::vec2f( i * ImageSize, j * ImageSize ), mt::vec2f( (i+1) * ImageSize, (j+1) * ImageSize ) );
				
				m_surfaces.push_back( imageRect );
			}
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DPool::_release()
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
	void Layer2DPool::_render( const Viewport & _viewport, bool _enableDebug )
	{
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
		mt::vec2f a( 0.0f, 0.0f );
		mt::vec2f b( ImageSize, 0.0f );
		mt::vec2f c( ImageSize, ImageSize );
		mt::vec2f d( 0.0f, ImageSize );
		mt::mat3f wm;
		mt::ident_m3( wm );
		/*for( TRenderImageVector::iterator it = m_surfaces.begin(), it_end = m_surfaces.end();
			it != it_end;
			it++ )
		{
			wm.v2.v2 = it->rect.min;
			renderEngine->renderImage( wm, a, b, c, d, mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f ), 0xFFFFFFFF, it->image );
		}*/
		for( int i = 0; i < 4; i++ )
		{
			wm.v2.v2 = m_surfaces[i].rect.min;
			renderEngine->renderImage( wm, a, b, c, d, mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f ), 0xFFFFFFFF, m_surfaces[i].image );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge