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
        m_value = _value;

        pybind::incref( m_value );

        const char * data = pybind::string_to_char( m_value );
        size_t size = pybind::string_size( m_value );

        this->setup( data, size );
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
		if( m_value != nullptr )
		{
			pybind::decref( m_value );
		}

        this->destroy();
    }
}