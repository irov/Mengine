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

        void setPaused( bool _paused );
        bool isPaused() const;
        void setTimeScale( float _timeScale );
        void beginAdvance();
        void endAdvance();
        bool isAdvancing() const;

    protected:
        void updateFreeze_();

    protected:
        ThreadMutexInterfacePtr m_logMutex;
        DequeRuntimeLogs m_logs;
        uint64_t m_nextLogSequence;
        bool m_paused;
        bool m_advancing;
        bool m_frozen;
        bool m_timeScaled;
        float m_previousTimeFactorBase;
        int32_t m_previousTimeFactorCount;
    };
    //////////////////////////////////////////////////////////////////////////
}
