#pragma once

#include "Interface/LoggerInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Deque.h"
#include "Kernel/LoggerMessage.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPRuntimeContext
    {
    public:
        struct RuntimeLog
        {
            uint64_t sequence;
            Timestamp timestamp;
            ELoggerLevel level;
            String category;
            String message;
        };

        typedef Deque<RuntimeLog> DequeRuntimeLogs;

    public:
        MCPRuntimeContext();

        bool initialize();
        void finalize();

        void captureLog( const LoggerMessage & _message );
        void restoreRuntime();

        const ThreadMutexInterfacePtr & getLogMutex() const;
        const DequeRuntimeLogs & getLogs() const;

        bool isPaused() const;
        void setPaused( bool _paused );
        float getPreviousTimeFactorBase() const;
        void setPreviousTimeFactorBase( float _value );
        int32_t getPreviousTimeFactorCount() const;
        void setPreviousTimeFactorCount( int32_t _value );

    protected:
        ThreadMutexInterfacePtr m_logMutex;
        DequeRuntimeLogs m_logs;
        uint64_t m_nextLogSequence;
        bool m_paused;
        float m_previousTimeFactorBase;
        int32_t m_previousTimeFactorCount;
    };
    //////////////////////////////////////////////////////////////////////////
}
