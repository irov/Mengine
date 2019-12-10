#include "TimepipeService.h"

#include "Kernel/Logger.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TimepipeService, Mengine::TimepipeService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TimepipeService::TimepipeService()
        : m_enumerator( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TimepipeService::~TimepipeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TimepipeService::addTimepipe( const TimepipeInterfacePtr & _timepipe, const DocumentPtr & _doc )
    {
        uint32_t newid = ++m_enumerator;

        TimepipeDesc desc;
        desc.id = newid;
        desc.timepipe = _timepipe;
        desc.doc = _doc;

        m_timepipeAdd.emplace_back( desc );

        return newid;
    }
    //////////////////////////////////////////////////////////////////////////
    TimepipeInterfacePtr TimepipeService::removeTimepipe( uint32_t _id )
    {
        VectorTimepipe::iterator it_find = std::find_if( m_timepipe.begin(), m_timepipe.end(), [_id]( const TimepipeDesc & _desc )
        {
            return _desc.id == _id;
        } );

        if( it_find != m_timepipe.end() )
        {
            TimepipeDesc & desc = *it_find;

            desc.id = ~0U;

            TimepipeInterfacePtr old_timepipe = desc.timepipe;
            desc.timepipe = nullptr;

            return old_timepipe;
        }

        VectorTimepipe::iterator it_add_find = std::find_if( m_timepipeAdd.begin(), m_timepipeAdd.end(), [_id]( const TimepipeDesc & _desc )
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
            if( desc.id == ~0U )
            {
                continue;
            }

            desc.timepipe->onTimepipe( _context );
        }

        m_timepipe.erase( std::remove_if( m_timepipe.begin(), m_timepipe.end(), []( const TimepipeDesc & _desc )
        {
            return _desc.id == ~0U;
        } ), m_timepipe.end() );
    }
}
