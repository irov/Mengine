#include "NodeScreenPosition.h"

#include "Interface/PlayerServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/NodeRenderHierarchy.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void getNodeScreenPosition( const Node * _node, const mt::mat4f & _offsetMatrix, mt::vec2f * const _screenPosition )
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

            mt::mat4f wm;
            transformation->getWorldMatrixOffset( _offsetMatrix, &wm );

            mt::vec2f camera_position;
            camera->fromWorldToScreenPosition( wm, &camera_position );

            const RenderViewportInterface * viewport = Helper::getNodeRenderViewportInheritance( _node );

            if( viewport == nullptr )
            {
                const RenderViewportInterfacePtr & player_viewport = PLAYER_SERVICE()
                    ->getRenderViewport();

                viewport = player_viewport.get();
            }

            mt::vec2f contentPosition;
            viewport->fromCameraToContentPosition( camera_position, &contentPosition );

            const RenderResolutionInterface * resolution = Helper::getNodeRenderResolutionInheritance( _node );

            if( resolution == nullptr )
            {
                const RenderResolutionInterfacePtr & player_resolution = PLAYER_SERVICE()
                    ->getRenderResolution();

                resolution = player_resolution.get();
            }   
            
            resolution->fromContentToScreenPosition( contentPosition, _screenPosition );
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