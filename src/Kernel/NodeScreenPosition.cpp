#include "NodeScreenPosition.h"

#include "Interface/PlayerServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/NodeRenderHierarchy.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void getNodeScreenPosition( const Node * _node, mt::vec2f * const _screenPosition )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _node, "node is nullptr" );

            const RenderCameraInterface * camera = Helper::getNodeRenderCameraInheritance( _node );

            if( camera == nullptr )
            {
                const RenderCameraInterfacePtr & player_camera = PLAYER_SERVICE()
                    ->getRenderCamera();

                camera = player_camera.get();
            }

            const TransformationInterface * transformation = _node->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            mt::vec2f camera_position;
            camera->fromWorldToScreenPosition( wm, &camera_position );

            const RenderViewportInterface * viewport = Helper::getNodeRenderViewportInheritance( _node );

            if( viewport == nullptr )
            {
                const RenderViewportInterfacePtr & player_viewport = PLAYER_SERVICE()
                    ->getRenderViewport();

                viewport = player_viewport.get();
            }

            viewport->fromScreenToViewportPosition( camera_position, _screenPosition );
        }
        //////////////////////////////////////////////////////////////////////////
        void setNodeScreenPosition( Node * _node, const mt::vec2f & _screenPosition, float _deep )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _node, "node is nullptr" );

            const RenderCameraInterface * camera = Helper::getNodeRenderCameraInheritance( _node );

            if( camera == nullptr )
            {
                const RenderCameraInterfacePtr & player_camera = PLAYER_SERVICE()
                    ->getRenderCamera();

                camera = player_camera.get();
            }

            mt::vec3f world;
            camera->fromScreenToWorldPosition( _screenPosition, _deep, &world );

            TransformationInterface * transformation = _node->getTransformation();

            transformation->setWorldPosition( world );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}