#	include "TimelineService.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimelineService, Menge::TimelineService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TimelineService::TimelineService()
		: m_time(0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TimelineService::~TimelineService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void TimelineService::beginOffset( float _offset )
	{ 
		m_offsets.push_back( _offset );
	}
	//////////////////////////////////////////////////////////////////////////
	float TimelineService::getOffset() const
	{
		if( m_offsets.empty() == true )
		{
			return 0.f;
		}

		float offset = m_offsets.back();

		return offset;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimelineService::endOffset()
	{
		m_offsets.pop_back();
	}
	//////////////////////////////////////////////////////////////////////////
	float TimelineService::getTime() const
	{
		if( m_offsets.empty() == true )
		{
			return m_time;
		}

		float offset = m_offsets.back();

		float totalTime = m_time + offset;

		return totalTime;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimelineService::update( float _timing )
	{
		if( m_offsets.empty() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("TimelineService::update invalid complete times"
				);

			return;
		}

		m_time += _timing;
	}
}
