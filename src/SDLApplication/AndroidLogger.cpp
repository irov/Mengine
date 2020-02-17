#include "AndroidLogger.h"

#include <iostream>

#include <android/log.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidLogger::AndroidLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidLogger::~AndroidLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLogger::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _count )
    {
		MENGINE_UNUSED(_level);
        MENGINE_UNUSED(_flag);
        MENGINE_UNUSED(_color);

        __android_log_print( ANDROID_LOG_ERROR, "Mengine", "%.*s", (int)_count, _data );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLogger::flush()
    {
        //Empty
    }
}
