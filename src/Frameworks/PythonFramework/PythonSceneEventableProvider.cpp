#include "PythonSceneEventableProvider.h"

#include "PythonEventableProviderHelper.h"

#include "PythonSceneEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    EventablePtr PythonSceneEventableProvider::createEventable( pybind::kernel_interface * _kernel, const pybind::object & _type, const FactoryInterfacePtr & _factory, const DocumentInterfacePtr & _doc )
    {
        if( Helper::initializePythonEventableType( _kernel, _type ) == false )
        {
            return nullptr;
        }

        EntityEventablePtr eventable = _factory->createObject( _doc );

        Helper::registerPythonEntityEventMethods<PythonSceneEventReceiver>( _kernel, _type, eventable, _doc );

        Helper::registerPythonEventReceiverMethod<PythonSceneEventReceiver>( _kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onAppMouseLeave" ), EVENT_SCENE_APP_MOUSE_LEAVE, _doc );
        Helper::registerPythonEventReceiverMethod<PythonSceneEventReceiver>( _kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onAppMouseEnter" ), EVENT_SCENE_APP_MOUSE_ENTER, _doc );
        Helper::registerPythonEventReceiverMethod<PythonSceneEventReceiver>( _kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onFocus" ), EVENT_SCENE_FOCUS, _doc );
        Helper::registerPythonEventReceiverMethod<PythonSceneEventReceiver>( _kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onRestartBegin" ), EVENT_SCENE_RESTART_BEGIN, _doc );
        Helper::registerPythonEventReceiverMethod<PythonSceneEventReceiver>( _kernel, _type, eventable, STRINGIZE_STRING_LOCAL( "onRestartEnd" ), EVENT_SCENE_RESTART_END, _doc );

        return eventable;
    }
    //////////////////////////////////////////////////////////////////////////
}