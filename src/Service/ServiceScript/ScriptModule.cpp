#	include "ScriptModule.h"

#	include "Kernel/Eventable.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptModule::ScriptModule()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModule::initialize( const pybind::module & _module )
	{
		m_module = _module;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModule::onInitialize( const ConstString & _method )
	{
		if( m_module.has_attr( _method ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("ScriptModule::onInitialize invalid has initializer %s"
				, _method.c_str()
				);

			return false;
		}

		pybind::object module_function = m_module.get_attr( _method );

		pybind::object py_result = module_function.call();

		if( py_result.is_invalid() == true )
		{
			LOGGER_ERROR( m_serviceProvider )("ScriptModule::onInitialize invalid call initializer %s"
				, _method.c_str()
				);

			return false;
		}

		if( py_result.is_bool() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("ScriptModule::onInitialize invalid call initializer %s need return bool [True|False] but return is '%s'"
				, _method.c_str()
				, py_result.repr()
				);

			return false;
		}

		bool successful = py_result.extract();
		
		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModule::onFinalize( const ConstString & _method )
	{
		if( m_module.has_attr( _method ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("ScriptModule::onFinalize invalid has finalizer %s"
				, _method.c_str()
				);

			return false;
		}

		pybind::object module_function = m_module.get_attr( _method );

		module_function.call();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptModule::registerEventMethod( Eventable * _event, uint32_t _id, const char * _method )
	{
		bool result = _event->registerEventMethod( _id, _method, m_module );

		return result;
	}
}