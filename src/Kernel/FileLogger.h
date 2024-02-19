#pragma once

#include "Interface/ContentInterface.h"
#include "Interface/OutputStreamInterface.h"

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FileLogger
        : public LoggerBase
    {
    public:
        FileLogger();
        ~FileLogger() override;

    public:
        void setContent( const ContentInterfacePtr & _content );
        const ContentInterfacePtr & getContent() const;

    public:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    public:
        const OutputStreamInterfacePtr & getStream() const;

    public:
        void _log( const LoggerRecordInterfacePtr & _record ) override;

    public:
        void _flush() override;

    protected:
        ContentInterfacePtr m_content;

        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileLogger, LoggerInterface> FileLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
