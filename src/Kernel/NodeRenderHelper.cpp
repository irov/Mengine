#include "NodeRenderHelper.h"

#include "Kernel/Node.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void nodeRenderChildren( const NodePtr & _node, const RenderContext * _context )
        {
            RenderInterfacePtr render = _node->getRender();
            
            if( render != nullptr )
            {
                render->render( _context );
            }

            _node->foreachChildren( [_context]( const NodePtr & _child )
            {
                Helper::nodeRenderChildren( _child, _context );
            } );
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