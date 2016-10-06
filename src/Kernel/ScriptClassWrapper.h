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
		PyObject * wrap( typename pybind::interface_<T>::extract_type_ptr::TCastRef _self ) override
        {
			if( _self == nullptr )
			{
				return pybind::ret_none();
			}

#   ifdef MENGINE_SCRIPTABLE
			pybind::bindable * bindable = static_cast<pybind::bindable *>(_self);

            PyObject * py_obj = bindable->getEmbed();

            return py_obj;
#   else
            return pybind::ret_none();
#   endif			
        }
    };

    template<class T>
	class ClassScriptWrapper
		: public ScriptWrapperInterface
	{
	public:
		ClassScriptWrapper()
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
        ScriptObject * wrap( Scriptable * _scriptable ) override
		{
#   ifdef _DEBUG
			if( dynamic_cast<T *>( _scriptable ) == nullptr )
            {
				LOGGER_ERROR( m_serviceProvider )("ScriptClassWrapper::wrap invalid type"
					);

                return nullptr;
            }
#   endif

            T * obj = static_cast<T *>( _scriptable );

			pybind::kernel_interface * kernel = pybind::get_kernel();

			const pybind::class_type_scope_ptr & scope = kernel->class_scope<T>();

            ScriptObject * py_obj = (ScriptObject *)scope->create_holder( (void *)obj );

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
