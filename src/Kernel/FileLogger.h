#pragma once

#include "Interface/FileGroupInterface.h"
#include "Interface/OutputStreamInterface.h"

#include "Kernel/LoggerBase.h"

namespace Mengine
{
    class FileLogger
        : public LoggerBase
    {
    public:
        FileLogger();
        ~FileLogger() override;

    public:
        void setFileGroup( const FileGroupInterfacePtr & _fileGroup );
        const FileGroupInterfacePtr & getFileGroup() const;

        void setFilePath( const FilePath & _filePath );
        const FilePath & getFilePath() const;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        const OutputStreamInterfacePtr & getStream() const;

    protected:
        void log( ELoggerLevel _level, uint32_t _flag, const Char * _data, uint32_t _count ) override;

    protected:
        void flush() override;

    protected:
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;

        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileLogger, LoggerInterface> FileLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
