#include "ConstStringHolderPythonString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderPythonString::ConstStringHolderPythonString()
        : m_kernel( nullptr )
        , m_value( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderPythonString::~ConstStringHolderPythonString()
    {
        if( m_kernel != nullptr )
        {
            m_kernel->decref( m_value );
            m_value = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderPythonString::setPythonObject( pybind::kernel_interface * _kernel, PyObject * _value )
    {
        if( m_kernel != nullptr )
        {
            m_kernel->decref( m_value );
        }

        m_kernel = _kernel;
        m_value = _value;

        if( m_kernel != nullptr )
        {
            m_kernel->incref( m_value );

            const char * data = m_kernel->string_to_char( m_value );
            size_t size = m_kernel->string_size( m_value );
            int64_t hash = m_kernel->string_hash( m_value );

            this->setup( data, (ConstStringHolder::size_type)size, (ConstStringHolder::hash_type)hash );
        }
        else
        {
            this->setup( "", 0, 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
