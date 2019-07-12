#include "NodeRenderHierarchy.h"

#define MENGINE_NEW_RENDER

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void nodeRenderChildren( const Node * _node, const RenderContext * _context, bool _external )
        {
            const RenderInterface * selfRender = _node->getRenderConst();

#ifdef MENGINE_NEW_RENDER
            selfRender->renderWithChildren( _context, _external );
#else
            if( selfRender != nullptr )
            {
                if( selfRender->isRenderEnable() == false )
                {
                    return;
                }

                if( selfRender->isExternalRender() == true && _external == false )
                {
                    return;
                }

                if( selfRender->isHide() == true )
                {
                    return;
                }

                if( selfRender->isLocalTransparent() == true )
                {
                    return;
                }

                RenderContext self_context;

                const RenderViewportInterfacePtr & renderViewport = selfRender->getRenderViewport();
                const RenderCameraInterfacePtr & renderCamera = selfRender->getRenderCamera();
                const RenderTransformationInterfacePtr & renderTransformation = selfRender->getRenderTransformation();
                const RenderScissorInterfacePtr & renderScissor = selfRender->getRenderScissor();
                const RenderTargetInterfacePtr & renderTarget = selfRender->getRenderTarget();

                self_context.viewport = renderViewport != nullptr ? renderViewport : _context->viewport;
                self_context.camera = renderCamera != nullptr ? renderCamera : _context->camera;
                self_context.transformation = renderTransformation != nullptr ? renderTransformation : _context->transformation;
                self_context.scissor = renderScissor != nullptr ? renderScissor : _context->scissor;
                self_context.target = renderTarget != nullptr ? renderTarget : _context->target;

                if( selfRender->isLocalHide() == false && selfRender->isPersonalTransparent() == false )
                {
                    selfRender->render( &self_context );
                }

                const RenderContext * children_context = &self_context;
                _node->foreachChildrenUnslug( [children_context]( const Node * _child )
                {
                    Helper::nodeRenderChildren( _child, children_context, false );
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
                _node->foreachChildrenUnslug( [_context]( const Node * _child )
                {
                    Helper::nodeRenderChildren( _child, _context, false );
                } );
            }
#endif
        }
        //////////////////////////////////////////////////////////////////////////
        RenderInterface * getNodeRenderInheritance( Node * _node )
        {
            if( _node == nullptr )
            {
                return nullptr;
            }

            RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                return render;
            }

            Node * parent = _node->getParent();

            RenderInterface * render_parent = Helper::getNodeRenderInheritance( parent );

            return render_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderViewportInterfacePtr & getRenderViewportInheritance( RenderInterface * _render )
        {
            const RenderViewportInterfacePtr & viewport = _render->getRenderViewport();

            if( viewport != nullptr )
            {
                return viewport;
            }

            RenderInterface * relationRender = _render->getRelationRender();

            if( relationRender == nullptr )
            {
                return RenderViewportInterfacePtr::none();
            }

            const RenderViewportInterfacePtr & relation_viewport = Helper::getRenderViewportInheritance( relationRender );

            return relation_viewport;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderCameraInterfacePtr & getRenderCameraInheritance( RenderInterface * _render )
        {
            const RenderCameraInterfacePtr & camera = _render->getRenderCamera();

            if( camera != nullptr )
            {
                return camera;
            }

            RenderInterface * relationRender = _render->getRelationRender();

            if( relationRender == nullptr )
            {
                return RenderCameraInterfacePtr::none();
            }

            const RenderCameraInterfacePtr & relation_camera = Helper::getRenderCameraInheritance( relationRender );

            return relation_camera;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTransformationInterfacePtr & getRenderTransformationInheritance( RenderInterface * _render )
        {
            const RenderTransformationInterfacePtr & transformation = _render->getRenderTransformation();

            if( transformation != nullptr )
            {
                return transformation;
            }

            RenderInterface * relationRender = _render->getRelationRender();

            if( relationRender == nullptr )
            {
                return RenderTransformationInterfacePtr::none();
            }

            const RenderTransformationInterfacePtr & relation_transformation = Helper::getRenderTransformationInheritance( relationRender );

            return relation_transformation;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderScissorInterfacePtr & getRenderScissorInheritance( RenderInterface * _render )
        {
            const RenderScissorInterfacePtr & scissor = _render->getRenderScissor();

            if( scissor != nullptr )
            {
                return scissor;
            }

            RenderInterface * relationRender = _render->getRelationRender();

            if( relationRender == nullptr )
            {
                return RenderScissorInterfacePtr::none();
            }

            const RenderScissorInterfacePtr & relation_scissor = Helper::getRenderScissorInheritance( relationRender );

            return relation_scissor;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTargetInterfacePtr & getRenderTargetInheritance( RenderInterface * _render )
        {
            const RenderTargetInterfacePtr & target = _render->getRenderTarget();

            if( target != nullptr )
            {
                return target;
            }

            RenderInterface * relationRender = _render->getRelationRender();

            if( relationRender == nullptr )
            {
                return RenderTargetInterfacePtr::none();
            }

            const RenderTargetInterfacePtr & relation_target = Helper::getRenderTargetInheritance( relationRender );

            return relation_target;
        }
        //////////////////////////////////////////////////////////////////////////
        void getNodeRenderContext( const NodePtr & _node, RenderContext * _context )
        {
            const RenderViewportInterfacePtr & viewport = Helper::getNodeRenderViewportInheritance( _node.get() );

            if( viewport != nullptr )
            { 
                _context->viewport = viewport;
            }
            
            const RenderCameraInterfacePtr & camera = Helper::getNodeRenderCameraInheritance( _node.get() );

            if( camera != nullptr )
            {
                _context->camera = camera;
            }

            const RenderTransformationInterfacePtr & transformation = Helper::getNodeRenderTransformationInheritance( _node.get() );

            if( transformation != nullptr )
            {
                _context->transformation = transformation;
            }

            const RenderScissorInterfacePtr & scissor = Helper::getNodeRenderScissorInheritance( _node.get() );

            if( camera != nullptr )
            {
                _context->scissor = scissor;
            }

            const RenderTargetInterfacePtr & target = Helper::getNodeRenderTargetInheritance( _node.get() );

            if( camera != nullptr )
            {
                _context->target = target;
            }

        }
        //////////////////////////////////////////////////////////////////////////
        const RenderViewportInterfacePtr & getNodeRenderViewportInheritance( Node * _node )
        {
            RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderViewportInterfacePtr & viewport = Helper::getRenderViewportInheritance( render );

                return viewport;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return RenderViewportInterfacePtr::none();
            }

            const RenderViewportInterfacePtr & viewport_parent = Helper::getNodeRenderViewportInheritance( parent );

            return viewport_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderCameraInterfacePtr & getNodeRenderCameraInheritance( Node * _node )
        {
            RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderCameraInterfacePtr & camera = Helper::getRenderCameraInheritance( render );

                return camera;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return RenderCameraInterfacePtr::none();
            }

            const RenderCameraInterfacePtr & camera_parent = Helper::getNodeRenderCameraInheritance( parent );

            return camera_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTransformationInterfacePtr & getNodeRenderTransformationInheritance( Node * _node )
        {
            RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderTransformationInterfacePtr & transformation = Helper::getRenderTransformationInheritance( render );

                return transformation;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return RenderTransformationInterfacePtr::none();
            }

            const RenderTransformationInterfacePtr & camera_parent = Helper::getNodeRenderTransformationInheritance( parent );

            return camera_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderScissorInterfacePtr & getNodeRenderScissorInheritance( Node * _node )
        {
            RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderScissorInterfacePtr & scissor = Helper::getRenderScissorInheritance( render );

                return scissor;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return RenderScissorInterfacePtr::none();
            }

            const RenderScissorInterfacePtr & scissor_parent = Helper::getNodeRenderScissorInheritance( parent );

            return scissor_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTargetInterfacePtr & getNodeRenderTargetInheritance( Node * _node )
        {
            RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderTargetInterfacePtr & target = Helper::getRenderTargetInheritance( render );

                return target;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return RenderTargetInterfacePtr::none();
            }

            const RenderTargetInterfacePtr & target_parent = Helper::getNodeRenderTargetInheritance( parent );

            return target_parent;
        }
    }
}