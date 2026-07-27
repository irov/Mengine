#include "TimepipeService.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/UniqueHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimepipeService, Mengine::TimepipeService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimepipeService::TimepipeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TimepipeService::~TimepipeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TimepipeService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimepipeService::_finalizeService()
    {
        m_timepipe.erase( StdAlgorithm::remove_if( m_timepipe.begin(), m_timepipe.end(), []( const TimepipeDesc & _desc )
        {
            return _desc.timepipe == nullptr;
        } ), m_timepipe.end() );

#if defined(MENGINE_DOCUMENT_ENABLE)
        for( const TimepipeDesc & desc : m_timepipe )
        {
            LOGGER_ASSERTION( "was forgotten remove timepipe '%s'"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_timepipe.clear();
        m_timepipeAdd.clear();

#if defined(MENGINE_DOCUMENT_ENABLE)
        for( const TimepipeCallbackDesc & desc : m_timebeginCallbacks )
        {
            LOGGER_ASSERTION( "was forgotten remove timebegin callback '%s'"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }

        for( const TimepipeCallbackDesc & desc : m_timebeginCallbacksAux )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            LOGGER_ASSERTION( "was forgotten remove timebegin callback '%s'"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }

        for( const TimepipeCallbackDesc & desc : m_timeendCallbacks )
        {
            LOGGER_ASSERTION( "was forgotten remove timeend callback '%s'"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }

        for( const TimepipeCallbackDesc & desc : m_timeendCallbacksAux )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            LOGGER_ASSERTION( "was forgotten remove timeend callback '%s'"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_timebeginCallbacks.clear();
        m_timebeginCallbacksAux.clear();
        m_timeendCallbacks.clear();
        m_timeendCallbacksAux.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void TimepipeService::addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_timepipeAdd.begin(), m_timepipeAdd.end(), [_timepipe]( const TimepipeDesc & _desc )
        {
            return _desc.timepipe == _timepipe;
        } ) == m_timepipeAdd.end(), "timepipe '%s' already added"
            , MENGINE_DOCUMENT_STR( _doc )
            );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_timepipe.begin(), m_timepipe.end(), [_timepipe]( const TimepipeDesc & _desc )
        {
            return _desc.timepipe == _timepipe;
        } ) == m_timepipe.end(), "timepipe '%s' already added"
            , MENGINE_DOCUMENT_STR( _doc )
            );

        TimepipeDesc desc;
        desc.timepipe = _timepipe;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_timepipeAdd.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void TimepipeService::removeTimepipe( const TimepipeInterfacePtr & _timepipe )
    {
        VectorTimepipe::iterator it_found = StdAlgorithm::find_if( m_timepipe.begin(), m_timepipe.end(), [_timepipe]( const TimepipeDesc & _desc )
        {
            return _desc.timepipe == _timepipe;
        } );

        if( it_found != m_timepipe.end() )
        {
            TimepipeDesc & desc = *it_found;

            TimepipeInterfacePtr old_timepipe = desc.timepipe;
            desc.timepipe = nullptr;

            return;
        }

        VectorTimepipe::iterator it_add_found = StdAlgorithm::find_if( m_timepipeAdd.begin(), m_timepipeAdd.end(), [_timepipe]( const TimepipeDesc & _desc )
        {
            return _desc.timepipe == _timepipe;
        } );

        if( it_add_found == m_timepipeAdd.end() )
        {
            return;
        }

        m_timepipeAdd.erase( it_add_found );
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId TimepipeService::addTimebeginCallback( const LambdaTimepipeCallback & _callback, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId id = Helper::generateUniqueIdentity();

        TimepipeCallbackDesc desc;
        desc.id = id;
        desc.callback = _callback;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_timebeginCallbacks.emplace_back( desc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TimepipeService::removeTimebeginCallback( UniqueId _id )
    {
        VectorTimepipeCallbacks::iterator it_found = Helper::findUnique( m_timebeginCallbacks, _id );

        if( it_found != m_timebeginCallbacks.end() )
        {
            m_timebeginCallbacks.erase( it_found );

            return true;
        }

        VectorTimepipeCallbacks::iterator it_aux_found = Helper::findUnique( m_timebeginCallbacksAux, _id );

        if( it_aux_found == m_timebeginCallbacksAux.end() )
        {
            return false;
        }

        TimepipeCallbackDesc & desc = *it_aux_found;
        desc.id = INVALID_UNIQUE_ID;
        desc.callback = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId TimepipeService::addTimeendCallback( const LambdaTimepipeCallback & _callback, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId id = Helper::generateUniqueIdentity();

        TimepipeCallbackDesc desc;
        desc.id = id;
        desc.callback = _callback;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_timeendCallbacks.emplace_back( desc );

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TimepipeService::removeTimeendCallback( UniqueId _id )
    {
        VectorTimepipeCallbacks::iterator it_found = Helper::findUnique( m_timeendCallbacks, _id );

        if( it_found != m_timeendCallbacks.end() )
        {
            m_timeendCallbacks.erase( it_found );

            return true;
        }

        VectorTimepipeCallbacks::iterator it_aux_found = Helper::findUnique( m_timeendCallbacksAux, _id );

        if( it_aux_found == m_timeendCallbacksAux.end() )
        {
            return false;
        }

        TimepipeCallbackDesc & desc = *it_aux_found;
        desc.id = INVALID_UNIQUE_ID;
        desc.callback = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimepipeService::tick( const UpdateContext * _context )
    {
        m_timepipe.insert( m_timepipe.end(), m_timepipeAdd.begin(), m_timepipeAdd.end() );
        m_timepipeAdd.clear();

        m_timebeginCallbacksAux.swap( m_timebeginCallbacks );
        m_timebeginCallbacks.clear();

        for( TimepipeCallbackDesc & desc : m_timebeginCallbacksAux )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            desc.id = INVALID_UNIQUE_ID;
            desc.callback( _context );
        }

        m_timebeginCallbacksAux.clear();

        for( const TimepipeDesc & desc : m_timepipe )
        {
            if( desc.timepipe == nullptr )
            {
                continue;
            }

            desc.timepipe->onTimepipe( _context );
        }

        m_timeendCallbacksAux.swap( m_timeendCallbacks );
        m_timeendCallbacks.clear();

        for( TimepipeCallbackDesc & desc : m_timeendCallbacksAux )
        {
            if( desc.id == INVALID_UNIQUE_ID )
            {
                continue;
            }

            desc.id = INVALID_UNIQUE_ID;
            desc.callback( _context );
        }

        m_timeendCallbacksAux.clear();

        m_timepipe.erase( StdAlgorithm::remove_if( m_timepipe.begin(), m_timepipe.end(), []( const TimepipeDesc & _desc )
        {
            return _desc.timepipe == nullptr;
        } ), m_timepipe.end() );
    }
    //////////////////////////////////////////////////////////////////////////
}
