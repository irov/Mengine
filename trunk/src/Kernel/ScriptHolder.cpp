#	include "ScriptHolder.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptHolder::ScriptHolder()
		: m_script(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptHolder::_setEventListener( const pybind::dict & _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_KEEP_SCRIPT, ("onKeepScript"), _listener );
		this->registerEvent( EVENT_RELEASE_SCRIPT, ("onReleaseScript"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScriptHolder::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		EVENTABLE_ASK( m_serviceProvider, this, EVENT_KEEP_SCRIPT, m_script )();

		if( m_script == nullptr )
		{
			return false;
		}

		pybind::incref( m_script );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptHolder::_deactivate()
	{
		Node::_deactivate();

		if( m_script != nullptr )
		{
			EVENTABLE_CALL(m_serviceProvider, this, EVENT_RELEASE_SCRIPT)( m_script	);

			pybind::decref( m_script );
			m_script = nullptr;
		}
	}

}
