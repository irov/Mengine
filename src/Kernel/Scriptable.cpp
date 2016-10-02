#	include "Kernel/Scriptable.h"

#   include "Interface/ScriptSystemInterface.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scriptable::Scriptable()
		: m_scriptWrapper(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable::~Scriptable()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::setScriptWrapper( ScriptWrapperInterface * _scriptWrapper )
	{
		m_scriptWrapper = _scriptWrapper;
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptWrapperInterface * Scriptable::getScriptWrapper() const
	{
		return m_scriptWrapper;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Scriptable::_embedded()
	{
		if( m_scriptWrapper == nullptr )
		{
			return nullptr;
		}

		PyObject * embedding = m_scriptWrapper->wrap( this );

		return embedding;
	}
}
