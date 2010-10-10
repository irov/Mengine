#	include "Scriptable.h"

#	include "ScriptEngine.h"
#	include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

#	include "Node.h"

#	include <assert.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scriptable::Scriptable()
		: m_embed(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable::~Scriptable()
	{
		if( m_embed )
		{
			pybind::unwrap( m_embed );
			pybind::decref( m_embed );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::setEmbed( PyObject * _embedding )
	{
		if( _embedding == 0 )
		{
			return;
		}

		m_embed = _embedding;

		this->_embedding( m_embed );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Scriptable::getEmbed()
	{
		if( m_embed == 0 )
		{
			m_embed = _embedded();

			if( m_embed == 0 )
			{
				return 0;
			}

			this->_embedding( m_embed );
		}
		//else
		{
			pybind::incref( m_embed );
		}

		return m_embed;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::callMethod( const char * _method, const char * _format, ... )
	{
		PyObject * _embedding = getEmbed();

		ScriptEngine * scriptEngine = 
			ScriptEngine::get();

		if( scriptEngine->hasModuleFunction( _embedding, _method ) == true )
		{
			PyObject * function = scriptEngine->getModuleFunction( _embedding, _method );

			va_list valist;
			va_start(valist, _format);

			scriptEngine->callFunction( function, _format, valist );

			va_end( valist );

			pybind::decref( function );
		}

		pybind::decref( _embedding );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::_embedding( PyObject * _embed )
	{
		//Empty
	}
}
