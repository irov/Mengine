#pragma once

#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32UnicodeSystem
        : public ServiceBase<UnicodeSystemInterface>
    {
    public:
        Win32UnicodeSystem();
        ~Win32UnicodeSystem() override;

    public:
        bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * const _utf8, size_t _utf8Capacity, size_t * const _utf8Size ) override;
        bool utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * const _unicode, size_t _unicodeCapacity, size_t * const _sizeUnicode ) override;
    };
}
