#	include "Scriptable.h"

#	include "ScriptEngine.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scriptable::Scriptable()
	: m_scriptable(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	Scriptable::~Scriptable()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::setScript( PyObject * _scriptable )
	{
		m_scriptable = _scriptable;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Scriptable::getScript()
	{
		if( m_scriptable == 0 )
		{
			m_scriptable = Holder<ScriptEngine>::hostage()
				->wrapp( this );

			if( m_scriptable == 0 )
			{
				return 0;
			}
		}

		return m_scriptable;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::callMethod( const std::string & _method, const char * _format, ... )
	{
		PyObject * module = getScript();

		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( module, _method ) )
		{
			PyObject * function = Holder<ScriptEngine>::hostage()
				->getModuleFunction( module, _method );

			va_list valist;
			va_start(valist, _format);

			PyObject * result = 
				Holder<ScriptEngine>::hostage()
				->callFunction( function, _format, valist );

			va_end( valist );

			if( result == 0 )
			{
				return;
			}

			if( pybind::convert::is_none( result ) == false )
			{
				printf("Warning: Method '%s' don't have return any value\n"
					, _method.c_str() 
					);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scriptable::isScriptable() const
	{
		return m_scriptable != 0;
	}
}