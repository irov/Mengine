#pragma once

#include "Config/Lambda.h"
#include "Config/Vector.h"

#include <algorithm>

namespace Mengine
{
    template<class T>
    class Event
    {
        typedef Lambda<T> LambdaEvent;

        struct EventDesc
        {
            uint32_t id;
            LambdaEvent lambda;
        };

    public:
        Event()
            : m_enumerator( 0 )
        {
        }

        ~Event()
        {
        }

    public:
        uint32_t add( const LambdaEvent & _lambda )
        {
            uint32_t id = ++m_enumerator;

            EventDesc desc;
            desc.id = id;
            desc.lambda = _lambda;

            m_events.emplace_back( desc );

            return id;
        }

        uint32_t remove( const LambdaEvent & _lambda )
        {
            typename VectorEvents::iterator it_found = std::find_if( m_events.begin(), m_events.end(), [&_lambda]( const EventDesc & _desc )
            {
                return _desc.lambda == _lambda;
            } );

            MENGINE_ASSERTION_FATAL( it_found != m_events.end() );

            const EventDesc & desc = *it_found;

            uint32_t id = desc.id;

            m_events.erase( it_found );

            return id;
        }

        LambdaEvent remove( uint32_t _id )
        {
            typename VectorEvents::iterator it_found = std::find_if( m_events.begin(), m_events.end(), [_id]( const EventDesc & _desc )
            {
                return _desc.id == _id;
            } );

            MENGINE_ASSERTION_FATAL( it_found != m_events.end() );

            const EventDesc & desc = *it_found;

            LambdaEvent lambda = desc.lambda;

            m_events.erase( it_found );

            return lambda;
        }

        void clear()
        {
            m_enumerator = 0;
            m_events.clear();
        }

    public:
        template<class ... Args>
        void operator()( Args && ... _args ) const
        {
            for( const EventDesc & desc : m_events )
            {
                desc.lambda( _args... );
            }
        }

    protected:
        uint32_t m_enumerator;

        typedef Vector<EventDesc> VectorEvents;
        VectorEvents m_events;
    };
}