#pragma once

#include "Interface/InputHandlerInterface.h"
#include "Interface/DocumentInterface.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( const InputKeyEvent & )> LambdaInputKeyEvent;
        typedef Lambda<void( const InputMouseButtonEvent & )> LambdaInputMouseButtonEvent;
        typedef Lambda<void( const InputMouseMoveEvent & )> LambdaInputMouseMoveEvent;
        typedef Lambda<void( const InputMouseWheelEvent & )> LambdaInputMouseWheelEvent;
        //////////////////////////////////////////////////////////////////////////
        UniqueId addGlobalKeyHandler( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _event, const DocumentInterfacePtr & _doc );
        UniqueId addGlobalKeyOnceHandler( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _event, const DocumentInterfacePtr & _doc );
        UniqueId addGlobalMouseButtonEvent( EMouseButtonCode _button, bool _isDown, const LambdaInputMouseButtonEvent & _event, const DocumentInterfacePtr & _doc );
        UniqueId addGlobalMouseMoveEvent( const LambdaInputMouseMoveEvent & _event, const DocumentInterfacePtr & _doc );
        UniqueId addGlobalMouseWheelEvent( const LambdaInputMouseWheelEvent & _event, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        void removeGlobalHandler( UniqueId _id );
        //////////////////////////////////////////////////////////////////////////
    }
}