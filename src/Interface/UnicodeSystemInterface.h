#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class UnicodeSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "UnicodeSystem" );

    public:
        virtual bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) = 0;
        virtual bool utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define UNICODE_SYSTEM()\
    ((Mengine::UnicodeSystemInterface *)SERVICE_GET(Mengine::UnicodeSystemInterface))
//////////////////////////////////////////////////////////////////////////

