#	include "Kernel/Scriptable.h"

#	include "Interface/ScriptSystemInterface.h"
#	include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

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
        this->unwrap();
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::unwrap()
	{
		if( m_embed )
		{
			pybind::unwrap( m_embed );
            pybind::decref( m_embed );

            m_embed = NULL;
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

		//PyObject * py_dict = pybind::dict_from( _embed );

		this->_embedding( m_embed );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Scriptable::getEmbed()
	{
		if( m_embed == 0 )
		{
			PyObject * embed = this->_embedded();

			if( embed == 0 )
			{
				return 0;
			}

			this->setEmbed( embed );
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
        (void)_embed;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scriptable::isEmbed() const
	{
		return m_embed != 0;
	}
}
