#include "Win32EnvironmentService.h"

#include "Kernel/Logger.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( EnvironmentService, Mengine::Win32EnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32EnvironmentService::Win32EnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32EnvironmentService::~Win32EnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32EnvironmentService::_initializeService()
    {
        ::ZeroMemory( &m_osInfo, sizeof( m_osInfo ) );
        m_osInfo.dwOSVersionInfoSize = sizeof( m_osInfo );

        HMODULE hNtdll = ::LoadLibrary( L"ntdll.dll" );

        if( hNtdll != NULL )
        {
            LONG( WINAPI * RtlGetVersion )(LPOSVERSIONINFOEXW);
            *(FARPROC *)&RtlGetVersion = ::GetProcAddress( hNtdll, "RtlGetVersion" );

            if( RtlGetVersion != NULL )
            {
                (*RtlGetVersion)(&m_osInfo);

                LOGGER_INFO( "platform", "windows version: %lu.%lu (build %lu)"
                    , m_osInfo.dwMajorVersion
                    , m_osInfo.dwMinorVersion
                    , m_osInfo.dwBuildNumber
                );

                LOGGER_INFO( "platform", "windows platform: %lu"
                    , m_osInfo.dwPlatformId
                );

                LOGGER_INFO( "platform", "windows service pack: %lu.%lu"
                    , (DWORD)m_osInfo.wServicePackMajor
                    , (DWORD)m_osInfo.wServicePackMinor
                );
            }

            ::FreeLibrary( hNtdll );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32EnvironmentService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32EnvironmentService::getDeviceName( Char * const _deviceName, size_t _capacity ) const
    {
        DWORD ComputerNameLen = (DWORD)_capacity;
        if( ::GetComputerNameA( _deviceName, &ComputerNameLen ) == FALSE )
        {
            return 0;
        }

        return ComputerNameLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32EnvironmentService::getDeviceModel( Char * const _deviceModel, size_t _capacity ) const
    {
        size_t len = MENGINE_STRNCPY_STATIC( _deviceModel, "PC", _capacity );

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32EnvironmentService::getDeviceLanguage( Char * const _deviceLanguage, size_t _capacity ) const
    {
        WCHAR unicode_localeName[LOCALE_NAME_MAX_LENGTH + 1] = {L'\0'};
        int result = ::GetUserDefaultLocaleName( unicode_localeName, LOCALE_NAME_MAX_LENGTH );

        if( result <= 0 )
        {
            return 0;
        }

        int32_t len = ::WideCharToMultiByte( CP_UTF8, 0, unicode_localeName, -1, _deviceLanguage, static_cast<int>(_capacity), NULL, NULL );
            
        if( len <= 0 )
        {
            return 0;
        }

        if( (size_t)len >= _capacity )
        {
            return 0;
        }

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32EnvironmentService::getOSFamily( Char * const _osFamily, size_t _capacity ) const
    {
        size_t len = MENGINE_STRNCPY_STATIC( _osFamily, "Windows", _capacity );

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32EnvironmentService::getOSVersion( Char * const _osVersion, size_t _capacity ) const
    {
        switch( m_osInfo.dwMajorVersion )
        {
        case 10:
            {
                switch( m_osInfo.dwMinorVersion )
                {
                case 0:
                    {
                        if( m_osInfo.wProductType == VER_NT_WORKSTATION )
                        {
                            if( m_osInfo.dwBuildNumber >= 22000 )
                            {
                                return MENGINE_STRNCPY_STATIC( _osVersion, "11", _capacity );
                            }
                            else
                            {
                                return MENGINE_STRNCPY_STATIC( _osVersion, "10", _capacity );
                            }
                        }
                        else
                        {
                            return MENGINE_STRNCPY_STATIC( _osVersion, "Server 2016", _capacity );
                        }
                    }break;
                }
            }break;
        case 6:
            {
                switch( m_osInfo.dwMinorVersion )
                {
                case 3:
                    return MENGINE_STRNCPY_STATIC( _osVersion, "8.1", _capacity );
                case 2:
                    return MENGINE_STRNCPY_STATIC( _osVersion, "8", _capacity );
                case 1:
                    return MENGINE_STRNCPY_STATIC( _osVersion, "7", _capacity );
                case 0:
                    return MENGINE_STRNCPY_STATIC( _osVersion, "Vista", _capacity );
                }
            }break;
        case 5:
            {
                switch( m_osInfo.dwMinorVersion )
                {
                case 2:
                    return MENGINE_STRNCPY_STATIC( _osVersion, "XP64", _capacity );
                case 1:
                    return MENGINE_STRNCPY_STATIC( _osVersion, "XP", _capacity );
                }
            }break;
        }

        return MENGINE_STRNCPY_STATIC( _osVersion, "Unknown", _capacity );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32EnvironmentService::getBundleId( Char * const _bundleId, size_t _capacity ) const
    {
        MENGINE_UNUSED( _bundleId );
        MENGINE_UNUSED( _capacity );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32EnvironmentService::getSessionId( Char * const _sessionId, size_t _capacity ) const
    {
        MENGINE_UNUSED( _sessionId );
        MENGINE_UNUSED( _capacity );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32EnvironmentService::getInstallKey( Char * const _installKey, size_t _capacity ) const
    {
        MENGINE_UNUSED( _installKey );
        MENGINE_UNUSED( _capacity );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t Win32EnvironmentService::getInstallTimestamp() const
    {
        return 0LL;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t Win32EnvironmentService::getInstallVersion( Char * const _installVersion, size_t _capacity ) const
    {
        MENGINE_UNUSED( _installVersion );
        MENGINE_UNUSED( _capacity );

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t Win32EnvironmentService::getInstallRND() const
    {
        return 0LL;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t Win32EnvironmentService::getSessionIndex() const
    {        
        return 0LL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32EnvironmentService::openUrlInDefaultBrowser( const Char * _url )
    {
        MENGINE_UNUSED( _url );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32EnvironmentService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        MENGINE_UNUSED( _email );
        MENGINE_UNUSED( _subject );
        MENGINE_UNUSED( _body );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
