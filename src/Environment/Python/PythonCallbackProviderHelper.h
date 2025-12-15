#pragma once

#include "Interface/FactoryInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Pointer.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            class PythonCallbackProviderGenerator
            {
            public:
                PythonCallbackProviderGenerator( const FactoryInterfacePtr & _factory, const pybind::object & _cb, const pybind::args & _args, const DocumentInterfacePtr & _doc );
                ~PythonCallbackProviderGenerator();

            public:
                template<class T>
                operator T () const
                {
                    if( m_cb.is_none() == true )
                    {
                        return nullptr;
                    }

                    T cb = m_factory->createObject( m_doc );

                    MENGINE_ASSERTION_MEMORY_PANIC( cb, "can't create PythonCallbackProvider" );

                    cb->initialize( m_cb, m_args );

                    return cb;
                }

            protected:
                FactoryInterfacePtr m_factory;

                pybind::object m_cb;
                pybind::args m_args;

                DocumentInterfacePtr m_doc;
            };
        }
        //////////////////////////////////////////////////////////////////////////
        Detail::PythonCallbackProviderGenerator makePythonCallbackProvider( const FactoryInterfacePtr & _factory, const pybind::object & _cb, const pybind::args & _args, const DocumentInterfacePtr & _doc );
        //////////////////////////////////////////////////////////////////////////
    }
}
