#include "ResolutionHelper.h"

#include "Interface/ApplicationInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void adaptScreenPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint )
        {
            const Viewport & renderViewport = APPLICATION_SERVICE()
                ->getRenderViewport();

            const Resolution & currentResolution = APPLICATION_SERVICE()
                ->getCurrentWindowResolution();

            mt::vec2f renderViewportSize;
            renderViewport.calcSize( &renderViewportSize );

            mt::vec2f currentResolutionSize;
            currentResolution.calcSize( &currentResolutionSize );

            mt::vec2f currentResolutionSizeInv = 1.f / currentResolutionSize;

            mt::vec2f windowScale = renderViewportSize * currentResolutionSizeInv;
            mt::vec2f windowOffset = renderViewport.begin * currentResolutionSizeInv;

            *_adaptScreenPoint = (_screenPoint - windowOffset) / windowScale;
        }
        //////////////////////////////////////////////////////////////////////////
        void adaptScreenDeltha( const mt::vec2f & _screenDeltha, mt::vec2f * const _adaptScreenDeltha )
        {
            const Viewport & renderViewport = APPLICATION_SERVICE()
                ->getRenderViewport();

            const Resolution & currentResolution = APPLICATION_SERVICE()
                ->getCurrentWindowResolution();

            mt::vec2f renderViewportSize;
            renderViewport.calcSize( &renderViewportSize );

            mt::vec2f currentResolutionSize;
            currentResolution.calcSize( &currentResolutionSize );

            mt::vec2f windowScale = renderViewportSize / currentResolutionSize;

            *_adaptScreenDeltha = _screenDeltha / windowScale;
        }
        //////////////////////////////////////////////////////////////////////////
        void adaptWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _adaptScreenPoint )
        {
            const Viewport & renderViewport = APPLICATION_SERVICE()
                ->getRenderViewport();

            const Resolution & currentResolution = APPLICATION_SERVICE()
                ->getCurrentWindowResolution();

            mt::vec2f renderViewportSize;
            renderViewport.calcSize( &renderViewportSize );

            mt::vec2f currentResolutionSize;
            currentResolution.calcSize( &currentResolutionSize );

            mt::vec2f currentResolutionSizeInv = 1.f / currentResolutionSize;

            mt::vec2f windowScale = renderViewportSize * currentResolutionSizeInv;
            mt::vec2f windowOffset = renderViewport.begin * currentResolutionSizeInv;

            *_adaptScreenPoint = _screenPoint * windowScale + windowOffset;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
