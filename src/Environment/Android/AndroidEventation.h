#pragma once

#include "Interface/ThreadMutexInterface.h"

#include "AndroidEventationInterface.h"

#include "Kernel/Vector.h"

#include "Config/Lambda.h"

namespace Mengine
{
    template<class T>
    class AndroidEventation
        : public AndroidEventationInterface
    {
    public:
        typedef IntrusivePtr<T> EventHandlerPtr;
        typedef Lambda<void( const EventHandlerPtr & )> LambdaEventHandler;
        typedef Vector<LambdaEventHandler> VectorEventHandlerCommand;

    public:
        bool initialize( const ThreadMutexInterfacePtr & _mutex )
        {
            m_mutex = _mutex;

            return true;
        }

        void finalize()
        {
            m_mutex = nullptr;
            m_handler = nullptr;

            m_commands.clear();
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

    protected:
        void invoke() override
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
        EventHandlerPtr m_handler;

        VectorEventHandlerCommand m_commands;
    };
}