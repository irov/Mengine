#include "NodeRenderHelper.h"

#include "Kernel/Node.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void nodeRenderChildren( const NodePtr & _node, const RenderContext * _context )
        {
            if( _node->isActivate() == false )
            {
                return;
            }
            else if( _node->isHide() == true )
            {
                return;
            }
            else if( _node->isLocalTransparent() == true )
            {
                return;
            }

            RenderInterfacePtr selfRender = _node->getRender();
            
            if( selfRender != nullptr )
            {
                if( selfRender->isExternalRender() == true )
                {
                    return;
                }

                RenderContext self_context;

                const RenderViewportInterfacePtr & renderViewport = selfRender->getRenderViewport();

                if( renderViewport != nullptr )
                {
                    self_context.viewport = renderViewport;
                }
                else
                {
                    self_context.viewport = _context->viewport;
                }

                const RenderCameraInterfacePtr & renderCamera = selfRender->getRenderCamera();

                if( renderCamera != nullptr )
                {
                    self_context.camera = renderCamera;
                }
                else
                {
                    self_context.camera = _context->camera;
                }

                const RenderScissorInterfacePtr & renderScissor = selfRender->getRenderScissor();

                if( renderScissor != nullptr )
                {
                    self_context.scissor = renderScissor;
                }
                else
                {
                    self_context.scissor = _context->scissor;
                }

                const RenderTargetInterfacePtr & renderTarget = selfRender->getRenderTarget();

                if( renderTarget != nullptr )
                {
                    self_context.target = renderTarget;
                }
                else
                {
                    self_context.target = _context->target;
                }

                if( _node->isLocalHide() == false && _node->isPersonalTransparent() == false )
                {
                    selfRender->render( _context );
                }

                const RenderContext * children_context = &self_context;
                _node->foreachChildren( [children_context]( const NodePtr & _child )
                {
                    Helper::nodeRenderChildren( _child, children_context );
                } );

                if( self_context.target != nullptr )
                {
                    const RenderInterfacePtr & targetRender = selfRender->makeTargetRender( &self_context );

                    if( targetRender != nullptr )
                    {
                        targetRender->render( _context );
                    }
                }
            }
            else
            {
                _node->foreachChildren( [_context]( const NodePtr & _child )
                {
                    Helper::nodeRenderChildren( _child, _context );
                } );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void nodeRenderChildrenExternal( const NodePtr & _node, const RenderContext * _context )
        {
            if( _node->isActivate() == false )
            {
                return;
            }
            else if( _node->isHide() == true )
            {
                return;
            }
            else if( _node->isLocalTransparent() == true )
            {
                return;
            }

            RenderInterfacePtr selfRender = _node->getRender();

            if( selfRender != nullptr )
            {
                RenderContext self_context;

                const RenderViewportInterfacePtr & renderViewport = selfRender->getRenderViewport();

                if( renderViewport != nullptr )
                {
                    self_context.viewport = renderViewport;
                }
                else
                {
                    self_context.viewport = _context->viewport;
                }

                const RenderCameraInterfacePtr & renderCamera = selfRender->getRenderCamera();

                if( renderCamera != nullptr )
                {
                    self_context.camera = renderCamera;
                }
                else
                {
                    self_context.camera = _context->camera;
                }

                const RenderScissorInterfacePtr & renderScissor = selfRender->getRenderScissor();

                if( renderScissor != nullptr )
                {
                    self_context.scissor = renderScissor;
                }
                else
                {
                    self_context.scissor = _context->scissor;
                }

                const RenderTargetInterfacePtr & renderTarget = selfRender->getRenderTarget();

                if( renderTarget != nullptr )
                {
                    self_context.target = renderTarget;
                }
                else
                {
                    self_context.target = _context->target;
                }

                if( _node->isLocalHide() == false && _node->isPersonalTransparent() == false )
                {
                    selfRender->render( _context );
                }

                const RenderContext * children_context = &self_context;
                _node->foreachChildren( [children_context]( const NodePtr & _child )
                {
                    Helper::nodeRenderChildren( _child, children_context );
                } );

                if( self_context.target != nullptr )
                {
                    const RenderInterfacePtr & targetRender = selfRender->makeTargetRender( &self_context );

                    if( targetRender != nullptr )
                    {
                        targetRender->render( _context );
                    }
                }
            }
            else
            {
                _node->foreachChildren( [_context]( const NodePtr & _child )
                {
                    Helper::nodeRenderChildren( _child, _context );
                } );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void nodeRenderChildrenVisitor( const NodePtr & _node, const RenderVisitorPtr & _visitor, const RenderContext * _context )
        {
            if( _node->isActivate() == false )
            {
                return;
            }
            else if( _node->isHide() == true )
            {
                return;
            }
            else if( _node->isLocalTransparent() == true )
            {
                return;
            }

            RenderInterfacePtr render = _node->getRender();

            if( render != nullptr )
            {
                if( render->isExternalRender() == true )
                {
                    return;
                }

                RenderContext self_context;

                const RenderViewportInterfacePtr & renderViewport = render->getRenderViewport();

                if( renderViewport != nullptr )
                {
                    self_context.viewport = renderViewport;
                }
                else
                {
                    self_context.viewport = _context->viewport;
                }

                const RenderCameraInterfacePtr & renderCamera = render->getRenderCamera();

                if( renderCamera != nullptr )
                {
                    self_context.camera = renderCamera;
                }
                else
                {
                    self_context.camera = _context->camera;
                }

                const RenderScissorInterfacePtr & renderScissor = render->getRenderScissor();

                if( renderScissor != nullptr )
                {
                    self_context.scissor = renderScissor;
                }
                else
                {
                    self_context.scissor = _context->scissor;
                }

                const RenderTargetInterfacePtr & renderTarget = render->getRenderTarget();

                if( renderTarget != nullptr )
                {
                    self_context.target = renderTarget;
                }
                else
                {
                    self_context.target = _context->target;
                }

                if( _node->isLocalHide() == false && _node->isPersonalTransparent() == false )
                {
                    render->render( &self_context );

                    _visitor->setRenderContext( &self_context );

                    _node->visit( _visitor );
                }

                const RenderContext * children_context = &self_context;
                _node->foreachChildren( [children_context, _visitor]( const NodePtr & _child )
                {
                    Helper::nodeRenderChildrenVisitor( _child, _visitor, children_context );
                } );

                if( self_context.target != nullptr )
                {
                    const RenderInterfacePtr & targetRender = render->makeTargetRender( &self_context );

                    if( targetRender != nullptr )
                    {
                        targetRender->render( _context );
                    }
                }
            }
            else
            {
                if( _node->isLocalHide() == false && _node->isPersonalTransparent() == false )
                {
                    _visitor->setRenderContext( _context );

                    _node->visit( _visitor );
                }

                _node->foreachChildren( [_context, _visitor]( const NodePtr & _child )
                {
                    Helper::nodeRenderChildrenVisitor( _child, _visitor, _context );
                } );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        RenderInterfacePtr getRenderInheritance( const Node * _node )
        {
            RenderInterfacePtr render = _node->getRender();

            if( render != nullptr )
            {
                return render;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return nullptr;
            }

            RenderInterfacePtr render_parent = Helper::getRenderInheritance( parent );

            return render_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderViewportInterfacePtr & getRenderViewportInheritance( const Node * _node )
        {
            RenderInterfacePtr render = _node->getRender();

            if( render != nullptr )
            {
                const RenderViewportInterfacePtr & viewport = render->getRenderViewport();

                if( viewport != nullptr )
                {
                    return viewport;
                }
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return RenderViewportInterfacePtr::none();
            }

            const RenderViewportInterfacePtr & viewport_parent = Helper::getRenderViewportInheritance( parent );

            return viewport_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderCameraInterfacePtr & getRenderCameraInheritance( const Node * _node )
        {
            RenderInterfacePtr render = _node->getRender();

            if( render != nullptr )
            {
                const RenderCameraInterfacePtr & camera = render->getRenderCamera();

                if( camera != nullptr )
                {
                    return camera;
                }
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return RenderCameraInterfacePtr::none();
            }

            const RenderCameraInterfacePtr & camera_parent = Helper::getRenderCameraInheritance( parent );

            return camera_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderScissorInterfacePtr & getRenderScissorInheritance( const Node * _node )
        {
            RenderInterfacePtr render = _node->getRender();

            if( render != nullptr )
            {
                const RenderScissorInterfacePtr & scissor = render->getRenderScissor();

                if( scissor != nullptr )
                {
                    return scissor;
                }
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return RenderScissorInterfacePtr::none();
            }

            const RenderScissorInterfacePtr & scissor_parent = Helper::getRenderScissorInheritance( parent );

            return scissor_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTargetInterfacePtr & getRenderTargetInheritance( const Node * _node )
        {
            RenderInterfacePtr render = _node->getRender();

            if( render != nullptr )
            {
                const RenderTargetInterfacePtr & target = render->getRenderTarget();

                if( target != nullptr )
                {
                    return target;
                }
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return RenderTargetInterfacePtr::none();
            }

            const RenderTargetInterfacePtr & target_parent = Helper::getRenderTargetInheritance( parent );

            return target_parent;
        }
    }
}