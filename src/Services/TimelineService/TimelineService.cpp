#include "TimelineService.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimelineService, Mengine::TimelineService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimelineService::TimelineService()
        : m_context{~0U, -1.f, -1.f}
        , m_total( 0.f )
        , m_timeFactorBase( 1.f )
        , m_timeFactorCoefficient( 0.0625f )
        , m_timeFactorCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TimelineService::~TimelineService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TimelineService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::_finalizeService()
    {
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_offsets );

        m_offsets.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::beginOffset( float _offset, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( m_context.time >= _offset, "time %f < %f revision (%u) [doc: %s]"
            , m_context.time
            , _offset
            , m_context.revision
            , MENGINE_DOCUMENT_STR( _doc )
        );

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
    const UpdateContext * TimelineService::getCurrentContext() const
    {
        return &m_context;
    }
    //////////////////////////////////////////////////////////////////////////
    float TimelineService::getTotalTime() const
    {
        if( m_offsets.empty() == true )
        {
            return m_total;
        }

        float offset = m_offsets.back();

        MENGINE_ASSERTION_FATAL( m_context.time >= offset, "time %f < %f"
            , m_context.time
            , offset
        );

        float totalTime = m_total + offset;

        return totalTime;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::begin( const UpdateContext * _context )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _context, "invalid context" );
        MENGINE_ASSERTION_FATAL( m_context.revision == ~0U, "double begin" );

        m_context = *_context;

        MENGINE_ASSERTION_FATAL( m_offsets.empty() == true, "invalid complete times" );
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::end()
    {
        m_total += m_context.time;

        m_context.revision = ~0U;
        m_context.current = -1.f;
        m_context.time = -1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float TimelineService::calcTimeFactor() const
    {
        float timeFactor = m_timeFactorBase + m_timeFactorCoefficient * m_timeFactorCount;

        if( timeFactor < 0.f )
        {
            return 0.f;
        }

        return timeFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::setTimeFactorBase( float _timeFactor )
    {
        m_timeFactorBase = _timeFactor;

        float timeFactor = this->calcTimeFactor();

        NOTIFICATION_NOTIFY( NOTIFICATOR_TIME_FACTOR_CHANGE, timeFactor );
    }
    //////////////////////////////////////////////////////////////////////////
    float TimelineService::getTimeFactorBase() const
    {
        return m_timeFactorBase;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::setTimeFactorCoefficient( float _timeFactorCoefficient )
    {
        m_timeFactorCoefficient = _timeFactorCoefficient;

        float timeFactor = this->calcTimeFactor();

        NOTIFICATION_NOTIFY( NOTIFICATOR_TIME_FACTOR_CHANGE, timeFactor );
    }
    //////////////////////////////////////////////////////////////////////////
    float TimelineService::getTimeFactorCoefficient() const
    {
        return m_timeFactorCoefficient;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimelineService::setTimeFactorCount( int32_t _timeFactorCount )
    {
        m_timeFactorCount = _timeFactorCount;

        float timeFactor = this->calcTimeFactor();

        NOTIFICATION_NOTIFY( NOTIFICATOR_TIME_FACTOR_CHANGE, timeFactor );
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t TimelineService::getTimeFactorCount() const
    {
        return m_timeFactorCount;
    }
    //////////////////////////////////////////////////////////////////////////
}
