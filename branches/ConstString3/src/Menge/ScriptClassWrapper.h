#	pragma once

#	include "ScriptEngine.h"
#	include "ScriptClassInterface.h"

namespace Menge
{
	template<class T>
	class ScriptClassWrapper
		: public ScriptClassInterface
	{
	public:
		PyObject * wrap( Node * _node ) override
		{
			T * obj = dynamic_cast<T *>( _node );
			PyObject * embedded =  pybind::class_holder<T>( obj );
			return embedded;
		}

	protected:
		struct ClassExtract
			: public pybind::interface_<T>::extract_ptr_type
		{
			PyObject * wrap( T * _node ) override
			{
				if( _node == 0 )
				{
					return 0;
				}

				PyObject * pyObj = _node->getEmbed();

				return pyObj;
			}
		};

		ClassExtract m_extract;
	};

}

# define SCRIPT_CLASS_WRAPPING( Class )\
	ScriptEngine::get()->regWrapping( #Class, new ScriptClassWrapper<Class>() )