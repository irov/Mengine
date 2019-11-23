#include "TimelineService.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimelineService, Mengine::TimelineService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimelineService::TimelineService()
        : m_time( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TimelineService::~TimelineService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::beginOffset( float _offset )
    {
        m_offsets.emplace_back( _offset );
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
    void TimelineService::tick( const UpdateContext * _context )
    {
        float time = _context->time;

        MENGINE_ASSERTION_FATAL( m_offsets.empty() == true, "invalid complete times" );

        m_time += time;
    }
}
