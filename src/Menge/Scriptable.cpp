#	include "Scriptable.h"

#	include "ScriptEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Scriptable::Scriptable()
	: m_scriptable(0)
	{}
	//////////////////////////////////////////////////////////////////////////
	Scriptable::~Scriptable()
	{}
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
				->wrapp( this );

			if( m_scriptable == 0 )
			{
				return 0;
			}
		}

		return m_scriptable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Scriptable::isScriptable() const
	{
		return m_scriptable != 0;
	}
}