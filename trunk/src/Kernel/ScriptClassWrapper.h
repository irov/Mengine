#	pragma once

#	include "Interface/ScriptSystemInterface.h"
#	include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

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
                return nullptr;
            }

            PyObject * py_obj = _node->getEmbed();

			pybind::incref( py_obj );

			return py_obj;
        }
    };

    template<class T>
	class ScriptClassWrapper
		: public ScriptClassInterface
	{
	public:
		ScriptClassWrapper()
			: m_serviceProvider( nullptr )
		{
		}

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override
		{
			m_serviceProvider = _serviceProvider;
		}

		ServiceProviderInterface * getServiceProvider() override
		{
			return m_serviceProvider;
		}

	public:
		bool initialize() override
		{
			pybind::registration_type_cast<T>(new ScriptClassExtract<T>());

			return true;
		}

		void finalize() override
		{

		}

	protected:
		PyObject * wrap( Scriptable * _node ) override
		{
#   ifdef _DEBUG
			if( dynamic_cast<T *>( _node ) == nullptr )
            {
				LOGGER_ERROR( m_serviceProvider )("ScriptClassWrapper::wrap invalid type"
					);

                return nullptr;
            }
#   endif

            T * obj = static_cast<T *>( _node );

			const pybind::class_type_scope_ptr & scope = pybind::detail::class_scope<T>();

			PyObject * py_obj = scope->create_holder( (void *)obj );

			//pybind::set_attr( py_embedded, "Menge_name", pybind::ptr(_node->getName()) );
			//pybind::set_attr( py_embedded, "Menge_type", pybind::ptr(_node->getType()) );
			//pybind::set_attr( py_embedded, "Menge_tag", pybind::ptr(_node->getTag()) );

			return py_obj;
		}

	protected:
		void destroy() override
		{
			delete this;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;
	};
}

# define SCRIPT_CLASS_WRAPPING( serviceProvider, Class )\
    SCRIPT_SERVICE(serviceProvider)->addWrapping( Helper::stringizeString(serviceProvider, #Class), new ScriptClassWrapper<Class>() )
