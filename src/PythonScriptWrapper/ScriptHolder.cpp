#	include "ScriptHolder.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScriptHolder::ScriptHolder()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptHolder::_setEventListener( const pybind::dict & _listener )
	{
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

        m_script = EVENTABLE_METHODR( this, EVENT_KEEP_SCRIPT, pybind::object() )
            ->onScriptHolderKeepScript();
		//EVENTABLE_ASK( m_serviceProvider, this, EVENT_KEEP_SCRIPT, m_script )();

		bool successful = m_script.is_valid();
		
		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScriptHolder::_deactivate()
	{
		Node::_deactivate();

        EVENTABLE_METHOD( this, EVENT_RELEASE_SCRIPT )
            ->onScriptHolderReleaseScript( m_script );
		//EVENTABLE_CALL(m_serviceProvider, this, EVENT_RELEASE_SCRIPT)( m_script	);

		m_script.reset();
	}

}
