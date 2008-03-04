#	include "Scriptable.h"

#	include "ScriptEngine.h"
#	include "LogEngine.h"

#	include "pybind/pybind.hpp"

#	include "Node.h"

#	include <assert.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scriptable::Scriptable()
	: m_scriptable(0)
	, m_node(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	Scriptable::~Scriptable()
	{}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::setWrapp( Node * _node )
	{
		m_node = _node;
	}
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
				->wrap( /*this*/m_node );

			if( m_scriptable == 0 )
			{
				assert(0);
				return 0;
			}
		}

		pybind::incref( m_scriptable );

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
				MENGE_LOG("Warning: Method '%s' don't have return any value\n"
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