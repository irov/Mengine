#include "TimepipeService.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/Logger.h"

#include "Config/Algorithm.h"

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
        m_timepipe.erase( Algorithm::remove_if( m_timepipe.begin(), m_timepipe.end(), []( const TimepipeDesc & _desc )
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
    }
    //////////////////////////////////////////////////////////////////////////
    void TimepipeService::addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( Algorithm::find_if( m_timepipeAdd.begin(), m_timepipeAdd.end(), [_timepipe]( const TimepipeDesc & _desc )
        {
            return _desc.timepipe == _timepipe;
        } ) == m_timepipeAdd.end(), "timepipe '%s' already added"
            , MENGINE_DOCUMENT_STR( _doc )
            );

        MENGINE_ASSERTION_FATAL( Algorithm::find_if( m_timepipe.begin(), m_timepipe.end(), [_timepipe]( const TimepipeDesc & _desc )
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
        VectorTimepipe::iterator it_found = Algorithm::find_if( m_timepipe.begin(), m_timepipe.end(), [_timepipe]( const TimepipeDesc & _desc )
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

        VectorTimepipe::iterator it_add_found = Algorithm::find_if( m_timepipeAdd.begin(), m_timepipeAdd.end(), [_timepipe]( const TimepipeDesc & _desc )
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
    void TimepipeService::tick( const UpdateContext * _context )
    {
        m_timepipe.insert( m_timepipe.end(), m_timepipeAdd.begin(), m_timepipeAdd.end() );
        m_timepipeAdd.clear();

        for( const TimepipeDesc & desc : m_timepipe )
        {
            if( desc.timepipe == nullptr )
            {
                continue;
            }

            desc.timepipe->onTimepipe( _context );
        }

        m_timepipe.erase( Algorithm::remove_if( m_timepipe.begin(), m_timepipe.end(), []( const TimepipeDesc & _desc )
        {
            return _desc.timepipe == nullptr;
        } ), m_timepipe.end() );
    }
    //////////////////////////////////////////////////////////////////////////
}
