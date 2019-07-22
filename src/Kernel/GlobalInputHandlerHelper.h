#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( const InputKeyEvent & )> LambdaInputKeyEvent;
        typedef Lambda<void( const InputMouseButtonEvent & )> LambdaInputMouseButtonEvent;
        typedef Lambda<void( const InputMouseMoveEvent & )> LambdaInputMouseMoveEvent;
        //////////////////////////////////////////////////////////////////////////
        uint32_t addGlobalKeyHandler( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _event, const Char * _doc );        
        uint32_t addGlobalMouseButtonEvent( MouseCode _code, bool _isDown, const LambdaInputMouseButtonEvent & _event, const Char * _doc );
        uint32_t addGlobalMouseMoveEvent( const LambdaInputMouseMoveEvent & _event, const Char * _doc );
        //////////////////////////////////////////////////////////////////////////
        void removeGlobalHandler( uint32_t _id );
        //////////////////////////////////////////////////////////////////////////
    }
}