#include "TimelineService.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimelineService, Mengine::TimelineService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimelineService::TimelineService()
        : m_revision( 0 )
        , m_current( 0.f )
        , m_time( 0.f )
        , m_total( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TimelineService::~TimelineService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::beginOffset( float _offset )
    {
        MENGINE_ASSERTION_FATAL( m_time >= _offset );

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
    uint32_t TimelineService::getCurrentRevision() const
    {
        return m_revision;
    }
    //////////////////////////////////////////////////////////////////////////
    float TimelineService::getCurrentTime() const
    {
        return m_current;
    }
    //////////////////////////////////////////////////////////////////////////
    float TimelineService::getCurrentDelta() const
    {
        return m_time;
    }
    //////////////////////////////////////////////////////////////////////////
    float TimelineService::getTotalTime() const
    {
        if( m_offsets.empty() == true )
        {
            return m_total;
        }

        float offset = m_offsets.back();

        MENGINE_ASSERTION_FATAL( m_time >= offset );

        float totalTime = m_total + offset;

        return totalTime;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::begin( const UpdateContext * _context )
    {
        m_revision = _context->revision;
        m_current = _context->current;
        m_time = _context->time;

        MENGINE_ASSERTION_FATAL( m_offsets.empty() == true, "invalid complete times" );
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::end()
    {
        m_total += m_time;
    }
}
