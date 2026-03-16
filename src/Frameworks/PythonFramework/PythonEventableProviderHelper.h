#pragma once

#include "Interface/FactoryInterface.h"

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/EntityEventable.h"
#include "Kernel/EntityEventReceiverInterface.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool initializePythonEventableType( pybind::kernel_interface * _kernel, const pybind::object & _type );
        //////////////////////////////////////////////////////////////////////////
        template<class TReceiver>
        static void registerPythonEntityEventMethods( pybind::kernel_interface * _kernel, const pybind::object & _type, const EventablePtr & _eventable, const DocumentInterfacePtr & _doc )
        {
            Helper::registerPythonEventReceiverMethod<TReceiver>( _kernel, _type, _eventable, STRINGIZE_STRING_LOCAL( "onCreate" ), EVENT_ENTITY_CREATE, _doc );
            Helper::registerPythonEventReceiverMethod<TReceiver>( _kernel, _type, _eventable, STRINGIZE_STRING_LOCAL( "onDestroy" ), EVENT_ENTITY_DESTROY, _doc );
            Helper::registerPythonEventReceiverMethod<TReceiver>( _kernel, _type, _eventable, STRINGIZE_STRING_LOCAL( "onPreparation" ), EVENT_ENTITY_PREPARATION, _doc );
            Helper::registerPythonEventReceiverMethod<TReceiver>( _kernel, _type, _eventable, STRINGIZE_STRING_LOCAL( "onActivate" ), EVENT_ENTITY_ACTIVATE, _doc );
            Helper::registerPythonEventReceiverMethod<TReceiver>( _kernel, _type, _eventable, STRINGIZE_STRING_LOCAL( "onPassivate" ), EVENT_ENTITY_PASSIVATE, _doc );
            Helper::registerPythonEventReceiverMethod<TReceiver>( _kernel, _type, _eventable, STRINGIZE_STRING_LOCAL( "onDeactivate" ), EVENT_ENTITY_DEACTIVATE, _doc );
            Helper::registerPythonEventReceiverMethod<TReceiver>( _kernel, _type, _eventable, STRINGIZE_STRING_LOCAL( "onCompile" ), EVENT_ENTITY_COMPILE, _doc );
            Helper::registerPythonEventReceiverMethod<TReceiver>( _kernel, _type, _eventable, STRINGIZE_STRING_LOCAL( "onRelease" ), EVENT_ENTITY_RELEASE, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}