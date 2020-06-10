#include "ViewportHelper.h"

#include "math/config.h"

namespace Mengine
{
    namespace Helper
    {
        bool findBestAspectViewport( const MapAspectRatioViewports & _aspectRatioViewports, float _aspect, float * const _bestAspect, Viewport * const _viewport )
        {
            if( _aspectRatioViewports.empty() == true )
            {
                return false;
            }

            float minimal_aspect = 100.f;

            for( auto && [aspect, viewport] : _aspectRatioViewports )
            {
                float deltha_aspect = MT_fabsf( _aspect - aspect );

                if( deltha_aspect < minimal_aspect )
                {
                    minimal_aspect = deltha_aspect;

                    *_bestAspect = aspect;
                    *_viewport = viewport;
                }
            }

            return true;
        }
    }
}

