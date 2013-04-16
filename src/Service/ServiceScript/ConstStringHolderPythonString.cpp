#   include "ConstStringHolderPythonString.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderPythonString::ConstStringHolderPythonString()
        : m_value(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderPythonString::~ConstStringHolderPythonString()
    {
        if( m_value )
        {
            pybind::decref( m_value );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderPythonString::setValue( PyObject * _value )
    {        
        m_value = _value;

        pybind::incref( m_value );

        const char * data = pybind::string_to_char( m_value );
        size_t size = pybind::string_size( m_value );

        this->setup( data, size );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderPythonString::releaseString()
    {
        pybind::decref( m_value );
        m_value = NULL;
    }
}