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
		: m_embedding(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable::~Scriptable()
	{
		if( m_embedding )
		{
			pybind::unwrap( m_embedding );
			pybind::decref( m_embedding );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::setEmbedding( PyObject * _embedding )
	{
		m_embedding = _embedding;

		//pybind::incref( m_embedding );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Scriptable::getEmbedding()
	{
		if( m_embedding == 0 )
		{
			m_embedding = _embedded();

			if( m_embedding == 0 )
			{
				return 0;
			}
		}
		//else
		{
			pybind::incref( m_embedding );
		}

		return m_embedding;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scriptable::decrefEmbedding()
	{
		bool destroy = ( m_embedding && m_embedding->ob_refcnt == 1 )?true:false;
					
		pybind::decref( m_embedding );

		return destroy;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::increfEmbedding()
	{
		pybind::incref( m_embedding );
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::callMethod( const ConstString& _method, const char * _format, ... )
	{
		PyObject * _embedding = getEmbedding();

		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( _embedding, _method ) )
		{
			PyObject * function = Holder<ScriptEngine>::hostage()
				->getModuleFunction( _embedding, _method );

			va_list valist;
			va_start(valist, _format);

			Holder<ScriptEngine>::hostage()
				->callFunction( function, _format, valist );

			va_end( valist );

			pybind::decref( function );
		}

		pybind::decref( _embedding );
	}
}
