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
	void ProfilerEngine::addResourceToProfile( const String& _name )
	{
		return m_interface->addResourceToProfile( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void ProfilerEngine::removeResourceToProfile( const String& _name )
	{
		return m_interface->removeResourceToProfile( _name );
	}
	//////////////////////////////////////////////////////////////////////////
	void ProfilerEngine::displayStats()
	{
		if(isEnabled() == false)
		{
			return;
		}

		const ProfileHistoryList & historyList = m_interface->getProfileHistoryList();

		Holder<RenderEngine>::hostage()
			->setRenderArea( mt::vec4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		static TChar m_debugText[128];

		int dipCount = Holder<RenderEngine>::hostage()->getNumDIP();
		STDSPRINTF( m_debugText, MENGE_TEXT("DIP = %d\n"), dipCount );

		mt::vec2f pos(0,20);
		Holder<RenderEngine>::hostage()->renderText( m_debugText, pos, 0xFFFFFFFF );

		for(ProfileHistoryList::const_iterator it = historyList.begin();
			it != historyList.end(); ++it)
		{
			const ProfileHistory & h = (*it);

			STDSPRINTF( m_debugText, MENGE_TEXT("%s : ctime = %.5f; calls = %d; min time = %.5f; max time = %.5f\n"), h.name.c_str(), h.currentTime, h.numCallsThisFrame, h.minTime, h.maxTime );

			pos.y+=20;
			Holder<RenderEngine>::hostage()->renderText(m_debugText, pos, 0xFFFFFFFF);
		}

		int totalReleased = m_interface->getTotalReleased();
		int totalCompiled = m_interface->getTotalCompiled();
		const String & currentCompiledResource = m_interface->getCurrentCompiled();

		STDSPRINTF( m_debugText, MENGE_TEXT("COMPILED = %s; COMPILED = %d / RELEASED = %d \n"),currentCompiledResource.c_str(), totalCompiled, totalReleased );

		pos.y+=20;
		Holder<RenderEngine>::hostage()->renderText(m_debugText, pos, 0xFF00FFFF);
		
		int numDisplayedResource = 0;

		const ProfileResourceVec & m_profileResourceList = m_interface->getProfileResourceList();

		for(ProfileResourceVec::const_iterator it = m_profileResourceList.begin();
			it != m_profileResourceList.end(); ++it)
		{

			if(++numDisplayedResource > 10)
			{
				return;
			}

			const ResourceStat * stat = *it;

			STDSPRINTF( m_debugText, MENGE_TEXT("%s: compiled = %d / released = %d \n"), stat->name.c_str(), stat->numCompiled, stat->numReleased );

			pos.y+=20;
			Holder<RenderEngine>::hostage()->renderText(m_debugText, pos, 0xFFFFFFFF);
		}

		Holder<RenderEngine>::hostage()
			->endLayer2D();
	}
}