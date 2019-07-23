#include "PythonScriptModule.h"

#include "Kernel/Eventable.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonScriptModule::PythonScriptModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonScriptModule::~PythonScriptModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptModule::initialize( const pybind::module & _module )
    {
        m_module = _module;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptModule::onInitialize( const ConstString & _method )
    {
#ifdef MENGINE_DEBUG
        if( m_module.has_attr( _method ) == false )
        {
            LOGGER_ERROR( "invalid has initializer '%s'"
                , _method.c_str()
            );

            return false;
        }
#endif

        pybind::object module_function = m_module.get_attr( _method );

        pybind::object py_result = module_function.call();

#ifdef MENGINE_DEBUG
        if( py_result.is_invalid() == true )
        {
            LOGGER_ERROR( "invalid call initializer '%s'"
                , _method.c_str()
            );

            return false;
        }

        if( py_result.is_bool() == false )
        {
            LOGGER_ERROR( "invalid call initializer '%'s need return bool [True|False] but return is '%s'"
                , _method.c_str()
                , py_result.repr()
            );

            return false;
        }
#endif

        bool successful = py_result.extract();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonScriptModule::onFinalize( const ConstString & _method )
    {
#ifdef MENGINE_DEBUG
        if( m_module.has_attr( _method ) == false )
        {
            LOGGER_ERROR( "invalid has finalizer '%s'"
                , _method.c_str()
            );

            return false;
        }
#endif

        pybind::object module_function = m_module.get_attr( _method );

        module_function.call();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const pybind::module & PythonScriptModule::getModule() const
    {
        return m_module;
    }
}