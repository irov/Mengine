#include "Win32LanguageSystem.h"

#include "Environment/Windows/WindowsIncluder.h"
#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LanguageSystem, Mengine::Win32LanguageSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32LanguageSystem::Win32LanguageSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32LanguageSystem::~Win32LanguageSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32LanguageSystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32LanguageSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32LanguageSystem::getUserLanguage( Char * const _userLanguage, size_t _capacity ) const
    {
        LANGID langId = ::GetUserDefaultUILanguage();
        LCID lcid = MAKELCID( langId, SORT_DEFAULT );
        int capacity = (int)_capacity;

        if( ::GetLocaleInfoA( lcid, LOCALE_SNAME, _userLanguage, capacity ) == 0 )
        {
            LOGGER_ERROR( "invalid get user default ui language name %ls"
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
