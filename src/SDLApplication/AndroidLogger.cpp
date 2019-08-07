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
    void AndroidLogger::log( ELoggerLevel _level, uint32_t _flag, const Char * _data, uint32_t _count )
    {
		MENGINE_UNUSED(_level);
        MENGINE_UNUSED(_flag);

        __android_log_print( ANDROID_LOG_ERROR, "Mengine", "%.*s", _count, _data );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLogger::flush()
    {
        //Empty
    }
}	// namespace Mengine
