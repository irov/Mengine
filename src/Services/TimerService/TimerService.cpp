#include "TimerService.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/DocumentHelper.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimerService, Mengine::TimerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimerService::TimerService()
        : m_prevTime( MENGINE_UINT64_C( ~0 ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TimerService::~TimerService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TimerService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimerService::_finalizeService()
    {
#if defined(MENGINE_DEBUG)
        for( const TimerDesc & desc : m_timers )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            LOGGER_ASSERTION( "forgot remove platform timer (doc: %s)"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_timers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId TimerService::addTimer( Timestamp _delay, const LambdaTimer & _lambda, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId new_id = Helper::generateUniqueIdentity();

        TimerDesc desc;
        desc.id = new_id;
        desc.delay = _delay;
        desc.time = _delay;
        desc.lambda = _lambda;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_timers.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimerService::removeTimer( UniqueId _id )
    {
        VectorTimers::iterator it_found = StdAlgorithm::find_if( m_timers.begin(), m_timers.end(), [_id]( const TimerDesc & _desc )
        {
            return _desc.id == _id;
        } );

        MENGINE_ASSERTION_FATAL( it_found != m_timers.end(), "not found timer '%u'"
            , _id
        );

        TimerDesc & desc = *it_found;

        desc.id = INVALID_UNIQUE_ID;
        desc.lambda = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimerService::_preUpdate()
    {
        Timestamp currentTime = Helper::getSystemTimestamp();

        Timestamp deltaTime = m_prevTime == MENGINE_UINT64_C( ~0 ) ? MENGINE_UINT64_C( 0 ) : currentTime - m_prevTime;

        m_prevTime = currentTime;

        for( TimerDesc & desc : m_timers )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            desc.time -= deltaTime;

            if( desc.time > 0 )
            {
                continue;
            }

            desc.time += desc.delay;

            desc.lambda( desc.id );
        }

        VectorTimers::iterator it_erase = StdAlgorithm::remove_if( m_timers.begin(), m_timers.end(), []( const TimerDesc & _desc )
        {
            return _desc.id == INVALID_UNIQUE_ID;
        } );

        m_timers.erase( it_erase, m_timers.end() );
    }
    //////////////////////////////////////////////////////////////////////////
}
