#	include "Layer2DLoop.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Player.h"

#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "LogEngine.h"
#	include "Game.h"


#	include "Sprite.h"
#	include "Animation.h"

#	include "VisitorAdapter.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Layer2DLoop);
	//////////////////////////////////////////////////////////////////////////
	Layer2DLoop::Layer2DLoop()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	class VisitorRenderLayer2DLoop
		: public VisitorAdapter<VisitorRenderLayer2DLoop>
	{
	public:
		VisitorRenderLayer2DLoop( const Viewport & _viewport, const mt::vec2f & _size, bool _enableDebug )
			: m_viewport(_viewport)
			, m_size(_size)
			, m_enableDebug( _enableDebug )
		{
		}

	public:
		void visit( Node * _node )
		{				
			if( _node->isRenderable() == true )
			{
				const mt::box2f & sprite_bbox = _node->getWorldBoundingBox();

				//if( m_viewport.end.x < sprite_bbox.vb.x ) 
				{
					if( sprite_bbox.ve.x < m_size.x )
					{
				//		return;
					}

					float segment_x = sprite_bbox.ve.x - m_size.x;

					if( segment_x < m_viewport.begin.x )
					{
				//		return;
					}

					//Viewport viewport = m_viewport;
					//viewport.begin += mt::vec2f( m_size.x, 0.f );
					//viewport.end += mt::vec2f( m_size.x, 0.f );
					_node->_render( m_viewport, m_enableDebug );
				}
				//else if( m_viewport.begin.x > sprite_bbox.ve.x ) 
				{
					if( m_viewport.end.x < m_size.x )
					{
				//		return;
					}

					float segment_x = m_viewport.end.x - m_size.x;

					if( segment_x < sprite_bbox.vb.x )
					{
				//		return;
					}

					/*Viewport viewport = m_viewport;
					viewport.begin += mt::vec2f( -m_size.x, 0.f );
					viewport.end += mt::vec2f( -m_size.x, 0.f );
					mt::vec2f vp_size = viewport.end - viewport.begin;
*/
					//Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( viewport.begin + vp_size * 0.5f );
					Camera2D* camera = Holder<Player>::hostage()->getRenderCamera2D();
					mt::vec2f oldPos = camera->getLocalPosition();
					camera->setLocalPosition( oldPos + mt::vec2f( -m_size.x, 0.0f ) );

					_node->_render( m_viewport, m_enableDebug );

					//Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( m_viewport.begin + vp_size * 0.5f );
					camera->setLocalPosition( oldPos );
				}

				_node->visitChildren( this );
			}

		}

		void visit( Layer * _layer )
		{
			_layer->render( m_viewport, m_enableDebug );
		}

	protected:
		Viewport m_viewport;
		bool m_enableDebug;
		mt::vec2f m_size;
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer2DLoop::render( const Viewport & _viewport, bool _enableDebug )
	{
		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		Camera2D* camera = Holder<Player>::hostage()->getRenderCamera2D();

		/*mt::vec2f camPos = 
			Holder<Player>::hostage()->getRenderCamera2D()->getLocalPosition();

		Viewport viewport = Holder<Player>::hostage()->getRenderCamera2D()->getViewport();
		mt::vec2f viewport_size = viewport.end - viewport.begin;
		viewport.begin.x *= m_factorParallax.x;
		viewport.begin.y *= m_factorParallax.y;

		float c = ::floorf( viewport.begin.x / m_size.x );
		viewport.begin.x -= m_size.x * c;
		viewport.end.x = viewport.begin.x + viewport_size.x;
		viewport.end.y = viewport.begin.y + viewport_size.y;

		mt::vec2f plxCamPos = viewport.begin + viewport_size * 0.5f;
		Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( plxCamPos );*/

		mt::vec2f oldPlx = camera->getParallax();

		mt::vec2f vp = camera->getViewport().begin;
		mt::vec2f parallax;
		parallax.x = m_factorParallax.x - ::floorf( vp.x * m_factorParallax.x / m_size.x ) * m_size.x / vp.x;
		parallax.y = m_factorParallax.y;

		camera->setParallax( parallax );

		VisitorRenderLayer2DLoop visitorRender( _viewport, m_size, _enableDebug );

		visitChildren( &visitorRender );

		//Holder<Player>::hostage()->getRenderCamera2D()->setLocalPosition( camPos );
		camera->setParallax( oldPlx );

		Holder<RenderEngine>::hostage()
			->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	class VisitorUpdateLayer2DLoop
		: public VisitorAdapter<VisitorUpdateLayer2DLoop>
	{
	public:
		VisitorUpdateLayer2DLoop( float _timing, const mt::vec2f& _size )
			: m_timing( _timing )
			, m_size( _size )
		{
		}

	public:
		void visit( Node * _node )
		{				
			const mt::vec2f& pos = _node->getWorldPosition();
			if( pos.x > m_size.x )
			{
				_node->translate( mt::vec2f( -m_size.x, 0.0f ));
			}
			else if( pos.x < 0 )
			{
				_node->translate( mt::vec2f( m_size.x, 0.0f ));
			}
			_node->update( m_timing );
		}

	protected:
		float m_timing;
		mt::vec2f m_size;
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer2DLoop::update( float _timing )
	{
		if( isUpdatable() == false )
		{
			return;
		}

		if( m_updatable )	// !!!!
		{
			_update( _timing );
		}

		VisitorUpdateLayer2DLoop visitorUpdate( _timing, m_size );

		visitChildren( &visitorUpdate );
		
	}
	//////////////////////////////////////////////////////////////////////////
}