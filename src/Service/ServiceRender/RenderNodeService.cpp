#include "RenderNodeService.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderNodeService, Mengine::RenderNodeService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    RenderNodeService::RenderNodeService()
        : m_invalidate( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderNodeService::~RenderNodeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderNodeService::_initializeService()
    {
        m_renders.reserve( 2048 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderNodeService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderNodeService::_stopService()
    {
        m_renders.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderNodeService::invalidateNodeCache()
    {
        m_invalidate = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderNodeService::renderNode( const RenderContext * _context, const RenderInterfacePtr & _render )
    {
        //uint32_t size = _node->getChildrenRecursiveCount();

        //if( m_invalidate == true )
        {
            m_invalidate = false;

            m_renders.clear();

            this->cacheNode_( _context, _render );
        }

        for( const RenderNodeDesc & desc : m_renders )
        {
            const RenderInterfacePtr & render = desc.render;

            render->render( &desc.context );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderNodeService::cacheNode_( const RenderContext * _context, const RenderInterfacePtr & _render )
    {
        if( _render->isRenderEnable() == false )
        {
            return;
        }

        if( _render->isExternalRender() == true )
        {
            return;
        }

        if( _render->isHide() == true )
        {
            return;
        }

        if( _render->isLocalTransparent() == true )
        {
            return;
        }

        RenderNodeDesc desc;
        desc.render = _render;

        const RenderViewportInterfacePtr & renderViewport = _render->getRenderViewport();

        if( renderViewport != nullptr )
        {
            desc.context.viewport = renderViewport;
        }
        else
        {
            desc.context.viewport = _context->viewport;
        }

        const RenderCameraInterfacePtr & renderCamera = _render->getRenderCamera();

        if( renderCamera != nullptr )
        {
            desc.context.camera = renderCamera;
        }
        else
        {
            desc.context.camera = _context->camera;
        }

        const RenderScissorInterfacePtr & renderScissor = _render->getRenderScissor();

        if( renderScissor != nullptr )
        {
            desc.context.scissor = renderScissor;
        }
        else
        {
            desc.context.scissor = _context->scissor;
        }

        const RenderTargetInterfacePtr & renderTarget = _render->getRenderTarget();

        if( renderTarget != nullptr )
        {
            desc.context.target = renderTarget;
        }
        else
        {
            desc.context.target = _context->target;
        }

        if( _render->isLocalHide() == false && _render->isPersonalTransparent() == false )
        {
            m_renders.emplace_back( desc );
        }

        const RenderContext * childrenContext = &desc.context;

        _render->foreachChildren( [this, childrenContext]( RenderInterface * _child )
        {
            this->cacheNode_( childrenContext, _child );
        } );

        if( desc.context.target != nullptr )
        {
            const RenderInterfacePtr & targetRender = _render->makeTargetRender( &desc.context );

            if( targetRender != nullptr )
            {
                RenderNodeDesc targetDesc;
                targetDesc.render = targetRender;
                targetDesc.context = *_context;

                m_renders.emplace_back( targetDesc );
            }
        }
        
    }
}