#pragma once

#include "Interface/ScriptSystemInterface.h"
#include "Interface/StringizeInterface.h"

#include "Core/ServantBase.h"

#include "Logger/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        struct ScriptExtract
            : public pybind::interface_<T>::extract_type_ptr
        {
            PyObject * wrap( pybind::kernel_interface * _kernel, typename pybind::interface_<T>::extract_type_ptr::TCastRef _self ) override
            {
                (void)_kernel;

                if( _self == nullptr )
                {
                    return pybind::ret_none();
                }

                pybind::bindable * bindable = static_cast<pybind::bindable *>(_self);

                PyObject * py_obj = bindable->getEmbed();

                return py_obj;
            }
        };
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T>
	class ScriptWrapper
		: public ServantBase<ScriptWrapperInterface>
	{
	public:
		bool initialize() override
		{
			pybind::kernel_interface * kernel = pybind::get_kernel();

			pybind::registration_type_cast<T>(kernel, new Helper::ScriptExtract<T>());

			return true;
		}

		void finalize() override
		{

		}

	protected:
        PyObject * wrap( Scriptable * _scriptable ) override
		{
#   ifdef _DEBUG
			if( dynamic_cast<T *>( _scriptable ) == nullptr )
            {
				LOGGER_ERROR("ScriptClassWrapper::wrap invalid type"
					);

                throw;
            }
#   endif

            T * obj = static_cast<T *>( _scriptable );

			pybind::kernel_interface * kernel = pybind::get_kernel();

            PyObject * py_obj = kernel->scope_create_holder_t( obj );

			//pybind::set_attr( py_embedded, "Mengine_name", pybind::ptr(_node->getName()) );
			//pybind::set_attr( py_embedded, "Mengine_type", pybind::ptr(_node->getType()) );
			//pybind::set_attr( py_embedded, "Mengine_tag", pybind::ptr(_node->getTag()) );

			return py_obj;
		}

	protected:
		void destroy() override
		{
			delete this;
		}
	};
}
