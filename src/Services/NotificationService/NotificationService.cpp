#include "NotificationService.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/MixinDebug.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( NotificationService, Mengine::NotificationService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NotificationService::NotificationService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NotificationService::~NotificationService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationService::_initializeService()
    {
        for( NotificatorId index = 0; index != MENGINE_NOTIFICATOR_MAX_COUNT; ++index )
        {
            NotificationArea & area = m_areas[index];

            if( area.initialize( index ) == false )
            {
                return false;
            }
        }

        for( uint32_t index = 0; index != MENGINE_NOTIFICATOR_MAX_COUNT; ++index )
        {
            NotificationArea & area = m_areas[index];

            ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create mutex" );

            area.setMutex( mutex );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::_finalizeService()
    {
        for( uint32_t index = 0; index != MENGINE_NOTIFICATOR_MAX_COUNT; ++index )
        {
            NotificationArea & area = m_areas[index];

            area.setMutex( nullptr );
        }

        for( uint32_t index = 0; index != MENGINE_NOTIFICATOR_MAX_COUNT; ++index )
        {
            NotificationArea & area = m_areas[index];

            area.finalize();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::addObserver( NotificatorId _id, Observable * _observer, const ObserverCallableInterfacePtr & _callable, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT, "id '%u' out of range '%u'"
            , _id
            , MENGINE_NOTIFICATOR_MAX_COUNT
        );

        NotificationArea & area = m_areas[_id];

        area.addObserver( _observer, _callable, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NotificationService::removeObserver( NotificatorId _id, Observable * _observer )
    {
        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT, "id '%u' out of range '%u'"
            , _id
            , MENGINE_NOTIFICATOR_MAX_COUNT
        );

        NotificationArea & area = m_areas[_id];

        area.removeObserver( _observer );
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationService::hasObserver( Observable * _observer ) const
    {
        for( uint32_t index = 0; index != MENGINE_NOTIFICATOR_MAX_COUNT; ++index )
        {
            const NotificationArea & area = m_areas[index];

            if( area.hasObserver( _observer ) == true )
            {
                return true;
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NotificationService::foreachObservers( NotificatorId _id, const LambdaObserver & _lambda )
    {
        MENGINE_ASSERTION_FATAL( _id < MENGINE_NOTIFICATOR_MAX_COUNT, "id '%u' out of range '%u'"
            , _id
            , MENGINE_NOTIFICATOR_MAX_COUNT
        );

        NotificationArea & area = m_areas[_id];

        bool successful = area.foreachObservers( _lambda );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}