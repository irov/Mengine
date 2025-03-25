#pragma once

#include "Interface/LoggerRecordInterface.h"

#include "Config/Path.h"

namespace Mengine
{
    class LoggerRecord
        : public LoggerRecordInterface
    {
        DECLARE_FACTORABLE( LoggerRecord );

    public:
        LoggerRecord();
        ~LoggerRecord() override;

    public:
        void initialize( const LoggerMessage & _message );
        
    public:
        void getMessage( LoggerMessage * const _message ) const override;

    protected:
        Timestamp m_timestamp;
        Char m_category[MENGINE_LOGGER_MAX_CATEGORY + 1] = {'\0'};
        ConstString m_threadName;
        ELoggerLevel m_level;
        uint32_t m_flag;
        uint32_t m_filter;
        uint32_t m_color;
        Path m_file = {'\0'};
        Char m_function[MENGINE_CODE_MAX_FUNCTION_NAME + 1] = {'\0'};
        int32_t m_line;
        Char m_data[MENGINE_LOGGER_MAX_MESSAGE + 1] = {'\0'};
        size_t m_size;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LoggerRecord, LoggerRecordInterface> LoggerRecordPtr;
    //////////////////////////////////////////////////////////////////////////
}
