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
        ZGroupType getRenderZGroupInheritance( const RenderInterface * _render )
        {
            ZGroupType zOrder = _render->getZGroup();

            if( zOrder != MENGINE_RENDER_ZGROUP_DEFAULT )
            {
                return zOrder;
            }

            const RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return MENGINE_RENDER_ZGROUP_DEFAULT;
            }

            ZGroupType relation_zGroup = Helper::getRenderZGroupInheritance( relationRender );

            return relation_zGroup;
        }
        //////////////////////////////////////////////////////////////////////////
        ZIndexType getRenderZIndexInheritance( const RenderInterface * _render )
        {
            ZIndexType zIndex = _render->getZIndex();

            if( zIndex != MENGINE_RENDER_ZINDEX_DEFAULT )
            {
                return zIndex;
            }

            const RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return MENGINE_RENDER_ZINDEX_DEFAULT;
            }

            ZIndexType relation_zIndex = Helper::getRenderZIndexInheritance( relationRender );

            return relation_zIndex;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderViewportInterface * getRenderViewportInheritance( const RenderInterface * _render )
        {
            const RenderViewportInterfacePtr & viewport = _render->getRenderViewport();

            if( viewport != nullptr )
            {
                return viewport.get();
            }

            const RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return nullptr;
            }

            const RenderViewportInterface * relation_viewport = Helper::getRenderViewportInheritance( relationRender );

            return relation_viewport;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderCameraInterface * getRenderCameraInheritance( const RenderInterface * _render )
        {
            const RenderCameraInterfacePtr & camera = _render->getRenderCamera();

            if( camera != nullptr )
            {
                return camera.get();
            }

            RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return nullptr;
            }

            const RenderCameraInterface * relation_camera = Helper::getRenderCameraInheritance( relationRender );

            return relation_camera;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTransformationInterface * getRenderTransformationInheritance( const RenderInterface * _render )
        {
            const RenderTransformationInterfacePtr & transformation = _render->getRenderTransformation();

            if( transformation != nullptr )
            {
                return transformation.get();
            }

            RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return nullptr;
            }

            const RenderTransformationInterface * relation_transformation = Helper::getRenderTransformationInheritance( relationRender );

            return relation_transformation;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderScissorInterface * getRenderScissorInheritance( const RenderInterface * _render )
        {
            const RenderScissorInterfacePtr & scissor = _render->getRenderScissor();

            if( scissor != nullptr )
            {
                return scissor.get();
            }

            RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return nullptr;
            }

            const RenderScissorInterface * relation_scissor = Helper::getRenderScissorInheritance( relationRender );

            return relation_scissor;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTargetInterface * getRenderTargetInheritance( const RenderInterface * _render )
        {
            const RenderTargetInterfacePtr & target = _render->getRenderTarget();

            if( target != nullptr )
            {
                return target.get();
            }

            RenderInterface * relationRender = _render->getTotalRelationRender();

            if( relationRender == nullptr )
            {
                return nullptr;
            }

            const RenderTargetInterface * relation_target = Helper::getRenderTargetInheritance( relationRender );

            return relation_target;
        }
        //////////////////////////////////////////////////////////////////////////
        void fillNodeRenderContextInheritance( const NodePtr & _node, RenderContext * const _context )
        {
            const Node * node_ptr = _node.get();

            ZGroupType zGroup = Helper::getNodeRenderZGroupInheritance( node_ptr );

            if( zGroup != MENGINE_RENDER_ZGROUP_DEFAULT )
            {
                _context->zGroup = zGroup;
            }

            int32_t zIndex = Helper::getNodeRenderZIndexInheritance( node_ptr );

            if( zIndex != MENGINE_RENDER_ZINDEX_DEFAULT )
            {
                _context->zIndex = zIndex;
            }

            const RenderViewportInterface * viewport = Helper::getNodeRenderViewportInheritance( node_ptr );

            if( viewport != nullptr )
            {
                _context->viewport = viewport;
            }

            const RenderCameraInterface * camera = Helper::getNodeRenderCameraInheritance( node_ptr );

            if( camera != nullptr )
            {
                _context->camera = camera;
            }

            const RenderTransformationInterface * transformation = Helper::getNodeRenderTransformationInheritance( node_ptr );

            if( transformation != nullptr )
            {
                _context->transformation = transformation;
            }

            const RenderScissorInterface * scissor = Helper::getNodeRenderScissorInheritance( node_ptr );

            if( camera != nullptr )
            {
                _context->scissor = scissor;
            }

            const RenderTargetInterface * target = Helper::getNodeRenderTargetInheritance( node_ptr );

            if( camera != nullptr )
            {
                _context->target = target;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        ZGroupType getNodeRenderZGroupInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                ZGroupType zGroup = Helper::getRenderZGroupInheritance( render );

                return zGroup;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return MENGINE_RENDER_ZGROUP_DEFAULT;
            }

            ZGroupType order_zGroup = Helper::getNodeRenderZGroupInheritance( parent );

            return order_zGroup;
        }
        //////////////////////////////////////////////////////////////////////////
        ZIndexType getNodeRenderZIndexInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                ZIndexType zIndex = Helper::getRenderZIndexInheritance( render );

                return zIndex;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return MENGINE_RENDER_ZINDEX_DEFAULT;
            }

            ZIndexType order_zIndex = Helper::getNodeRenderZIndexInheritance( parent );

            return order_zIndex;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderViewportInterface * getNodeRenderViewportInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderViewportInterface * viewport = Helper::getRenderViewportInheritance( render );

                return viewport;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return nullptr;
            }

            const RenderViewportInterface * viewport_parent = Helper::getNodeRenderViewportInheritance( parent );

            return viewport_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderCameraInterface * getNodeRenderCameraInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderCameraInterface * camera = Helper::getRenderCameraInheritance( render );

                return camera;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return nullptr;
            }

            const RenderCameraInterface * camera_parent = Helper::getNodeRenderCameraInheritance( parent );

            return camera_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTransformationInterface * getNodeRenderTransformationInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderTransformationInterface * transformation = Helper::getRenderTransformationInheritance( render );

                return transformation;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return nullptr;
            }

            const RenderTransformationInterface * camera_parent = Helper::getNodeRenderTransformationInheritance( parent );

            return camera_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderScissorInterface * getNodeRenderScissorInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderScissorInterface * scissor = Helper::getRenderScissorInheritance( render );

                return scissor;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return nullptr;
            }

            const RenderScissorInterface * scissor_parent = Helper::getNodeRenderScissorInheritance( parent );

            return scissor_parent;
        }
        //////////////////////////////////////////////////////////////////////////
        const RenderTargetInterface * getNodeRenderTargetInheritance( const Node * _node )
        {
            const RenderInterface * render = _node->getRender();

            if( render != nullptr )
            {
                const RenderTargetInterface * target = Helper::getRenderTargetInheritance( render );

                return target;
            }

            Node * parent = _node->getParent();

            if( parent == nullptr )
            {
                return nullptr;
            }

            const RenderTargetInterface * target_parent = Helper::getNodeRenderTargetInheritance( parent );

            return target_parent;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}