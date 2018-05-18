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
    void AndroidLogger::log( EMessageLevel _level, uint32_t _flag, const Char * _data, size_t _count )
    {
		(void)_level;
        (void)_flag;
		(void)_count;

        __android_log_print( ANDROID_LOG_ERROR, "Mengine", "%.*s", _count, _data );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidLogger::flush()
    {
    }
}	// namespace Mengine
