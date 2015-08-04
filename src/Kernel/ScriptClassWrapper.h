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
        PyObject * wrap( pybind::kernel_interface * _kernel, typename pybind::interface_<T>::extract_type_ptr::TCastRef _node ) override
        {
			(void)_kernel;

            if( _node == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::object py_obj = _node->getEmbed();

			PyObject * py_obj_ptr = py_obj.ptr();

			pybind::incref( py_obj_ptr );

			return py_obj_ptr;
        }
    };

    template<class T>
	class ScriptClassWrapper
		: public ScriptClassInterface
	{
	public:
		ScriptClassWrapper()
			: m_serviceProvider( nullptr )
			, m_kernel( nullptr )
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
		bool initialize( pybind::kernel_interface * _kernel ) override
		{
			m_kernel = _kernel;

			pybind::registration_type_cast<T>(m_kernel, new ScriptClassExtract<T>());

			return true;
		}

		void finalize() override
		{

		}

	protected:
		pybind::object wrap( Scriptable * _node ) override
		{
#   ifdef _DEBUG
			if( dynamic_cast<T *>( _node ) == nullptr )
            {
				LOGGER_ERROR( m_serviceProvider )("ScriptClassWrapper::wrap invalid type"
					);

                return pybind::make_invalid_object_t();
            }
#   endif

            T * obj = static_cast<T *>( _node );

			const pybind::class_type_scope_interface_ptr & scope = m_kernel->class_scope<T>();

			PyObject * py_obj = scope->create_holder( (void *)obj );

			//pybind::set_attr( py_embedded, "Menge_name", pybind::ptr(_node->getName()) );
			//pybind::set_attr( py_embedded, "Menge_type", pybind::ptr(_node->getType()) );
			//pybind::set_attr( py_embedded, "Menge_tag", pybind::ptr(_node->getTag()) );

			return pybind::object( py_obj );
		}

	protected:
		void destroy() override
		{
			delete this;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;

		pybind::kernel_interface * m_kernel;
	};
}

# define SCRIPT_CLASS_WRAPPING( serviceProvider, Class )\
    SCRIPT_SERVICE(serviceProvider)->addWrapping( Helper::stringizeString(serviceProvider, #Class), new ScriptClassWrapper<Class>() )
