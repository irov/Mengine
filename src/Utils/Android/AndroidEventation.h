#pragma once

#include "Interface/ThreadInterface.h"

#include "Config/Lambda.h"
#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class AndroidEventation
    {
    public:
        typedef IntrusivePtr<T> EventHandlerPtr;
        typedef Lambda<void( const EventHandlerPtr & )> LambdaEventHandler;
        typedef Vector<LambdaEventHandler> VectorEventHandlerCommand;

    public:
        void setMutex( const ThreadMutexInterfacePtr & _mutex )
        {
            m_mutex = _mutex;
        }

        void setEventHandler( const EventHandlerPtr & _handler )
        {
            m_handler = _handler;
        }

        void addCommand( const LambdaEventHandler & _command )
        {
            m_mutex->lock();
            m_commands.emplace_back( _command );
            m_mutex->unlock();
        }

        void invoke()
        {
            VectorEventHandlerCommand commands;

            m_mutex->lock();
            std::swap( m_commands, commands );
            m_mutex->unlock();

            if( m_handler == nullptr )
            {
                return;
            }

            for( const LambdaEventHandler & command : commands )
            {
                command( m_handler );
            }
        }

    protected:
        ThreadMutexInterfacePtr m_mutex;

        VectorEventHandlerCommand m_commands;

        EventHandlerPtr m_handler;
    };
}