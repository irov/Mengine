#	include "Layer2D.h"

#	include "Kernel/Scene.h"

#	include "Camera2D.h"
#	include "RenderViewport.h"

#	include "Interface/NodeInterface.h"
#   include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer2D::Layer2D()
		: m_viewport(0.f, 0.f, 0.f, 0.f)
		, m_renderCamera(nullptr)
		, m_hasViewport(false)
		, m_renderViewport(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::_activate()
	{
		if( Layer::_activate() == false )
		{
			return false;
		}

        if( m_hasViewport == true )
        {
            this->createViewport_();
        }

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::_deactivate()
    {
        if( m_hasViewport == true )
        {
            this->clearViewport_();
        }

        Node::_deactivate();
    }
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Layer2D::cameraToLocal( Camera2D * _camera2D, const mt::vec2f& _point )
	{
		const Viewport & viewport = _camera2D->getCameraRenderport();

		return _point + viewport.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node * _node ) const
	{
		//Viewport vp = _viewport;
		//Viewport vp = _viewport;
		//vp.parallax( m_factorParallax );

		//const mt::vec3f & wpos = _node->getWorldPosition();

		//_screen = wpos.to_vec2f() - vp.begin;

		const mt::vec3f & wp = _node->getWorldPosition();

		const mt::mat4f & vm = _camera->getCameraViewMatrix();

		mt::vec3f sp;
		mt::mul_m4_v3( sp, vm, wp );

		_screen.x = sp.x;
		_screen.y = sp.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;
		
		m_hasViewport = true;

        this->createViewport_();
	}
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::createViewport_()
    {
        if( m_renderViewport == nullptr )
        {
			m_renderCamera = NODE_SERVICE(m_serviceProvider)
				->createNodeT<Camera2D>( CONST_STRING(m_serviceProvider, Camera2D) );

			if( m_renderCamera == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Layer2D::createRenderViewport_ %s invalid create Camera2D"
					, this->getName().c_str()
					);

				return;
			}

			this->addChildren( m_renderCamera );

            m_renderViewport = NODE_SERVICE(m_serviceProvider)
				->createNodeT<RenderViewport>( CONST_STRING(m_serviceProvider, RenderViewport) );

			if( m_renderViewport == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Layer2D::createRenderViewport_ %s invalid create RenderViewport"
					, this->getName().c_str()
					);

				return;
			}

			this->addChildren( m_renderViewport );
		}

		m_renderCamera->setRenderport( m_viewport );
        m_renderViewport->setViewport( m_viewport );

		Node::setRenderCamera( m_renderCamera );
		Node::setRenderViewport( m_renderViewport );
    }
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::removeViewport()
	{
		if( m_hasViewport == false )
		{
			return;
		}

        m_hasViewport = false;

        this->clearViewport_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Layer2D::clearViewport_()
    {	
        if( m_renderViewport != nullptr )
        {
		    m_renderViewport->destroy();
		    m_renderViewport = nullptr;
        }

		if( m_renderCamera != nullptr )
		{
			m_renderCamera->destroy();
			m_renderCamera = nullptr;
		}

		Node::setRenderCamera( nullptr );
		Node::setRenderViewport( nullptr );
	}
}
