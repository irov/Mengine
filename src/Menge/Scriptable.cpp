#	include "Scriptable.h"

namespace Menge
{
	Scriptable::Scriptable()
	: m_scriptable(0)
	{

	}
	Scriptable::~Scriptable()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::setScriptable( PyObject * _scriptable )
	{
		m_scriptable = _scriptable;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Scriptable::getScriptable()
	{
		return m_scriptable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scriptable::isScriptable() const
	{
		return m_scriptable != 0;
	}
}