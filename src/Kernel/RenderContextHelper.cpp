#include "RenderContextHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool equalRenderContext( const RenderContext * _left, const RenderContext * _right )
        {
            if( _left->zGroup != _right->zGroup )
            {
                return false;
            }

            if( _left->zIndex != _right->zIndex )
            {
                return false;
            }

            if( _left->resolution != _right->resolution )
            {
                return false;
            }

            if( _left->viewport != _right->viewport )
            {
                return false;
            }

            if( _left->camera != _right->camera )
            {
                return false;
            }

            if( _left->transformation != _right->transformation )
            {
                return false;
            }

            if( _left->scissor != _right->scissor )
            {
                return false;
            }

            if( _left->target != _right->target )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void clearRenderContext( RenderContext * const _renderContext )
        {
            _renderContext->resolution = nullptr;
            _renderContext->viewport = nullptr;
            _renderContext->camera = nullptr;
            _renderContext->transformation = nullptr;
            _renderContext->scissor = nullptr;
            _renderContext->target = nullptr;
            _renderContext->zGroup = MENGINE_RENDER_ZGROUP_DEFAULT;
            _renderContext->zIndex = MENGINE_RENDER_ZINDEX_DEFAULT;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}