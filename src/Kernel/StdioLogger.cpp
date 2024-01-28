#include "StdioLogger.h"

#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

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
    void StdioLogger::_log( const LoggerMessage & _message )
    {
        if( _message.flag & LFLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( _message.file, _message.line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );
            std::cout.write( functionstamp, functionstampSize );
            std::cout.write( " ", 1 );
        }

        if( _message.flag & LFLAG_TIMESTAMP )
        {
            Char timestamp[256] = {'\0'};
            size_t timestampSize = Helper::makeLoggerShortDate( _message.timestamp, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            std::cout.write( timestamp, timestampSize );
            std::cout.write( " ", 1 );
        }

        if( _message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( _message.threadName, "|%s|", threadstamp, 0, 256 );
            std::cout.write( threadstamp, threadstampSize );
            std::cout.write( " ", 1 );
        }

        if( _message.flag & LFLAG_SYMBOLSTAMP )
        {
            ELoggerLevel level = _message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            std::cout.write( &symbol, 1 );
            std::cout.write( " ", 1 );
        }

        if( _message.flag & LFLAG_CATEGORYSTAMP )
        {
            size_t category_size = MENGINE_STRLEN( _message.category );

            std::cout.write( "[", 1 );
            std::cout.write( _message.category, category_size );
            std::cout.write( "]", 1 );
            std::cout.write( " ", 1 );
        }

        const Char * data = _message.data;
        size_t size = _message.size;

        std::cout.write( data, size );
        std::cout.write( "\n", 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void StdioLogger::_flush()
    {
        std::cout.flush();
    }
    //////////////////////////////////////////////////////////////////////////
}
