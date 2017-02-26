#	include "Kernel/Scriptable.h"

#   include "Interface/ScriptSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scriptable::Scriptable()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable::~Scriptable()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::setScriptWrapper( const ScriptWrapperInterfacePtr & _scriptWrapper )
	{
		m_scriptWrapper = _scriptWrapper;
	}
	//////////////////////////////////////////////////////////////////////////
    const ScriptWrapperInterfacePtr & Scriptable::getScriptWrapper() const
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

		PyObject * embedding = (PyObject *)m_scriptWrapper->wrap( this );

		return embedding;
	}
}
