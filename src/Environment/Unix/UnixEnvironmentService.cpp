#include "UnixEnvironmentService.h"

#include "Kernel/CryptographyHelper.h"
#include "Kernel/SHA1.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

#include <clocale>
#include <cstdlib>

#include <pwd.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( EnvironmentService, Mengine::UnixEnvironmentService );
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<size_t N>
        static void readFirstLine( const Char * _path, StaticString<N> * const _value )
        {
            FILE * file = ::fopen( _path, "r" );

            if( file == nullptr )
            {
                return;
            }

            Char line[N] = {'\0'};
            if( ::fgets( line, N, file ) != nullptr )
            {
                size_t length = StdString::strlen( line );
                while( length != 0 && (line[length - 1] == '\n' || line[length - 1] == '\r') )
                {
                    line[--length] = '\0';
                }

                _value->assign( line );
            }

            ::fclose( file );
        }
        //////////////////////////////////////////////////////////////////////////
        template<size_t N>
        static void readOSVersion( StaticString<N> * const _value )
        {
            FILE * file = ::fopen( "/etc/os-release", "r" );

            if( file == nullptr )
            {
                return;
            }

            Char line[512];
            while( ::fgets( line, sizeof( line ), file ) != nullptr )
            {
                static const Char prefix[] = "PRETTY_NAME=";
                const size_t prefixLength = sizeof( prefix ) - 1;

                if( StdString::strncmp( line, prefix, prefixLength ) != 0 )
                {
                    continue;
                }

                Char * value = line + prefixLength;
                size_t length = StdString::strlen( value );

                while( length != 0 && (value[length - 1] == '\n' || value[length - 1] == '\r') )
                {
                    value[--length] = '\0';
                }

                if( length >= 2 && value[0] == '"' && value[length - 1] == '"' )
                {
                    value[length - 1] = '\0';
                    ++value;
                }

                _value->assign( value );
                break;
            }

            ::fclose( file );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    UnixEnvironmentService::UnixEnvironmentService()
        : m_installTimestamp( 0 )
        , m_installRND( 0 )
        , m_sessionTimestamp( 0 )
        , m_sessionIndex( 0 )
        , m_sessionRND( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UnixEnvironmentService::~UnixEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixEnvironmentService::_initializeService()
    {
        ::setlocale( LC_ALL, "" );

        m_userName.assign( "" );
        m_deviceModel.assign( "Linux" );
        m_deviceLanguage.assign( "" );
        m_osFamily.assign( "Linux" );
        m_osVersion.assign( "" );
        m_bundleId.assign( "" );
        m_installId.assign( "" );
        m_installVersion.assign( "" );
        m_sessionId.assign( "" );

        struct passwd password;
        struct passwd * passwordResult = nullptr;
        Char passwordBuffer[4096];

        if( ::getpwuid_r( ::getuid(), &password, passwordBuffer, sizeof( passwordBuffer ), &passwordResult ) == 0 && passwordResult != nullptr )
        {
            m_userName.assign( password.pw_name );
        }

        struct utsname systemInfo;
        if( ::uname( &systemInfo ) == 0 )
        {
            m_deviceModel.assign( systemInfo.machine );
            m_osVersion.assign( systemInfo.release );
        }

        Detail::readOSVersion( &m_osVersion );

        const Char * locale = ::setlocale( LC_MESSAGES, nullptr );
        if( locale != nullptr )
        {
            m_deviceLanguage.assign( locale );
        }

        StaticString<256> machineId;
        machineId.assign( "" );
        Detail::readFirstLine( "/etc/machine-id", &machineId );

        StaticString<MENGINE_ENVIRONMENT_USER_MAXNAME + MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME + 320> fingerprintSource;
        fingerprintSource.assign( "FINGERPRINT_" );
        fingerprintSource.append( m_userName );
        fingerprintSource.append( '_' );
        fingerprintSource.append( m_deviceModel );
        fingerprintSource.append( '_' );
        fingerprintSource.append( machineId );

        StaticString<MENGINE_SHA1_HEX_COUNT + 1> fingerprint;
        Helper::makeSHA1HEX( fingerprintSource.data(), fingerprintSource.size(), fingerprint.data(), true );
        m_installId.assign( fingerprint );

        m_sessionTimestamp = Helper::getSystemTimestamp();

        uint64_t randomSeed = 0;
        if( Helper::generateRandomSeed( &randomSeed ) == true )
        {
            m_sessionRND = (int64_t)randomSeed;
        }

        Helper::generateRandomHexadecimal( m_sessionId.capacity(), m_sessionId.data(), false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixEnvironmentService::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixEnvironmentService::getUserName( Char * const _userName ) const
    {
        m_userName.copy( _userName );
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixEnvironmentService::getDeviceModel( Char * const _deviceModel ) const
    {
        m_deviceModel.copy( _deviceModel );
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixEnvironmentService::getDeviceLanguage( Char * const _deviceLanguage ) const
    {
        m_deviceLanguage.copy( _deviceLanguage );
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixEnvironmentService::getOSFamily( Char * const _osFamily ) const
    {
        m_osFamily.copy( _osFamily );
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixEnvironmentService::getOSVersion( Char * const _osVersion ) const
    {
        m_osVersion.copy( _osVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixEnvironmentService::getBundleId( Char * const _bundleId ) const
    {
        m_bundleId.copy( _bundleId );
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixEnvironmentService::getInstallId( Char * const _installId ) const
    {
        m_installId.copy( _installId );
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp UnixEnvironmentService::getInstallTimestamp() const
    {
        return m_installTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixEnvironmentService::getInstallVersion( Char * const _installVersion ) const
    {
        m_installVersion.copy( _installVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t UnixEnvironmentService::getInstallRND() const
    {
        return m_installRND;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp UnixEnvironmentService::getSessionTimestamp() const
    {
        return m_sessionTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t UnixEnvironmentService::getSessionIndex() const
    {
        return m_sessionIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t UnixEnvironmentService::getSessionRND() const
    {
        return m_sessionRND;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixEnvironmentService::getSessionId( Char * const _sessionId ) const
    {
        m_sessionId.copy( _sessionId );
    }
    //////////////////////////////////////////////////////////////////////////
}
