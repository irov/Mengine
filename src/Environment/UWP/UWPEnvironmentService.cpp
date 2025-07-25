#include "UWPEnvironmentService.h"

#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"
#include "Kernel/SHA1.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( EnvironmentService, Mengine::UWPEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void getUserName( Char * const _userName )
        {
            DWORD unicode_userNameLen = UNLEN + 1;
            ::GetUserNameA( _userName, &unicode_userNameLen );

            LOGGER_INFO_PROTECTED( "environment", "user name: %s"
                , _userName
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static void getDeviceModel( Char * const _deviceModel )
        {
            MENGINE_STRCPY_STATIC( _deviceModel, "PC", MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME );

            LOGGER_INFO_PROTECTED( "environment", "device model: %s"
                , _deviceModel
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static void getDeviceLanguage( Char * const _deviceLanguage )
        {
            LCID lcid = GetUserDefaultLCID();

            ::GetLocaleInfoA( lcid, LOCALE_SISO639LANGNAME, _deviceLanguage, LOCALE_NAME_MAX_LENGTH );

            LOGGER_INFO_PROTECTED( "environment", "device language: %s"
                , _deviceLanguage
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static void getOSFamily( Char * const _osFamily )
        {
            MENGINE_STRCPY_STATIC( _osFamily, "Windows", MENGINE_ENVIRONMENT_OS_FAMILY_MAXNAME );

            LOGGER_INFO_PROTECTED( "environment", "os family: %s"
                , _osFamily
            );
        }
        //////////////////////////////////////////////////////////////////////////
        static void getOSVersion( const OSVERSIONINFOEXW * _osInfo, Char * const _osVersion )
        {
            switch( _osInfo->dwMajorVersion )
            {
            case 10:
                {
                    switch( _osInfo->dwMinorVersion )
                    {
                    case 0:
                        {
                            if( _osInfo->wProductType == VER_NT_WORKSTATION )
                            {
                                if( _osInfo->dwBuildNumber >= 22000 )
                                {
                                    MENGINE_STRCPY_STATIC( _osVersion, "11", MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME );
                                }
                                else
                                {
                                    MENGINE_STRCPY_STATIC( _osVersion, "10", MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME );
                                }
                            }
                            else
                            {
                                MENGINE_STRCPY_STATIC( _osVersion, "Server 2016", MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME );
                            }
                        }break;
                    }
                }break;
            case 6:
                {
                    switch( _osInfo->dwMinorVersion )
                    {
                    case 3:
                        MENGINE_STRCPY_STATIC( _osVersion, "8.1", MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME );
                        break;
                    case 2:
                        MENGINE_STRCPY_STATIC( _osVersion, "8", MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME );
                        break;
                    case 1:
                        MENGINE_STRCPY_STATIC( _osVersion, "7", MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME );
                        break;
                    case 0:
                        MENGINE_STRCPY_STATIC( _osVersion, "Vista", MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME );
                        break;
                    }
                }break;
            case 5:
                {
                    switch( _osInfo->dwMinorVersion )
                    {
                    case 2:
                        MENGINE_STRCPY_STATIC( _osVersion, "XP64", MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME );
                        break;
                    case 1:
                        MENGINE_STRCPY_STATIC( _osVersion, "XP", MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME );
                        break;
                    }
                }break;
            default:
                {
                    MENGINE_STRCPY_STATIC( _osVersion, "Unknown", MENGINE_ENVIRONMENT_OS_VERSION_MAXNAME );
                }break;
            }

            LOGGER_INFO_PROTECTED( "environment", "os version: %s"
                , _osVersion
            );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    UWPEnvironmentService::UWPEnvironmentService()
        : m_installTimestamp( 0 )
        , m_installRND( 0 )
        , m_sessionIndex( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UWPEnvironmentService::~UWPEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UWPEnvironmentService::_initializeService()
    {
        OSVERSIONINFOEXW osInfo;
        ::ZeroMemory( &osInfo, sizeof( osInfo ) );
        osInfo.dwOSVersionInfoSize = sizeof( osInfo );

        HMODULE hNtdll = ::LoadLibrary( L"ntdll.dll" );

        if( hNtdll != NULL )
        {
            LONG( WINAPI * RtlGetVersion )(LPOSVERSIONINFOEXW);
            *(FARPROC *)&RtlGetVersion = ::GetProcAddress( hNtdll, "RtlGetVersion" );

            if( RtlGetVersion != NULL )
            {
                (*RtlGetVersion)(&osInfo);

                LOGGER_INFO( "platform", "windows version: %lu.%lu (build %lu)"
                    , osInfo.dwMajorVersion
                    , osInfo.dwMinorVersion
                    , osInfo.dwBuildNumber
                );

                LOGGER_INFO( "platform", "windows platform: %lu"
                    , osInfo.dwPlatformId
                );

                LOGGER_INFO( "platform", "windows service pack: %lu.%lu"
                    , (DWORD)osInfo.wServicePackMajor
                    , (DWORD)osInfo.wServicePackMinor
                );
            }

            ::FreeLibrary( hNtdll );
        }

        Detail::getUserName( m_userName.data() );
        Detail::getDeviceModel( m_deviceModel.data() );
        Detail::getDeviceLanguage( m_deviceLanguage.data() );
        Detail::getOSFamily( m_osFamily.data() );
        Detail::getOSVersion( &osInfo, m_osVersion.data() );

        StaticString<MENGINE_ENVIRONMENT_USER_MAXNAME + MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME + 64> fingerprintGarbage;

        fingerprintGarbage.assign( "FINGERPRINT" );

        fingerprintGarbage.append( "_" );
        fingerprintGarbage.append( m_userName.c_str() );
        fingerprintGarbage.append( "_" );
        fingerprintGarbage.append( m_deviceModel.c_str() );

        DWORD VolumeSerialNumber = 0;
        DWORD VolumeMaxComponentLen = 0;
        DWORD VolumeFileSystemFlags = 0;
        CHAR VolumeFileSystemNameBuffer[MENGINE_MAX_PATH + 1] = {'\0'};
        CHAR VolumeNameBuffer[MENGINE_MAX_PATH + 1] = {'\0'};

        if( ::GetVolumeInformationA( NULL
            , VolumeNameBuffer
            , sizeof( VolumeNameBuffer ) / sizeof( VolumeNameBuffer[0] )
            , &VolumeSerialNumber
            , &VolumeMaxComponentLen
            , &VolumeFileSystemFlags
            , VolumeFileSystemNameBuffer
            , sizeof( VolumeFileSystemNameBuffer ) / sizeof( VolumeFileSystemNameBuffer[0] ) ) == TRUE )
        {
            LOGGER_INFO_PROTECTED( "environment", "system volume serial number: %u"
                , VolumeSerialNumber
            );

            Path VolumeSerialNumberBuffer = {'\0'};
            MENGINE_SNPRINTF( VolumeSerialNumberBuffer, MENGINE_MAX_PATH, "%u", VolumeSerialNumber );

            fingerprintGarbage.append( "_" );
            fingerprintGarbage.append( VolumeSerialNumberBuffer );
        }

        StaticString<MENGINE_SHA1_HEX_COUNT + 1> fingerprint;
        Helper::makeSHA1HEX( fingerprintGarbage.data(), fingerprintGarbage.size(), fingerprint.data(), true );

        m_sessionId.assign( fingerprint );
        m_installId.assign( fingerprint );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPEnvironmentService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPEnvironmentService::getUserName( Char * const _userName ) const
    {
        m_userName.copy( _userName );
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPEnvironmentService::getDeviceModel( Char * const _deviceModel ) const
    {
        m_deviceModel.copy( _deviceModel );
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPEnvironmentService::getDeviceLanguage( Char * const _deviceLanguage ) const
    {
        m_deviceLanguage.copy( _deviceLanguage );
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPEnvironmentService::getOSFamily( Char * const _osFamily ) const
    {
        m_osFamily.copy( _osFamily );
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPEnvironmentService::getOSVersion( Char * const _osVersion ) const
    {
        m_osVersion.copy( _osVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPEnvironmentService::getBundleId( Char * const _bundleId ) const
    {
        m_bundleId.copy( _bundleId );
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPEnvironmentService::getSessionId( Char * const _sessionId ) const
    {
        m_sessionId.copy( _sessionId );
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPEnvironmentService::getInstallId( Char * const _installId ) const
    {
        m_installId.copy( _installId );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t UWPEnvironmentService::getInstallTimestamp() const
    {
        return m_installTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPEnvironmentService::getInstallVersion( Char * const _installVersion ) const
    {
        m_installVersion.copy( _installVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t UWPEnvironmentService::getInstallRND() const
    {
        return m_installRND;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t UWPEnvironmentService::getSessionIndex() const
    {        
        return m_sessionIndex;
    }
    //////////////////////////////////////////////////////////////////////////
}
