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
		: m_embed(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable::~Scriptable()
	{
		if( m_embed )
		{
			pybind::decref( m_embed );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::incref()
	{
		if( m_embed )
		{
			pybind::incref( m_embed );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::decref()
	{
		if( m_embed )
		{
			pybind::decref( m_embed );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::unwrap()
	{
		if( m_embed )
		{
			pybind::unwrap( m_embed );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::setEmbed( PyObject * _embed )
	{
		if( _embed == 0 )
		{
			return;
		}

		m_embed = _embed;

		PyObject * py_dict = pybind::dict_from(_embed);

		this->_embedding( py_dict );

		pybind::decref( py_dict );

		pybind::incref( m_embed );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Scriptable::getEmbed()
	{
		if( m_embed == 0 )
		{
			PyObject * embed = _embedded();

			if( embed == 0 )
			{
				return 0;
			}

			this->setEmbed( embed );
			pybind::decref( embed );
		}
		//else
		//{
			//pybind::incref( m_embed );
		//}

		return m_embed;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::_embedding( PyObject * _embed )
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scriptable::isEmbed() const
	{
		return m_embed != 0;
	}
}
