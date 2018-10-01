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
        m_nodies.reserve( 2048 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderNodeService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderNodeService::_stopService()
    {
        m_nodies.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderNodeService::invalidateNodeCache()
    {
        m_invalidate = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderNodeService::renderNode( const RenderContext * _context, const NodePtr & _node )
    {
        uint32_t size = _node->getChildrenRecursiveCount();

        //if( m_invalidate == true )
        {
            m_invalidate = false;

            m_nodies.clear();

            this->cacheNode_( _context, _node );
        }

        for( const RenderNodeDesc & desc : m_nodies )
        {
            const RenderInterfacePtr & render = desc.render;

            render->render( &desc.context );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderNodeService::cacheNode_( const RenderContext * _context, const NodePtr & _node )
    {
        if( _node->isActivate() == false )
        {
            return;
        }
        else if( _node->isHide() == true )
        {
            return;
        }

        RenderInterface * selfRender = _node->getRender();

        if( selfRender != nullptr )
        {
            if( selfRender->isExternalRender() == true )
            {
                return;
            }

            if( selfRender->isLocalTransparent() == true )
            {
                return;
            }

            RenderNodeDesc desc;
            desc.render = selfRender;

            const RenderViewportInterfacePtr & renderViewport = selfRender->getRenderViewport();

            if( renderViewport != nullptr )
            {
                desc.context.viewport = renderViewport;
            }
            else
            {
                desc.context.viewport = _context->viewport;
            }

            const RenderCameraInterfacePtr & renderCamera = selfRender->getRenderCamera();

            if( renderCamera != nullptr )
            {
                desc.context.camera = renderCamera;
            }
            else
            {
                desc.context.camera = _context->camera;
            }

            const RenderScissorInterfacePtr & renderScissor = selfRender->getRenderScissor();

            if( renderScissor != nullptr )
            {
                desc.context.scissor = renderScissor;
            }
            else
            {
                desc.context.scissor = _context->scissor;
            }

            const RenderTargetInterfacePtr & renderTarget = selfRender->getRenderTarget();

            if( renderTarget != nullptr )
            {
                desc.context.target = renderTarget;
            }
            else
            {
                desc.context.target = _context->target;
            }

            if( _node->isLocalHide() == false && selfRender->isPersonalTransparent() == false )
            {
                m_nodies.emplace_back( desc );
            }

            const RenderContext * childrenContext = &desc.context;

            _node->foreachChildren( [this, childrenContext]( const NodePtr & _child )
            {
                this->cacheNode_( childrenContext, _child );
            } );

            if( desc.context.target != nullptr )
            {
                const RenderInterfacePtr & targetRender = selfRender->makeTargetRender( &desc.context );

                if( targetRender != nullptr )
                {
                    RenderNodeDesc targetDesc;
                    targetDesc.render = targetRender;
                    targetDesc.context = *_context;

                    m_nodies.emplace_back( targetDesc );
                }
            }
        }
        else
        {
            _node->foreachChildren( [this, _context]( const NodePtr & _child )
            {
                this->cacheNode_( _context, _child );
            } );
        }
    }
}