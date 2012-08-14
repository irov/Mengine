#	pragma once

#	include "ScriptEngine.h"
#	include "ScriptClassInterface.h"

namespace Menge
{
    template<class T>
    struct ScriptClassExtract
        : public pybind::interface_<T>::extract_type_ptr
    {
        PyObject * wrap( T * _node ) override
        {
            if( _node == 0 )
            {
                return pybind::ret_none();
            }

            PyObject * pyObj = _node->getEmbed();
            pybind::incref(pyObj);

            return pyObj;
        }
    };

    template<class T>
	class ScriptClassWrapper
		: public ScriptClassInterface
	{
	protected:

	public:
		ScriptClassWrapper()
		{
            m_extract = new ScriptClassExtract<T>();

			pybind::registration_type_cast<T>( m_extract );
		}

        ~ScriptClassWrapper()
        {
            delete m_extract;
        }

	protected:
		PyObject * wrap( Node * _node ) override
		{
			T * obj = dynamic_cast<T *>( _node );
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

    protected:
        ScriptClassExtract<T> * m_extract;
	};
}

# define SCRIPT_CLASS_WRAPPING( Class )\
	do{\
		ConstString c_##Class(#Class);\
		ScriptEngine::get()->addWrapping( c_##Class, new ScriptClassWrapper<Class>() );\
	}while(false)