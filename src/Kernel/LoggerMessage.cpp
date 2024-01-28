#include "LoggerMessage.h"

#include "Kernel/Assertion.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void recordLoggerMessage( const LoggerMessage & _message, LoggerRecord * const _record )
        {
            MENGINE_ASSERTION_FATAL( _message.category != nullptr );
            MENGINE_ASSERTION_FATAL( _message.data != nullptr );

            _record->timestamp = _message.timestamp;
            MENGINE_STRNCPY( _record->category, _message.category, MENGINE_LOGGER_MAX_CATEGORY );            
            _record->threadName = _message.threadName;
            _record->level = _message.level;
            _record->flag = _message.flag;
            _record->filter = _message.filter;
            _record->color = _message.color;
            _record->file = _message.file;
            _record->line = _message.line;
            _record->data.assign( _message.data, _message.data + _message.size );
        }
        //////////////////////////////////////////////////////////////////////////
        void unrecordLoggerMessage( const LoggerRecord & _record, LoggerMessage * const _message )
        {
            _message->timestamp = _record.timestamp;
            _message->category = _record.category;
            _message->threadName = _record.threadName;
            _message->level = _record.level;
            _message->flag = _record.flag;
            _message->filter = _record.filter;
            _message->color = _record.color;
            _message->file = _record.file;
            _message->line = _record.line;
            _message->data = _record.data.data();
            _message->size = _record.data.size();
        }
        //////////////////////////////////////////////////////////////////////////
    }
}