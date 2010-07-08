#	include "Layer2DLoop.h"

#	include "XmlEngine.h"
#	include "Application.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Player.h"

#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "Logger/Logger.h"
#	include "Game.h"


#	include "Sprite.h"
#	include "Animation.h"

#	include "VisitorAdapter.h"

#	include "NodeManager.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer2DLoop::Layer2DLoop()
		: m_camera2DLeft( NULL )
		, m_camera2DRight( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	class VisitorRenderLayer2DLoop
		: public VisitorAdapter<VisitorRenderLayer2DLoop>
	{
	public:
		VisitorRenderLayer2DLoop( 
			const mt::vec2f & _size
			, Camera2D* _cameraLeft
			, Camera2D* _cameraRight
			, Camera2D* _cameraMain )
			: m_size(_size)
			, m_cameraLeft( _cameraLeft )
			, m_cameraRight( _cameraRight )
			, m_cameraMain( _cameraMain )
			, m_leftBouncing( -m_size.x, 0.0f )
			, m_rightBouncing( m_size.x, 0.0f )
		{
			m_cameraLeft->setOffset( m_leftBouncing );
			m_cameraRight->setOffset( m_rightBouncing );
		}

	public:
		void visit( Node * _node )
		{				
			if( _node->isRenderable() == true )
			{
				Holder<RenderEngine>::hostage()
					->setActiveCamera( m_cameraMain );

				const Viewport & mainViewport = m_cameraMain->getViewport();

				if( _node->checkVisibility( mainViewport ) == true )
				{
					_node->_render( m_cameraMain );
				}

				const mt::box2f & bbox = _node->getBoundingBox();

				if( mainViewport.testRectangle( bbox.minimum - m_leftBouncing, bbox.maximum - m_leftBouncing ) == true )
				{
					// left render
					Holder<RenderEngine>::hostage()->
						setActiveCamera( m_cameraLeft );

					const Viewport & leftViewport = m_cameraLeft->getViewport();

					if( _node->checkVisibility( leftViewport ) == true )
					{
						_node->_render( m_cameraLeft );
					}
				}
				else if( mainViewport.testRectangle( bbox.minimum - m_rightBouncing, bbox.maximum - m_rightBouncing ) == true )
				{
					// right render
					Holder<RenderEngine>::hostage()->
						setActiveCamera( m_cameraRight );

					const Viewport & rightViewport = m_cameraRight->getViewport();

					if( _node->checkVisibility( rightViewport ) == true )
					{
						_node->_render( m_cameraRight );
					}
				}
			}
			_node->visitChildren( this );
		}

		void visit( Layer * _layer )
		{
			Holder<RenderEngine>::hostage()
				->setActiveCamera( m_cameraMain );

			_layer->render( m_cameraMain );
		}

	protected:
		mt::vec2f m_size;
		Camera2D* m_cameraLeft;
		Camera2D* m_cameraRight;
		mt::vec2f m_leftBouncing;
		mt::vec2f m_rightBouncing;
		Camera2D* m_cameraMain;
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer2DLoop::render( Camera2D * _camera )
	{
		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		Camera2D* camera = m_scene->getCamera();//Holder<Player>::hostage()->getRenderCamera2D();

		//mt::vec2f oldPlx = camera->getParallax();

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


		/*float vpy = m_camera2D->getViewportSize().y;
		float wpy = m_camera2D->getWorldPosition().y;

		if( ( wpy - vpy * 0.5f ) <= 0.f )
		{
			m_camera2D->translate( mt::vec2f( 0.0f, 0.f + vpy * 0.5f - wpy ) );
		}

		if( ( wpy + vpy * 0.5f ) > m_size.y )
		{
			
			m_camera2D->translate( mt::vec2f( 0.0f, m_size.y - vpy * 0.5f - wpy ) );
		}*/

		m_camera2D->setParallax( parallax );
		m_camera2DLeft->setParallax( parallax );
		m_camera2DRight->setParallax( parallax );

		//const mt::mat4f& viewMatrixSecond = camera->getViewMatrix();

		//Holder<RenderEngine>::hostage()->setViewMatrix( viewMatrixSecond );

		Camera* oldCam = Holder<RenderEngine>::hostage()
			->getActiveCamera();

		Holder<RenderEngine>::hostage()
			->setActiveCamera( m_camera2D );

		VisitorRenderLayer2DLoop visitorRender( m_size
			, m_camera2DLeft
			, m_camera2DRight
			, m_camera2D );

		visitChildren( &visitorRender );

		//camera->setParallax( oldPlx );

		Holder<RenderEngine>::hostage()->
			setActiveCamera( oldCam );

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
	bool Layer2DLoop::testBoundingBox( const Viewport &, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const
	{
		const Viewport& vp = m_scene->getCamera()->getViewport();
		mt::vec2f parallax;
		if( ::fabsf( vp.begin.x ) > 0.0001f )
		{
			parallax.x = m_factorParallax.x - ::floorf( vp.begin.x * m_factorParallax.x / m_size.x ) * m_size.x / vp.begin.x;
		}
		else
		{
			parallax.x = m_factorParallax.x;
		}
		parallax.y = m_factorParallax.y;

		Viewport convertView = vp;
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
	bool Layer2DLoop::testPolygon( const Viewport &, HotSpot * _layerspaceHotspot, const mt::polygon & _polygon, const mt::mat3f & _wm ) const 
	{
		const mt::vec2f & dirA = _layerspaceHotspot->getWorldDirection();
		//const mt::vec2f & posA = _layerspaceHotspot->getScreenPosition();
		//mt::vec2f posA = Layer2D::calcScreenPosition( _layerspaceHotspot );
		Viewport vp = m_scene->getCamera()->getViewport();
		mt::vec2f parallax;
		if( ::fabsf( vp.begin.x ) > 0.0001f )
		{
			parallax.x = m_factorParallax.x - ::floorf( vp.begin.x * m_factorParallax.x / m_size.x ) * m_size.x / vp.begin.x;
		}
		else
		{
			parallax.x = m_factorParallax.x;
		}
		parallax.y = m_factorParallax.y;
		vp.parallax( parallax );

		mt::vec2f posA = _layerspaceHotspot->getWorldPosition() - vp.begin;

		const mt::vec2f & dirB = _wm.v0.to_vec2f(); //direction
		const mt::vec2f & posB = _wm.v2.to_vec2f(); //position

		const mt::polygon & layerspacePolygon = _layerspaceHotspot->getPolygon();
		const mt::polygon & screenspacePolygon = _polygon;

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
	bool Layer2DLoop::testPoint( const Viewport & _viewport, HotSpot * _layerspaceHotspot, const mt::vec2f& _point ) const 
	{
		const mt::vec2f & dirA = _layerspaceHotspot->getWorldDirection();

		Viewport vp = m_scene->getCamera()->getViewport();
		mt::vec2f parallax;
		if( ::fabsf( vp.begin.x ) > 0.0001f )
		{
			parallax.x = m_factorParallax.x - ::floorf( vp.begin.x * m_factorParallax.x / m_size.x ) * m_size.x / vp.begin.x;
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
	mt::vec2f Layer2DLoop::calcScreenPosition( const Viewport& , Node* _node ) const
	{
		Viewport vp = m_scene->getCamera()->getViewport();
		mt::vec2f parallax;
		if( ::fabsf( vp.begin.x ) > 0.0001f )
		{
			parallax.x = m_factorParallax.x - ::floorf( vp.begin.x * m_factorParallax.x / m_size.x ) * m_size.x / vp.begin.x;
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
	bool Layer2DLoop::_activate()
	{
		if( Layer2D::_activate() == false )
		{
			return false;
		}

		m_camera2DLeft = Holder<NodeManager>::hostage()->createNodeT<Camera2D>( "Camera2D" );
		m_camera2DLeft->setViewportSize( m_camera2D->getViewportSize() );
		m_camera2D->addChildren( m_camera2DLeft );

		m_camera2DRight = Holder<NodeManager>::hostage()->createNodeT<Camera2D>( "Camera2D" );
		m_camera2DRight->setViewportSize( m_camera2D->getViewportSize() );
		m_camera2D->addChildren( m_camera2DRight );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DLoop::_deactivate()
	{
		if( m_camera2DLeft != NULL )
		{
			//m_camera2D->removeChildren( m_camera2DLeft );
			m_camera2DLeft->destroy();
			m_camera2DLeft = NULL;
		}

		if( m_camera2DRight != NULL )
		{
			//m_camera2D->removeChildren( m_camera2DRight );
			m_camera2DRight->destroy();
			m_camera2DRight = NULL;
		}

		Layer2D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
}
