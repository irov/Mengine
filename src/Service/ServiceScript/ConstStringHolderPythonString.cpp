#include "ConstStringHolderPythonString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderPythonString::ConstStringHolderPythonString()
        : m_value(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderPythonString::~ConstStringHolderPythonString()
    {
        if( m_value != nullptr )
        {
            pybind::decref( m_value );
            m_value = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderPythonString::setPythonObject( PyObject * _value )
    {   
		pybind::decref( m_value );
        m_value = _value;
        pybind::incref( m_value );

        const char * data = pybind::string_to_char( m_value );
        size_t size = pybind::string_size( m_value );
        ConstString::hash_type hash = pybind::string_hash( m_value );
		
        this->setup( data, size, hash );
    }
}