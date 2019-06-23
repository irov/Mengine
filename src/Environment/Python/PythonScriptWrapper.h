#pragma once

#include "Interface/ScriptServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Scriptable.h"
#include "Kernel/ScriptWrapperInterface.h"

#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        struct ScriptExtract
            : public pybind::interface_<T>::extract_type_ptr
        {
            PyObject * wrap( pybind::kernel_interface * _kernel, typename pybind::interface_<T>::extract_type_ptr::TCastRef _self ) override
            {
                if( _self == nullptr )
                {
                    return _kernel->ret_none();
                }

                Scriptable * bindable = static_cast<Scriptable *>(_self);

                PyObject * py_obj = bindable->getEmbed( _kernel );

                return py_obj;
            }
        };
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class PythonScriptWrapper
        : public ScriptWrapperInterface
        , public Factorable
    {
    public:
        PythonScriptWrapper( pybind::kernel_interface * _kernel )
            : m_kernel( _kernel )
        {
            pybind::type_cast_ptr cast( new Helper::ScriptExtract<T> );
            pybind::registration_type_cast<T>(m_kernel, cast);
        }

        ~PythonScriptWrapper() override
        {
            pybind::unregistration_type_cast<T>(m_kernel);
        }

    protected:
        PyObject * wrap( Scriptable * _scriptable ) override
        {
#ifdef MENGINE_DEBUG
            if( dynamic_cast<T *>(_scriptable) == nullptr )
            {
                LOGGER_ERROR( "invalid type"
                );

                throw;
            }
#endif

            T * obj = static_cast<T *>(_scriptable);

            PyObject * py_obj = m_kernel->scope_create_holder_t( obj );

            //pybind::set_attr( py_embedded, "Mengine_name", pybind::ptr(_node->getName()) );
            //pybind::set_attr( py_embedded, "Mengine_type", pybind::ptr(_node->getType()) );
            //pybind::set_attr( py_embedded, "Mengine_tag", pybind::ptr(_node->getTag()) );

            return py_obj;
        }

    protected:
        pybind::kernel_interface * m_kernel;
    };
}
