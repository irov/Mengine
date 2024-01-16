#include "TimerService.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimerService, Mengine::TimerService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimerService::TimerService()
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
    UniqueId TimerService::addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId new_id = Helper::generateUniqueIdentity();

        TimerDesc desc;
        desc.id = new_id;
        desc.milliseconds = _milliseconds;
        desc.time = _milliseconds;
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
        VectorTimers::iterator it_found = Algorithm::find_if( m_timers.begin(), m_timers.end(), [_id]( const TimerDesc & _desc )
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
    void TimerService::update( float _time )
    {
        for( TimerDesc & desc : m_timers )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            desc.time -= _time;

            if( desc.time > 0.f )
            {
                continue;
            }

            desc.time += desc.milliseconds;

            desc.lambda( desc.id );
        }

        VectorTimers::iterator it_erase = Algorithm::remove_if( m_timers.begin(), m_timers.end(), []( const TimerDesc & _desc )
        {
            return _desc.id == INVALID_UNIQUE_ID;
        } );

        m_timers.erase( it_erase, m_timers.end() );
    }
    //////////////////////////////////////////////////////////////////////////
}
