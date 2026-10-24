#include "POSIXLanguageSystem.h"

#include "Config/StdLib.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LanguageSystem, Mengine::POSIXLanguageSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char * getMessagesLocale()
        {
            const Char * names[] = {"LC_ALL", "LC_MESSAGES", "LANG"};

            for( const Char * name : names )
            {
                const Char * value = StdLib::getenv( name );

                if( value == nullptr )
                {
                    continue;
                }

                if( value[0] == '\0' )
                {
                    continue;
                }

                return value;
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXLanguageSystem::POSIXLanguageSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXLanguageSystem::~POSIXLanguageSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXLanguageSystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXLanguageSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXLanguageSystem::getUserLanguage( Char * const _userLanguage, size_t _capacity ) const
    {
        const Char * language = Detail::getMessagesLocale();

        if( language == nullptr )
        {
            return false;
        }

        StdString::strcpy_safe( _userLanguage, language, _capacity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
