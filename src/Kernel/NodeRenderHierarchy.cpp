#include "NodeRenderHierarchy.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void nodeRenderChildren( const Node * _node, const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external )
        {
            const RenderInterface * selfRender = _node->getRender();

            selfRender->renderWithChildren( _renderPipeline, _context, _external );
        }
        //////////////////////////////////////////////////////////////////////////
        bool hasNodeRenderInheritance( const Node * _node )
        {
            if( _node == nullptr )
            {
                return false;
            }

            const RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                return true;
            }

            const Node * parent = _node->getParent();

            bool successful = Helper::hasNodeRenderInheritance( parent );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        RenderInterface * getNodeRenderInheritance( Node * _node, Node ** const _nodeRender )
        {
            if( _node == nullptr )
            {
                return nullptr;
            }

            RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                *_nodeRender = _node;

                return render;
            }

            Node * parent = _node->getParent();

            RenderInterface * render_parent = Helper::getNodeRenderInheritance( parent, _nodeRender );

            return render_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderOrderInterfacePtr & getRenderOrederInheritance( const RenderInterface * _render )
        {
            const RenderOrderInterfacePtr & order = _render->getRenderOrder();

            if( order != nullptr )
            {
                return order;
            }

            const RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return RenderOrderInterfacePtr::none();
            }

            const RenderOrderInterfacePtr & relation_order = Helper::getRenderOrederInheritance( relationRender );

            return relation_order;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderViewportInterfacePtr & getRenderViewportInheritance( const RenderInterface * _render )
        {
            const RenderViewportInterfacePtr & viewport = _render->getRenderViewport();

            if( viewport != nullptr )
            {
                return viewport;
            }

            const RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return RenderViewportInterfacePtr::none();
            }

            const RenderViewportInterfacePtr & relation_viewport = Helper::getRenderViewportInheritance( relationRender );

            return relation_viewport;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderCameraInterfacePtr & getRenderCameraInheritance( const RenderInterface * _render )
        {
            const RenderCameraInterfacePtr & camera = _render->getRenderCamera();

            if( camera != nullptr )
            {
                return camera;
            }

            RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return RenderCameraInterfacePtr::none();
            }

            const RenderCameraInterfacePtr & relation_camera = Helper::getRenderCameraInheritance( relationRender );

            return relation_camera;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTransformationInterfacePtr & getRenderTransformationInheritance( const RenderInterface * _render )
        {
            const RenderTransformationInterfacePtr & transformation = _render->getRenderTransformation();

            if( transformation != nullptr )
            {
                return transformation;
            }

            RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return RenderTransformationInterfacePtr::none();
            }

            const RenderTransformationInterfacePtr & relation_transformation = Helper::getRenderTransformationInheritance( relationRender );

            return relation_transformation;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderScissorInterfacePtr & getRenderScissorInheritance( const RenderInterface * _render )
        {
            const RenderScissorInterfacePtr & scissor = _render->getRenderScissor();

            if( scissor != nullptr )
            {
                return scissor;
            }

            RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return RenderScissorInterfacePtr::none();
            }

            const RenderScissorInterfacePtr & relation_scissor = Helper::getRenderScissorInheritance( relationRender );

            return relation_scissor;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTargetInterfacePtr & getRenderTargetInheritance( const RenderInterface * _render )
        {
            const RenderTargetInterfacePtr & target = _render->getRenderTarget();

            if( target != nullptr )
            {
                return target;
            }

            RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return RenderTargetInterfacePtr::none();
            }

            const RenderTargetInterfacePtr & relation_target = Helper::getRenderTargetInheritance( relationRender );

            return relation_target;
        }
        //////////////////////////////////////////////////////////////////////////
        void fillNodeRenderContext( const NodePtr & _node, RenderContext * const _context )
        {
            const Node * node_ptr = _node.get();

            const RenderOrderInterfacePtr & order = Helper::getNodeRenderOrderInheritance( node_ptr );

            if( order != nullptr )
            {
                _context->order = order.get();
            }

            const RenderViewportInterfacePtr & viewport = Helper::getNodeRenderViewportInheritance( node_ptr );

            if( viewport != nullptr )
            {
                _context->viewport = viewport.get();
            }

            const RenderCameraInterfacePtr & camera = Helper::getNodeRenderCameraInheritance( node_ptr );

            if( camera != nullptr )
            {
                _context->camera = camera.get();
            }

            const RenderTransformationInterfacePtr & transformation = Helper::getNodeRenderTransformationInheritance( node_ptr );

            if( transformation != nullptr )
            {
                _context->transformation = transformation.get();
            }

            const RenderScissorInterfacePtr & scissor = Helper::getNodeRenderScissorInheritance( node_ptr );

            if( camera != nullptr )
            {
                _context->scissor = scissor.get();
            }

            const RenderTargetInterfacePtr & target = Helper::getNodeRenderTargetInheritance( node_ptr );

            if( camera != nullptr )
            {
                _context->target = target.get();
            }
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderOrderInterfacePtr & getNodeRenderOrderInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderOrderInterfacePtr & order = Helper::getRenderOrederInheritance( render );

                return order;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return RenderOrderInterfacePtr::none();
            }

            const RenderOrderInterfacePtr & order_parent = Helper::getNodeRenderOrderInheritance( parent );

            return order_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderViewportInterfacePtr & getNodeRenderViewportInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

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
        const RenderCameraInterfacePtr & getNodeRenderCameraInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

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
        const RenderTransformationInterfacePtr & getNodeRenderTransformationInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

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
        const RenderScissorInterfacePtr & getNodeRenderScissorInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

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
        const RenderTargetInterfacePtr & getNodeRenderTargetInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

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
        //////////////////////////////////////////////////////////////////////////
    }
}