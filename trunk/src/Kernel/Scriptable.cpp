#	include "Kernel/Scriptable.h"

#	include "Interface/ScriptSystemInterface.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scriptable::Scriptable()
		: m_embed(nullptr)
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
		if( m_embed != nullptr )
		{
			pybind::unwrap( m_embed );
            pybind::decref( m_embed );

            m_embed = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::setEmbed( PyObject * _embed )
	{
		pybind::decref( m_embed );
		m_embed = _embed;

		this->_embedding( m_embed );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Scriptable::getEmbed()
	{
		if( m_embed == nullptr )
		{
			PyObject * embed = this->_embedded();

			if( embed == nullptr )
			{
				return nullptr;
			}

			this->setEmbed( embed );
		}

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
		return m_embed != nullptr;
	}
}
