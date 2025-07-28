#include "ResolutionHelper.h"

#include "Interface/ApplicationInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void adaptScreenPosition( const mt::vec2f & _screenPosition, mt::vec2f * const _adaptScreenPosition )
        {
            const Viewport & renderViewport = APPLICATION_SERVICE()
                ->getRenderViewport();

            const Resolution & currentResolution = APPLICATION_SERVICE()
                ->getCurrentWindowResolution();

            mt::vec2f renderViewportSize;
            renderViewport.calcSize( &renderViewportSize );

            mt::vec2f currentResolutionSizeInv;
            currentResolution.calcSizeInv( &currentResolutionSizeInv );

            mt::vec2f windowScale = renderViewportSize * currentResolutionSizeInv;
            mt::vec2f windowOffset = renderViewport.begin * currentResolutionSizeInv;

            *_adaptScreenPosition = (_screenPosition - windowOffset) / windowScale;
        }
        //////////////////////////////////////////////////////////////////////////
        void adaptScreenDelta( const mt::vec2f & _screenDelta, mt::vec2f * const _adaptScreenDelta )
        {
            const Viewport & renderViewport = APPLICATION_SERVICE()
                ->getRenderViewport();

            const Resolution & currentResolution = APPLICATION_SERVICE()
                ->getCurrentWindowResolution();

            mt::vec2f renderViewportSize;
            renderViewport.calcSize( &renderViewportSize );

            mt::vec2f currentResolutionSizeInv;
            currentResolution.calcSizeInv( &currentResolutionSizeInv );

            mt::vec2f windowScale = renderViewportSize * currentResolutionSizeInv;

            *_adaptScreenDelta = _screenDelta / windowScale;
        }
        //////////////////////////////////////////////////////////////////////////
        void adaptWorldPosition( const mt::vec2f & _screenPosition, mt::vec2f * const _adaptScreenPosition )
        {
            const Viewport & renderViewport = APPLICATION_SERVICE()
                ->getRenderViewport();

            const Resolution & currentResolution = APPLICATION_SERVICE()
                ->getCurrentWindowResolution();

            mt::vec2f renderViewportSize;
            renderViewport.calcSize( &renderViewportSize );

            mt::vec2f currentResolutionSizeInv;
            currentResolution.calcSizeInv( &currentResolutionSizeInv );

            mt::vec2f windowScale = renderViewportSize * currentResolutionSizeInv;
            mt::vec2f windowOffset = renderViewport.begin * currentResolutionSizeInv;

            *_adaptScreenPosition = _screenPosition * windowScale + windowOffset;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
