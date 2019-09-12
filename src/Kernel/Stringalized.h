#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"
#include "Kernel/Resolution.h"
#include "Kernel/Color.h"
#include "Kernel/AspectRatioViewport.h"

#include "Config/String.h"
#include "Config/Char.h"

#include <stdint.h>

namespace Mengine
{
    namespace Helper
    {
        bool stringalized( const Char * _string, bool * _value );
        bool stringalized( const Char * _string, int8_t * _value );
        bool stringalized( const Char * _string, uint8_t * _value );
        bool stringalized( const Char * _string, int32_t * _value );
        bool stringalized( const Char * _string, uint32_t * _value );
        bool stringalized( const Char * _string, int64_t * _value );
        bool stringalized( const Char * _string, uint64_t * _value );
        bool stringalized( const Char * _string, float * _value );
        bool stringalized( const Char * _string, double * _value );
        bool stringalized( const Char * _string, const Char ** _value );
        bool stringalized( const Char * _string, String * _value );
        bool stringalized( const Char * _string, ConstString * _value );
        bool stringalized( const Char * _string, FilePath * _value );
        bool stringalized( const Char * _string, Tags * _value );
        bool stringalized( const Char * _string, Resolution * _value );
        bool stringalized( const Char * _string, Color * _value );
        bool stringalized( const Char * _string, AspectRatioViewport * _value );
    }
}