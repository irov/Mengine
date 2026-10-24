#include "LanguageService.h"

#include "Interface/LanguageSystemInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LanguageService, Mengine::LanguageService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool normalizeLanguageTag( const Char * _language, Char * const _tag, size_t _capacity )
        {
            _tag[0] = '\0';

            const Char * modifier = StdString::strchr( _language, '@' );

            if( modifier != nullptr )
            {
                return false;
            }

            size_t length = 0;
            size_t subtagLength = 0;
            bool primary = true;

            for( const Char * it = _language; *it != '\0' && *it != '.'; ++it )
            {
                Char ch = *it;

                if( length + 1 >= _capacity )
                {
                    return false;
                }

                if( ch == '-' || ch == '_' )
                {
                    if( subtagLength == 0 )
                    {
                        return false;
                    }

                    if( primary == true && subtagLength < 2 )
                    {
                        return false;
                    }

                    primary = false;
                    subtagLength = 0;
                }
                else
                {
                    bool alpha = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
                    bool digit = ch >= '0' && ch <= '9';

                    if( alpha == false && (primary == true || digit == false) )
                    {
                        return false;
                    }

                    ++subtagLength;

                    if( subtagLength > 8 )
                    {
                        return false;
                    }
                }

                ++length;
            }

            if( subtagLength == 0 || (primary == true && subtagLength < 2) )
            {
                return false;
            }

            for( size_t index = 0; index != length; ++index )
            {
                Char ch = _language[index];
                _tag[index] = ch == '_' ? '-' : ch;
            }

            _tag[length] = '\0';

            if( MENGINE_STRICMP( _tag, "POSIX" ) == 0 )
            {
                _tag[0] = '\0';

                return false;
            }

            if( MENGINE_STRICMP( _tag, "und" ) == 0 )
            {
                _tag[0] = '\0';

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t getLocaleMatchSize( const Char * _language, const ConstString & _locale )
        {
            const Char * locale = _locale.c_str();

            Char available[MENGINE_LOCALE_LANGUAGE_SIZE + 1] = {'\0'};
            if( Detail::normalizeLanguageTag( locale, available, sizeof( available ) ) == false )
            {
                return 0;
            }

            Char language[MENGINE_LOCALE_LANGUAGE_SIZE + 1] = {'\0'};
            StdString::strcpy_safe( language, _language, sizeof( language ) );

            size_t availableSize = StdString::strlen( available );
            size_t languageSize = StdString::strlen( language );

            size_t singletonSuffixSize = 2;

            while( languageSize > availableSize )
            {
                while( languageSize != 0 && language[languageSize - 1] != '-' )
                {
                    --languageSize;
                }

                if( languageSize == 0 )
                {
                    return 0;
                }

                --languageSize;

                if( languageSize >= singletonSuffixSize && language[languageSize - singletonSuffixSize] == '-' )
                {
                    languageSize -= singletonSuffixSize;
                }
            }

            if( languageSize != availableSize )
            {
                return 0;
            }

            language[languageSize] = '\0';

            if( MENGINE_STRICMP( language, available ) != 0 )
            {
                return 0;
            }

            return availableSize;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    LanguageService::LanguageService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LanguageService::~LanguageService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LanguageService::_initializeService()
    {
        Char language[MENGINE_LOCALE_LANGUAGE_SIZE + 1] = {'\0'};
        if( LANGUAGE_SYSTEM()
            ->getUserLanguage( language, sizeof( language ) ) == false )
        {
            LOGGER_WARNING( "invalid get user language" );

            return true;
        }

        Char userLanguage[MENGINE_LOCALE_LANGUAGE_SIZE + 1] = {'\0'};
        if( Detail::normalizeLanguageTag( language, userLanguage, sizeof( userLanguage ) ) == false )
        {
            LOGGER_WARNING( "invalid user language '%s'"
                , language
            );

            return true;
        }

        m_userLanguage = Helper::stringizeString( userLanguage );

        LOGGER_INFO( "language", "user language '%s'"
            , m_userLanguage.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LanguageService::_finalizeService()
    {
        m_userLanguage = ConstString::none();
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & LanguageService::getUserLanguage() const
    {
        return m_userLanguage;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LanguageService::findUserLocale( const VectorConstString & _locales, ConstString * const _locale ) const
    {
        const Char * userLanguage = m_userLanguage.c_str();

        if( this->lookupLanguageLocale( userLanguage, _locales, _locale ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LanguageService::lookupLanguageLocale( const Char * _language, const VectorConstString & _locales, ConstString * const _locale ) const
    {
        Char language[MENGINE_LOCALE_LANGUAGE_SIZE + 1] = {'\0'};
        if( Detail::normalizeLanguageTag( _language, language, sizeof( language ) ) == false )
        {
            return false;
        }

        size_t bestSize = 0;
        const ConstString * bestLocale = nullptr;

        for( const ConstString & locale : _locales )
        {
            size_t matchSize = Detail::getLocaleMatchSize( language, locale );

            if( matchSize <= bestSize )
            {
                continue;
            }

            bestSize = matchSize;
            bestLocale = &locale;
        }

        if( bestLocale == nullptr )
        {
            return false;
        }

        *_locale = *bestLocale;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
