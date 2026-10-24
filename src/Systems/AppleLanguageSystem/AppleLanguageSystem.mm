#include "AppleLanguageSystem.h"

#include "Config/StdString.h"

#import <Foundation/Foundation.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LanguageSystem, Mengine::AppleLanguageSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleLanguageSystem::AppleLanguageSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleLanguageSystem::~AppleLanguageSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleLanguageSystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleLanguageSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleLanguageSystem::getUserLanguage( Char * const _userLanguage, size_t _capacity ) const
    {
        NSString * preferredLanguage = [[NSLocale preferredLanguages] firstObject];

        if( preferredLanguage == nil )
        {
            return false;
        }

        const Char * language = [preferredLanguage UTF8String];

        StdString::strcpy_safe( _userLanguage, language, _capacity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
