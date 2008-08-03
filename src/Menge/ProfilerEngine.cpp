#	include "ProfilerEngine.h"

#	include <stdio.h>
#	include <sstream>
#	include <stdarg.h>

#	include "python.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ProfilerEngine::ProfilerEngine( ProfilerSystemInterface * _interface )
		: m_interface( _interface )
	{}
	//////////////////////////////////////////////////////////////////////////
	void ProfilerEngine::beginProfile( const String & _name )
	{
		m_interface->beginProfile( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void ProfilerEngine::endProfile( const String & _name )
	{
		m_interface->endProfile( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void ProfilerEngine::setEnabled( bool _enabled )
	{
		m_interface->setEnabled(_enabled);
	}
	//////////////////////////////////////////////////////////////////////////
	bool ProfilerEngine::isEnabled() const
	{
		return m_interface->getEnabled();
	}
	//////////////////////////////////////////////////////////////////////////
	void ProfilerEngine::displayStats( TextField * _debugTextField )
	{
		if(isEnabled() == false || _debugTextField == NULL)
		{
			return;
		}

		ProfileHistoryList historyList = m_interface->getProfileHistoryList();

		Viewport vp( mt::vec2f( 0.0, 0.0f ), mt::vec2f( 1024.0f, 768.0f ) );

		_debugTextField->setLocalPosition(mt::vec2f::zero_v2);

		static char m_debugText[128];

		int dipCount = Holder<RenderEngine>::hostage()->getNumDIP();
		sprintf( m_debugText, "DIP = %d\n", dipCount );

		_debugTextField->setText( m_debugText );
		_debugTextField->render( vp, false );

		for(ProfileHistoryList::iterator it = historyList.begin();
			it != historyList.end(); ++it)
		{
			ProfileHistory & h = (*it);

			sprintf( m_debugText, "%s : ctime = %.7f; calls = %d\n", h.name.c_str(), h.currentTime, h.numCallsThisFrame );

			_debugTextField->translate(mt::vec2f(0,25));
			_debugTextField->setText( m_debugText );
			_debugTextField->render( vp, false );
		}
	}
}