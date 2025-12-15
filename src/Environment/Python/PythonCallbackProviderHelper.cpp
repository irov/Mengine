#include "PythonCallbackProviderHelper.h"

#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/AssertionMemoryPanic.h"

#include "Config/DynamicCast.h"

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            PythonCallbackProviderGenerator::PythonCallbackProviderGenerator( const FactoryInterfacePtr & _factory, const pybind::object & _cb, const pybind::args & _args, const DocumentInterfacePtr & _doc )
                : m_factory( _factory )
                , m_cb( _cb )
                , m_args( _args )
                , m_doc( _doc )
            {
            }
            //////////////////////////////////////////////////////////////////////////
            PythonCallbackProviderGenerator::~PythonCallbackProviderGenerator()
            {
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        Detail::PythonCallbackProviderGenerator makePythonCallbackProvider( const FactoryInterfacePtr & _factory, const pybind::object & _cb, const pybind::args & _args, const DocumentInterfacePtr & _doc )
        {
            return Detail::PythonCallbackProviderGenerator( _factory, _cb, _args, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
