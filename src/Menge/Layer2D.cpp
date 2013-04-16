#	include "Layer2D.h"

#	include "Kernel/Scene.h"

#	include "Camera2D.h"

#	include "Player.h"

#	include "Interface/NodeInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/PhysicSystem2DInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "Sprite.h"
#	include "Animation.h"


namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer2D::Layer2D()
		: m_factorParallax(1.f, 1.f)
		, m_viewport(0.f, 0.f, 0.f, 0.f)
		, m_hasViewport(false)
		, m_cameraViewport(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setParallaxFactor( const mt::vec2f & _factor )
	{
		m_factorParallax = _factor;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Layer2D::getParallaxFactor() const
	{
		return m_factorParallax;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::_activate()
	{
		Node::_activate();

		if( m_scene == 0 )
		{
			return false;
		}

        if( m_hasViewport == true )
        {
            this->createRenderViewport_();
        }

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::_deactivate()
    {
        if( m_hasViewport == true )
        {
            this->clearRenderViewport_();
        }

        Node::_deactivate();
    }
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_addChildren( Node * _node )
	{
		_node->setLayer( this );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Layer2D::cameraToLocal( Camera2D * _camera2D, const mt::vec2f& _point )
	{
		const Viewport & viewport = _camera2D->getViewport();

		Viewport vp = viewport;
		vp.begin.x *= m_factorParallax.x;
		vp.begin.y *= m_factorParallax.y;

		return _point + vp.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const
	{
		Viewport vp = _viewport;
		vp.parallax( m_factorParallax );

		bool result = Layer::testBoundingBox( _viewport, _layerspaceBox, _screenspaceBox );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::calcScreenPosition( mt::vec2f & _screen, const Viewport& _viewport, Node* _node ) const
	{
		//Viewport vp = _viewport;
		Viewport vp = _viewport;
		vp.parallax( m_factorParallax );

		const mt::vec3f & wpos = _node->getWorldPosition();

		_screen = wpos.to_vec2f() - vp.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setRenderViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;
		
		m_hasViewport = true;

        this->createRenderViewport_();
	}
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::createRenderViewport_()
    {
        if( m_cameraViewport == NULL )
        {
            m_cameraViewport = NODE_SERVICE(m_serviceProvider)
                ->createNodeT<Camera2D>( CONST_STRING(m_serviceProvider, Camera2D) );            

            this->addChildren( m_cameraViewport );
        }

        m_cameraViewport->setViewport( m_viewport );       

        this->setRenderCamera( m_cameraViewport );
    }
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::removeRenderViewport()
	{
		if( m_hasViewport == false )
		{
			return;
		}

        m_hasViewport = false;

        this->clearRenderViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::clearRenderViewport_()
    {	
        if( m_cameraViewport != NULL )
        {
		    m_cameraViewport->destroy();
		    m_cameraViewport = NULL;
        }

		this->setRenderCamera( NULL );
	}
}
