#	include "ProfilerEngine.h"

#	include <stdio.h>
#	include <sstream>
#	include <stdarg.h>

#	include "python.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ProfilerEngine::ProfilerEngine( ProfilerSystemInterface * _interface )
		: m_interface( _interface )
		, m_totalCompiled(0)
		, m_totalReleased(0)
		, m_currentCompiledResource("")
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
	void ProfilerEngine::addResourceToProfile(const std::string & _name)
	{
		ProfileResourceMap::iterator it = m_profileResourceMap.find(_name);

		if (it == m_profileResourceMap.end()) 
		{
			Stat stat = {_name,1,0};
			ProfileResourceList::iterator it_stat;
			it_stat = m_profileResourceList.insert(m_profileResourceList.end(), stat);
			it = m_profileResourceMap.insert(std::make_pair(_name,&*it_stat)).first;
		}
		else
		{
			Stat & stat = *it->second;
			stat.numCompiled++;
			m_profileResourceList.sort();
		}

		m_totalCompiled++;

		m_currentCompiledResource = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	void ProfilerEngine::removeResourceToProfile(const std::string & _name)
	{
		ProfileResourceMap::iterator it = m_profileResourceMap.find(_name);

		if (it == m_profileResourceMap.end()) 
		{
			MENGE_LOG("ERROR, RELEASED without being compiled [%s] ...\n", _name.c_str() );
		}
		else
		{
			Stat & stat = *it->second;
			stat.numReleased++;
		}

		m_totalReleased++;
	}
	//////////////////////////////////////////////////////////////////////////
	void ProfilerEngine::displayStats( TextField * _debugTextField )
	{
		if(isEnabled() == false || _debugTextField == NULL)
		{
			return;
		}

		ProfileHistoryList historyList = m_interface->getProfileHistoryList();

		Holder<RenderEngine>::hostage()
			->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		_debugTextField->setLocalPosition(mt::vec2f::zero_v2);

		static char m_debugText[128];

		int dipCount = Holder<RenderEngine>::hostage()->getNumDIP();
		sprintf( m_debugText, "DIP = %d\n", dipCount );

		_debugTextField->setText( m_debugText );
		_debugTextField->render( false );

		for(ProfileHistoryList::iterator it = historyList.begin();
			it != historyList.end(); ++it)
		{
			ProfileHistory & h = (*it);

			sprintf( m_debugText, "%s : ctime = %.7f; calls = %d\n", h.name.c_str(), h.currentTime, h.numCallsThisFrame );

			_debugTextField->translate(mt::vec2f(0,25));
			_debugTextField->setText( m_debugText );
			_debugTextField->render( false );
		}

		sprintf( m_debugText, "COMPILED = %s; COMPILED = %d / RELEASED = %d \n",m_currentCompiledResource.c_str(), m_totalCompiled, m_totalReleased );

		_debugTextField->translate(mt::vec2f(0,25));
		_debugTextField->setText( m_debugText );
		_debugTextField->render( false );

		int numDisplayedResource = 0;

		for(ProfileResourceList::iterator it = m_profileResourceList.begin();
			it != m_profileResourceList.end(); ++it)
		{

			if(++numDisplayedResource > 10)
			{
				return;
			}

			Stat stat = *it;

			sprintf( m_debugText, "%s: compiled = %d / released = %d \n", stat.name.c_str(), stat.numCompiled, stat.numReleased );
		
			_debugTextField->translate(mt::vec2f(0,25));
			_debugTextField->setText( m_debugText );
			_debugTextField->render( false );
		}

		Holder<RenderEngine>::hostage()
			->endLayer2D();
	}
}