#include "Win32ResolutionHelper.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void Win32ResolutionToRect( const Resolution & _resolution, RECT * const _rect )
        {
            uint32_t width = _resolution.getWidth();

            MENGINE_ASSERTION_FATAL( width <= static_cast<uint32_t>(MAXLONG), "resolution width %u exceeds RECT coordinate range"
                , width
            );

            uint32_t height = _resolution.getHeight();

            MENGINE_ASSERTION_FATAL( height <= static_cast<uint32_t>(MAXLONG), "resolution height %u exceeds RECT coordinate range"
                , height
            );

            _rect->left = 0;
            _rect->top = 0;
            _rect->right = static_cast<LONG>(width);
            _rect->bottom = static_cast<LONG>(height);
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
