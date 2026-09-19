#include "ViewportHelper.h"

#include "Config/StdMath.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool findBestAspectViewport( const VectorAspectRatioViewports & _aspectRatioViewports, float _aspect, float * const _bestAspect, Viewport * const _viewport )
        {
            if( _aspectRatioViewports.empty() == true )
            {
                return false;
            }

            float minimal_aspect = 100.f;

            for( const AspectRatioViewport & aspectRatioViewport : _aspectRatioViewports )
            {
                float aspect = aspectRatioViewport.width / aspectRatioViewport.height;

                float delta_aspect = StdMath::fabsf( _aspect - aspect );

                if( delta_aspect >= minimal_aspect )
                {
                    continue;
                }

                minimal_aspect = delta_aspect;

                if( _bestAspect != nullptr )
                {
                    *_bestAspect = aspect;
                }

                if( _viewport != nullptr )
                {
                    *_viewport = aspectRatioViewport.viewport;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}

