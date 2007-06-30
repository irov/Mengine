#	include "Scriptable.h"

namespace Menge
{
	Scriptable::Scriptable()
	: m_scriptable(0)
	{

	}
	Scriptable::~Scriptable()
	{
		delete m_scriptable;
	}
	//////////////////////////////////////////////////////////////////////////
	void Scriptable::setScriptable( ScriptObject * _scriptable )
	{
		m_scriptable = _scriptable;
	}
	//////////////////////////////////////////////////////////////////////////
	ScriptObject * Scriptable::getScriptable()
	{
		return m_scriptable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scriptable::isScriptable() const
	{
		return m_scriptable != 0;
	}
}