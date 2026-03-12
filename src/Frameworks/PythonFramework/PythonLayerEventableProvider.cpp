#include "PythonLayerEventableProvider.h"

#include "PythonEventableProviderHelper.h"

#include "PythonLayerEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    EventablePtr PythonLayerEventableProvider::createEventable( pybind::kernel_interface * _kernel, const pybind::object & _type, const FactoryInterfacePtr & _factory, const DocumentInterfacePtr & _doc )
    {
        if( Helper::initializePythonEventableType( _kernel, _type ) == false )
        {
            return nullptr;
        }

        EntityEventablePtr eventable = _factory->createObject( _doc );

        Helper::registerPythonEntityEventMethods<PythonLayerEventReceiver>( _kernel, _type, eventable, _doc );

        return eventable;
    }
    //////////////////////////////////////////////////////////////////////////
}