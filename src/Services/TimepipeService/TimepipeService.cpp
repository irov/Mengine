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
            return _desc.id == REMOVE_UNIQUE_ID;
        } ), m_timepipe.end() );

#if defined(MENGINE_DOCUMENT_ENABLE)
        for( const TimepipeDesc & desc : m_timepipe )
        {
            LOGGER_ERROR( "Not remove timepipe '%s'"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_timepipe.clear();
        m_timepipeAdd.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId TimepipeService::addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId newid = Helper::generateUniqueIdentity();

        TimepipeDesc desc;
        desc.id = newid;
        desc.timepipe = _timepipe;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_timepipeAdd.emplace_back( desc );

        return newid;
    }
    //////////////////////////////////////////////////////////////////////////
    TimepipeInterfacePtr TimepipeService::removeTimepipe( UniqueId _id )
    {
        VectorTimepipe::iterator it_find = Algorithm::find_if( m_timepipe.begin(), m_timepipe.end(), [_id]( const TimepipeDesc & _desc )
        {
            return _desc.id == _id;
        } );

        if( it_find != m_timepipe.end() )
        {
            TimepipeDesc & desc = *it_find;

            desc.id = REMOVE_UNIQUE_ID;

            TimepipeInterfacePtr old_timepipe = desc.timepipe;
            desc.timepipe = nullptr;

            return old_timepipe;
        }

        VectorTimepipe::iterator it_add_find = Algorithm::find_if( m_timepipeAdd.begin(), m_timepipeAdd.end(), [_id]( const TimepipeDesc & _desc )
        {
            return _desc.id == _id;
        } );

        if( it_add_find == m_timepipeAdd.end() )
        {
            return nullptr;
        }

        TimepipeDesc & add_desc = *it_add_find;

        TimepipeInterfacePtr old_timepipe = add_desc.timepipe;

        m_timepipeAdd.erase( it_add_find );

        return old_timepipe;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimepipeService::tick( const UpdateContext * _context )
    {
        m_timepipe.insert( m_timepipe.end(), m_timepipeAdd.begin(), m_timepipeAdd.end() );
        m_timepipeAdd.clear();

        for( const TimepipeDesc & desc : m_timepipe )
        {
            if( desc.id == REMOVE_UNIQUE_ID )
            {
                continue;
            }

            desc.timepipe->onTimepipe( _context );
        }

        m_timepipe.erase( Algorithm::remove_if( m_timepipe.begin(), m_timepipe.end(), []( const TimepipeDesc & _desc )
        {
            return _desc.id == REMOVE_UNIQUE_ID;
        } ), m_timepipe.end() );
    }
    //////////////////////////////////////////////////////////////////////////
}
