#pragma once

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/UniqueHelper.h"
#include "Kernel/Vector.h"

#include "Config/Lambda.h"
#include "Config/StdAssert.h"
#include "Config/StdUtility.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    template<class T>
    class Event
    {
        typedef Lambda<T> LambdaEvent;

        struct EventDesc
        {
            UniqueId id;
            LambdaEvent lambda;
        };

    public:
        Event()
        {
        }

        ~Event()
        {
        }

    public:
        UniqueId add( const LambdaEvent & _lambda )
        {
            UniqueId id = Helper::generateUniqueIdentity();

            EventDesc desc;
            desc.id = id;
            desc.lambda = _lambda;

            m_events.emplace_back( desc );

            return id;
        }

        LambdaEvent remove( UniqueId _id )
        {
            typename VectorEvents::iterator it_found = Helper::findUnique( m_events, _id );

            assert( it_found != m_events.end() );

            const EventDesc & desc = *it_found;

            LambdaEvent lambda = desc.lambda;

            m_events.erase( it_found );

            return lambda;
        }

        void clear()
        {
            m_events.clear();
        }

    public:
        template<class ... Args>
        void operator()( Args && ... _args ) const
        {
            for( const EventDesc & desc : m_events )
            {
                desc.lambda( StdUtility::forward<Args>( _args )... );
            }
        }

    protected:
        typedef Vector<EventDesc> VectorEvents;
        VectorEvents m_events;
    };
}
