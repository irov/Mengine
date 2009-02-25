#	include "Layer2DLoop.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"
#	include "Application.h"

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

				if( viewport.testRectangle( bbox.minimum - leftBouncing, bbox.maximum - leftBouncing ) == true )
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
				else if( viewport.testRectangle( bbox.minimum - rightBouncing, bbox.maximum - rightBouncing ) == true )
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

		const Viewport & cameraViewport = camera->getViewport();

		const mt::vec2f & vp = cameraViewport.begin;
		mt::vec2f parallax;
		if( ::fabsf( vp.x ) > 0.0001f )
		{
			parallax.x = m_factorParallax.x - ::floorf( vp.x * m_factorParallax.x / m_size.x ) * m_size.x / vp.x;
		}
		else
		{
			parallax.x = m_factorParallax.x;
		}
		parallax.y = m_factorParallax.y;

		camera->setParallax( parallax );

		const mt::mat4f& viewMatrixSecond = camera->getViewMatrix();

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
			//const mt::box2f & bbox = _node->getBoundingBox();
			const mt::vec2f& pos = _node->getLocalPosition();

			if( pos.x >= m_size.x )
			{
				_node->translate( mt::vec2f( -m_size.x, 0.0f ));
			}
			else if( pos.x < 0.f )
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
	bool Layer2DLoop::testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const
	{
		mt::vec2f parallax;
		if( ::fabsf( _viewport.begin.x ) > 0.0001f )
		{
			parallax.x = m_factorParallax.x - ::floorf( _viewport.begin.x * m_factorParallax.x / m_size.x ) * m_size.x / _viewport.begin.x;
		}
		else
		{
			parallax.x = m_factorParallax.x;
		}
		parallax.y = m_factorParallax.y;

		Viewport convertView = _viewport;
		convertView.parallax( parallax );

		bool result = Layer::testBoundingBox( convertView, _layerspaceBox, _screenspaceBox );

		//bool result = Layer2D::testBoundingBox( _viewport, _layerspaceBox, _screenspaceBox );

		if( result == true )
		{
			return true;
		}

		if( _layerspaceBox.maximum.x > convertView.end.x )
		{
			Viewport leftViewport = convertView;
			//leftViewport.parallax( m_factorParallax );

			leftViewport.begin.x += m_size.x;
			leftViewport.end.x += m_size.x;
			result = Layer::testBoundingBox( leftViewport, _layerspaceBox, _screenspaceBox );
		}

		if( result == true )
		{
			return true;
		}

		if( _layerspaceBox.minimum.x < convertView.begin.x )
		{
			Viewport rightViewport = convertView;
			//rightViewport.parallax( m_factorParallax );

			rightViewport.begin.x -= m_size.x;
			rightViewport.end.x -= m_size.x;
			result = Layer::testBoundingBox( rightViewport, _layerspaceBox, _screenspaceBox );
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2DLoop::testHotspot( const Viewport & _viewport, HotSpot * _layerspaceHotspot, HotSpot * _screenspaceHotspot ) const 
	{
		const mt::vec2f & dirA = _layerspaceHotspot->getWorldDirection();
		//const mt::vec2f & posA = _layerspaceHotspot->getScreenPosition();
		//mt::vec2f posA = Layer2D::calcScreenPosition( _layerspaceHotspot );
		Viewport vp = _viewport;
		mt::vec2f parallax;
		if( ::fabsf( _viewport.begin.x ) > 0.0001f )
		{
			parallax.x = m_factorParallax.x - ::floorf( _viewport.begin.x * m_factorParallax.x / m_size.x ) * m_size.x / _viewport.begin.x;
		}
		else
		{
			parallax.x = m_factorParallax.x;
		}
		parallax.y = m_factorParallax.y;
		vp.parallax( parallax );

		mt::vec2f posA = _layerspaceHotspot->getWorldPosition() - vp.begin;

		const mt::vec2f & dirB = _screenspaceHotspot->getWorldDirection();
		const mt::vec2f & posB = _screenspaceHotspot->getWorldPosition();

		const mt::polygon & layerspacePolygon = _layerspaceHotspot->getPolygon();
		const mt::polygon & screenspacePolygon = _screenspaceHotspot->getPolygon();

		bool is_intersect = mt::intersect_poly_poly( 
			layerspacePolygon, 
			screenspacePolygon,
			dirA, 
			posA, 
			dirB, 
			posB 
			);

		if( is_intersect == true )
		{
			return true;
		}

		posA.x -= m_size.x;

		is_intersect = mt::intersect_poly_poly( 
			layerspacePolygon, 
			screenspacePolygon,
			dirA, 
			posA, 
			dirB, 
			posB 
			);

		if( is_intersect == true )
		{
			return true;
		}

		posA.x += m_size.x * 2.0f;

		is_intersect = mt::intersect_poly_poly( 
			layerspacePolygon, 
			screenspacePolygon,
			dirA, 
			posA, 
			dirB, 
			posB 
			);

		return is_intersect;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2DLoop::testHotspot( const Viewport & _viewport, HotSpot * _layerspaceHotspot, const mt::vec2f& _point ) const 
	{
		const mt::vec2f & dirA = _layerspaceHotspot->getWorldDirection();

		Viewport vp = _viewport;
		mt::vec2f parallax;
		if( ::fabsf( _viewport.begin.x ) > 0.0001f )
		{
			parallax.x = m_factorParallax.x - ::floorf( _viewport.begin.x * m_factorParallax.x / m_size.x ) * m_size.x / _viewport.begin.x;
		}
		else
		{
			parallax.x = m_factorParallax.x;
		}
		parallax.y = m_factorParallax.y;
		vp.parallax( parallax );

		mt::vec2f posA = _layerspaceHotspot->getWorldPosition() - vp.begin;

		const mt::polygon & layerspacePolygon = _layerspaceHotspot->getPolygon();

		bool result = mt::is_point_inside_polygon( 
			layerspacePolygon, 
			_point,
			posA,
			dirA 
			);

		if( result == true )
		{
			return true;
		}

		posA.x -= m_size.x;

		result = mt::is_point_inside_polygon( 
			layerspacePolygon, 
			_point,
			posA, 
			dirA
			);

		if( result == true )
		{
			return true;
		}

		posA.x += m_size.x * 2.0f;

		result = mt::is_point_inside_polygon( 
			layerspacePolygon, 
			_point,
			posA, 
			dirA
			);

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Layer2DLoop::calcScreenPosition( const Viewport& _viewport, Node* _node ) const
	{
		Viewport vp = _viewport;
		mt::vec2f parallax;
		if( ::fabsf( _viewport.begin.x ) > 0.0001f )
		{
			parallax.x = m_factorParallax.x - ::floorf( _viewport.begin.x * m_factorParallax.x / m_size.x ) * m_size.x / _viewport.begin.x;
		}
		else
		{
			parallax.x = m_factorParallax.x;
		}
		parallax.y = m_factorParallax.y;
		vp.parallax( parallax );

		mt::vec2f screenPos = _node->getWorldPosition() - vp.begin;
		//mt::vec2f screenPos = Layer2D::calcScreenPosition( _node );
		const Resolution & currentResolution = Holder<Application>::hostage()->getCurrentResolution();

		float crx = float( currentResolution[0] );

		if( screenPos.x < 0.0f )
		{
			screenPos += mt::vec2f( m_size.x, 0.0f );
		}
		else if( screenPos.x >= crx )
		{
			screenPos -= mt::vec2f( m_size.x, 0.0f );
		}
		return screenPos;
	}
	//////////////////////////////////////////////////////////////////////////
}
