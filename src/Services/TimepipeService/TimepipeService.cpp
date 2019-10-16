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
    uint32_t TimepipeService::addTime( const LambdaTimepipe & _timepipe )
    {
        uint32_t newid = ++m_enumerator;

        TimepipeDesc desc;
        desc.id = newid;
        desc.timepipe = _timepipe;

        m_timepipeAdd.emplace_back( desc );

        return newid;
    }
    //////////////////////////////////////////////////////////////////////////
    void TimepipeService::removeTime( uint32_t _id )
    {
        VectorTimepipe::iterator it_find = std::find_if( m_timepipe.begin(), m_timepipe.end(), [_id]( const TimepipeDesc & _desc )
        {
            return _desc.id == _id;
        } );

        if( it_find != m_timepipe.end() )
        {
            TimepipeDesc & desc = *it_find;

            desc.id = ~0U;

            return;
        }

        VectorTimepipe::iterator it_add_find = std::find_if( m_timepipeAdd.begin(), m_timepipeAdd.end(), [_id]( const TimepipeDesc & _desc )
        {
            return _desc.id == _id;
        } );

        if( it_add_find != m_timepipeAdd.end() )
        {
            m_timepipeAdd.erase( it_add_find );
        }
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

            desc.timepipe( _context );
        }

        m_timepipe.erase( std::remove_if( m_timepipe.begin(), m_timepipe.end(), []( const TimepipeDesc & _desc )
        {
            return _desc.id == ~0U;
        } ), m_timepipe.end() );
    }
}
