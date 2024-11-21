#include "StdioLogger.h"

#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

#include "Config/Path.h"

#include <iostream>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    StdioLogger::StdioLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StdioLogger::~StdioLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void StdioLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        if( message.flag & LFLAG_TIMESTAMP )
        {
            Char timestamp[256 + 1] = {'\0'};
            size_t timestampSize = Helper::makeLoggerShortDate( message.timestamp, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            std::cout.write( timestamp, timestampSize );
            std::cout.write( " ", 1 );
        }

        if( message.flag & LFLAG_SYMBOLSTAMP )
        {
            ELoggerLevel level = message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            std::cout.write( &symbol, 1 );
            std::cout.write( " ", 1 );
        }

        if( message.flag & LFLAG_CATEGORYSTAMP )
        {
            size_t category_size = StdString::strlen( message.category );

            std::cout.write( "[", 1 );
            std::cout.write( message.category, category_size );
            std::cout.write( "]", 1 );
            std::cout.write( " ", 1 );
        }

        if( message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256 + 1] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( message.threadName, "|%s|", threadstamp, 0, 256 );
            std::cout.write( threadstamp, threadstampSize );
            std::cout.write( " ", 1 );
        }

        if( message.flag & LFLAG_FUNCTIONSTAMP )
        {
            Path functionstamp = {'\0'};
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( message.function, message.line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );
            std::cout.write( functionstamp, functionstampSize );
            std::cout.write( " ", 1 );
        }

        std::cout.write( message.data, message.size );
        std::cout.write( "\n", 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void StdioLogger::_flush()
    {
        std::cout.flush();
    }
    //////////////////////////////////////////////////////////////////////////
}
