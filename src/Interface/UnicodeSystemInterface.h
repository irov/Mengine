#pragma once

#include "Config/Typedef.h"
#include "Config/String.h"

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const size_t MENGINE_UNICODE_UNKNOWN_SIZE = ~0U;
    //////////////////////////////////////////////////////////////////////////
    class UnicodeSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "UnicodeSystem" );

    public:
        virtual bool unicodeToUtf8( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size ) = 0;
        virtual bool utf8ToUnicode( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity, size_t * _unicodeSize ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define UNICODE_SYSTEM()\
    ((Mengine::UnicodeSystemInterface *)SERVICE_GET(Mengine::UnicodeSystemInterface))
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool unicodeToUtf8Size( const WChar * _unicode, size_t _unicodeSize, String & _utf8 )
        {
            UnicodeSystemInterface * unicodeService = UNICODE_SYSTEM();

            size_t utf8Size;
            if( unicodeService->unicodeToUtf8( _unicode, _unicodeSize, nullptr, 0, &utf8Size ) == false )
            {
                return false;
            }

            if( utf8Size == 0 )
            {
                _utf8.clear();

                return true;
            }

            _utf8.resize( utf8Size );

            if( unicodeService->unicodeToUtf8( _unicode, _unicodeSize, &_utf8[0], utf8Size, nullptr ) == false )
            {
                _utf8.clear();

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool unicodeToUtf8( const WString & _unicode, String & _utf8 )
        {
            bool result = unicodeToUtf8Size( _unicode.c_str(), _unicode.size(), _utf8 );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool unicodeToUtf8( const WChar * _unicode, Char * _utf8, size_t _utf8Capacity, size_t * _utf8Size = nullptr )
        {
            UnicodeSystemInterface * unicodeService = UNICODE_SYSTEM();

            size_t unicodeSize = wcslen( _unicode );

            size_t utf8Size;
            if( unicodeService->unicodeToUtf8( _unicode, unicodeSize, _utf8, _utf8Capacity, &utf8Size ) == false )
            {
                return false;
            }

            if( _utf8Size != nullptr )
            {
                *_utf8Size = utf8Size;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool unicodeToUtf8Size( const WChar * _unicode, size_t _unicodeSize, Char * _utf8, size_t _utf8Capacity )
        {
            UnicodeSystemInterface * unicodeService = UNICODE_SYSTEM();

            size_t utf8Size;
            bool successful = unicodeService->unicodeToUtf8( _unicode, _unicodeSize, _utf8, _utf8Capacity, &utf8Size );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool unicodeToUtf8( const WString & _unicode, Char * _utf8, size_t _utf8Capacity )
        {
            UnicodeSystemInterface * unicodeService = UNICODE_SYSTEM();

            const WChar * unicodeStr = _unicode.c_str();
            size_t unicodeSize = _unicode.size();

            size_t utf8Size;
            bool successful = unicodeService->unicodeToUtf8( unicodeStr, unicodeSize, _utf8, _utf8Capacity, &utf8Size );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool utf8ToUnicode( const Char * _utf8, WChar * _unicode, size_t _unicodeCapacity )
        {
            UnicodeSystemInterface * unicodeService = UNICODE_SYSTEM();

            size_t utf8Size = strlen( _utf8 );

            size_t unicodeSize;
            bool successful = unicodeService->utf8ToUnicode( _utf8, utf8Size, _unicode, _unicodeCapacity, &unicodeSize );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool utf8ToUnicode( const Char* _utf8, WString& _unicode )
        {
            UnicodeSystemInterface* unicodeService = UNICODE_SYSTEM();

            size_t unicodeSize;
            if( unicodeService->utf8ToUnicode( _utf8, MENGINE_UNICODE_UNKNOWN_SIZE, nullptr, 0, &unicodeSize ) == false )
            {
                return false;
            }

            if( unicodeSize == 0 )
            {
                _unicode.clear();

                return true;
            }

            _unicode.resize( unicodeSize );

            if( unicodeService->utf8ToUnicode( _utf8, MENGINE_UNICODE_UNKNOWN_SIZE, &_unicode[0], unicodeSize, nullptr ) == false )
            {
                _unicode.clear();

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool utf8ToUnicodeSize( const Char * _utf8, size_t _utf8Size, WChar * _unicode, size_t _unicodeCapacity )
        {
            UnicodeSystemInterface * unicodeService = UNICODE_SYSTEM();

            size_t unicodeSize;
            bool successful = unicodeService->utf8ToUnicode( _utf8, _utf8Size, _unicode, _unicodeCapacity, &unicodeSize );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool utf8ToUnicodeSize( const Char * _utf8, size_t _utf8Size, WString & _unicode )
        {
            UnicodeSystemInterface * unicodeService = UNICODE_SYSTEM();

            size_t unicodeSize;
            if( unicodeService->utf8ToUnicode( _utf8, _utf8Size, nullptr, 0, &unicodeSize ) == false )
            {
                return false;
            }

            if( unicodeSize == 0 )
            {
                _unicode.clear();

                return true;
            }

            _unicode.resize( unicodeSize );

            if( unicodeService->utf8ToUnicode( _utf8, _utf8Size, &_unicode[0], unicodeSize, nullptr ) == false )
            {
                _unicode.clear();

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool utf8ToUnicode( const String & _utf8, WString & _unicode )
        {
            bool result = utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), _unicode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool utf8ToUnicode( const String & _utf8, WChar * _unicode, size_t _unicodeCapacity )
        {
            bool result = utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), _unicode, _unicodeCapacity );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool utf8ToUnicode( const ConstString & _utf8, WString & _unicode )
        {
            bool result = utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), _unicode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool utf8ToUnicode( const PathString & _utf8, WString & _unicode )
        {
            bool result = utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), _unicode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool utf8ToUnicode( const FilePath & _utf8, WString & _unicode )
        {
            bool result = utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), _unicode );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool utf8ToUnicode( const FilePath & _utf8, WChar * _unicode, size_t _unicodeCapacity )
        {
            bool result = utf8ToUnicodeSize( _utf8.c_str(), _utf8.size(), _unicode, _unicodeCapacity );

            return result;
        }
    }
}
