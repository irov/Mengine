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
        typedef Lambda<void( const InputAccelerometerEvent & )> LambdaInputAccelerometerEvent;
        typedef Lambda<void( const InputMouseButtonEvent & )> LambdaInputMouseButtonEvent;
        typedef Lambda<void( const InputMouseMoveEvent & )> LambdaInputMouseMoveEvent;
        typedef Lambda<void( const InputMouseWheelEvent & )> LambdaInputMouseWheelEvent;
        //////////////////////////////////////////////////////////////////////////
        UniqueId addGlobalKeyHandler( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _event, const DocumentInterfacePtr & _doc );
        UniqueId addGlobalKeyOnceHandler( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _event, const DocumentInterfacePtr & _doc );
        UniqueId addGlobalAccelerometerHandler( const LambdaInputAccelerometerEvent & _event, const DocumentInterfacePtr & _doc );
        UniqueId addGlobalMouseButtonHandler( EMouseButtonCode _button, bool _isDown, const LambdaInputMouseButtonEvent & _event, const DocumentInterfacePtr & _doc );
        UniqueId addGlobalMouseMoveHandler( const LambdaInputMouseMoveEvent & _event, const DocumentInterfacePtr & _doc );
        UniqueId addGlobalMouseWheelHandler( const LambdaInputMouseWheelEvent & _event, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        void removeGlobalHandler( UniqueId _id );
        //////////////////////////////////////////////////////////////////////////
    }
}