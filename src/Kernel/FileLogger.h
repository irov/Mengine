#pragma once

#include "Interface/LoggerInterface.h"
#include "Interface/StreamInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class FileLogger
        : public LoggerInterface
        , public Factorable
    {
    public:
        FileLogger();
        ~FileLogger() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setVerboseLevel( EMessageLevel _level ) override;
        void setVerboseFlag( uint32_t _flag ) override;

    public:
        bool validMessage( EMessageLevel _level, uint32_t _flag ) const override;

    public:
        OutputStreamInterfacePtr getStream() const;

    protected:
        void log( EMessageLevel _level, uint32_t _flag, const Char * _data, uint32_t _count ) override;

    protected:
        void flush() override;

    protected:
        EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;

        OutputStreamInterfacePtr m_stream;
    };
}
