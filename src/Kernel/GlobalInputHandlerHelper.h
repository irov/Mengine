#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( const InputKeyEvent & )> LambdaInputKeyEvent;
        //////////////////////////////////////////////////////////////////////////
        uint32_t addGlobalKeyHandler( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _event, const Char * _doc );
        void removeGlobalHandler( uint32_t _id );
        //////////////////////////////////////////////////////////////////////////
    }
}