#	pragma once

#	include "ScriptEngine.h"
#	include "ScriptClassInterface.h"

namespace Menge
{
	template<class T>
	class ScriptClassWrapper
		: public ScriptClassInterface
	{
	protected:
		PyObject * wrap( Node * _node ) override
		{
			T * obj = dynamic_cast<T *>( _node );
			PyObject * embedded =  pybind::class_holder<T>( obj );
			return embedded;
		}

	protected:
		void destroy() override
		{
			delete this;
		}

	protected:
		struct ClassExtract
			: public pybind::interface_<T>::extract_ptr_type
		{
			PyObject * wrap( T * _node ) override
			{
				if( _node == 0 )
				{
					return pybind::ret_none();
				}

				PyObject * pyObj = _node->getEmbed();

				return pyObj;
			}
		};

		ClassExtract m_extract;
	};
}

# define SCRIPT_CLASS_WRAPPING( Class )\
	do{\
		ConstString c_##Class(#Class);\
		ScriptEngine::get()->addWrapping( c_##Class, new ScriptClassWrapper<Class>() );\
	}while(false)