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
		VisitorRenderLayer2DLoop( const mt::vec2f & _size, unsigned int _debugMask )
			: m_size(_size)
			, m_debugMask( _debugMask )
		{
		}

	public:
		void visit( Node * _node )
		{				
			if( _node->isRenderable() == true )
			{
				//const mt::box2f & sprite_bbox = _node->getBoundingBox();
				Camera2D* camera = Holder<Player>::hostage()->getRenderCamera2D();

				const mt::mat4f & viewMatrixFirst = camera->getViewMatrix();

				Holder<RenderEngine>::hostage()->setViewMatrix( viewMatrixFirst );

				if( _node->checkVisibility() == true )
				{
					_node->_render( m_debugMask );
				}

				const mt::box2f & bbox = _node->getBoundingBox();

				const Viewport & viewport = camera->getViewport();

				mt::vec2f leftBouncing( -m_size.x, 0.0f );
				mt::vec2f rightBouncing( m_size.x, 0.0f );

				if( viewport.testRectangle( bbox.vb - leftBouncing, bbox.ve - leftBouncing ) == true )
					// left render
				{
					mt::vec2f oldOffs = camera->getOffset();

					camera->setOffset( oldOffs + leftBouncing );

					const mt::mat4f & viewMatrixSecond = camera->getViewMatrix();

					Holder<RenderEngine>::hostage()->setViewMatrix( viewMatrixSecond );

					if( _node->checkVisibility() == true )
					{
						_node->_render( m_debugMask );
					}

					camera->setOffset( oldOffs );
				}
				else if( viewport.testRectangle( bbox.vb - rightBouncing, bbox.ve - rightBouncing ) == true )
					// right render
				{
					mt::vec2f oldOffs = camera->getOffset();

					camera->setOffset( oldOffs + rightBouncing );

					const mt::mat4f & viewMatrixSecond = camera->getViewMatrix();

					Holder<RenderEngine>::hostage()->setViewMatrix( viewMatrixSecond );

					if( _node->checkVisibility() == true )
					{
						_node->_render( m_debugMask );
					}

					camera->setOffset( oldOffs );
				}
			}
			_node->visitChildren( this );
		}

		void visit( Layer * _layer )
		{
			_layer->render( m_debugMask );
		}

	protected:
		unsigned int m_debugMask;
		mt::vec2f m_size;
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer2DLoop::render( unsigned int _debugMask )
	{
		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		Camera2D* camera = Holder<Player>::hostage()->getRenderCamera2D();

		mt::vec2f oldPlx = camera->getParallax();

		mt::vec2f vp = camera->getViewport().begin;
		mt::vec2f parallax;
		parallax.x = m_factorParallax.x - ::floorf( vp.x * m_factorParallax.x / m_size.x ) * m_size.x / vp.x;
		parallax.y = m_factorParallax.y;

		camera->setParallax( parallax );

		const mt::mat4f & viewMatrixSecond = camera->getViewMatrix();

		Holder<RenderEngine>::hostage()->setViewMatrix( viewMatrixSecond );

		VisitorRenderLayer2DLoop visitorRender( m_size, _debugMask );

		visitChildren( &visitorRender );

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
			const mt::box2f & bbox = _node->getBoundingBox();

			if( bbox.vb.x >= m_size.x )
			{
				_node->translate( mt::vec2f( -m_size.x, 0.0f ));
			}
			else if( bbox.ve.x < 0.f )
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