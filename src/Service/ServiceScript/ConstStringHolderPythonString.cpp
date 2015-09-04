#   include "ConstStringHolderPythonString.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    ConstStringHolderPythonString::ConstStringHolderPythonString()
        : m_value(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderPythonString::setPythonObject( PyObject * _value )
    {   
		pybind::decref( m_value );
        m_value = _value;
        pybind::incref( m_value );

        const char * data = pybind::string_to_char( m_value );
        size_t size = pybind::string_size( m_value );
		stdex::const_string_holder::hash_type hash = pybind::object_hash( m_value );
				
        this->setup( data, size, hash, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderPythonString::_releaseString()
    {
        pybind::decref( m_value );
        m_value = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConstStringHolderPythonString::_destroyString()
    {
		pybind::decref( m_value );
		m_value = nullptr;

        this->destroy();
    }
}