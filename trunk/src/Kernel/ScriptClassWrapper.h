#	pragma once

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "pybind/pybind.hpp"

namespace Menge
{
    template<class T>
    struct ScriptClassExtract
        : public pybind::interface_<T>::extract_type_ptr
    {
        PyObject * wrap( typename pybind::interface_<T>::extract_type_ptr::TCastRef _node ) override
        {
            if( _node == nullptr )
            {
                return pybind::ret_none();
            }

            PyObject * pyObj = _node->getEmbed();

            pybind::incref( pyObj );

            return pyObj;
        }
    };

    template<class T>
	class ScriptClassWrapper
		: public ScriptClassInterface
	{
	public:
		ScriptClassWrapper()
		{
			pybind::registration_type_cast<T>( new ScriptClassExtract<T>() );
		}

	protected:
		PyObject * wrap( Scriptable * _node ) override
		{
#   ifdef _DEBUG
			if( dynamic_cast<T *>( _node ) == nullptr )
            {
                return nullptr;
            }
#   endif

            T * obj = static_cast<T *>( _node );

			PyObject * py_embedded =  pybind::class_holder<T>( obj );

			//pybind::set_attr( py_embedded, "Menge_name", pybind::ptr(_node->getName()) );
			//pybind::set_attr( py_embedded, "Menge_type", pybind::ptr(_node->getType()) );
			//pybind::set_attr( py_embedded, "Menge_tag", pybind::ptr(_node->getTag()) );

			return py_embedded;
		}

	protected:
		void destroy() override
		{
			delete this;
		}
	};
}

# define SCRIPT_CLASS_WRAPPING( serviceProvider, Class )\
    SCRIPT_SERVICE(serviceProvider)->addWrapping( Helper::stringizeString(serviceProvider, #Class), new ScriptClassWrapper<Class>() )
