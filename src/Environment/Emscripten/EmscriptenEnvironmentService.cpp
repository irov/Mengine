#include "EmscriptenEnvironmentService.h"

#include "Interface/DateTimeSystemInterface.h"

#include "EmscriptenLocalStorage.h"

#include "Kernel/CryptographyHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/TimestampHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( EnvironmentService, Mengine::EmscriptenEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void getLocalStorageValue( const Char * _key, T * const _value )
        {
            constexpr size_t valueCapacity = 32;

            Char value[valueCapacity] = {'\0'};
            if( mengineLocalStorageGetItem( _key, value, valueCapacity ) == false )
            {
                return;
            }

            Helper::stringalized( value, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static void setLocalStorageValue( const Char * _key, T _value )
        {
            constexpr size_t valueCapacity = 32;

            Char value[valueCapacity] = {'\0'};
            Helper::stringalized( _value, value, valueCapacity );

            mengineLocalStorageSetItem( _key, value );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    EmscriptenEnvironmentService::EmscriptenEnvironmentService()
        : m_installTimestamp( 0 )
        , m_installRND( 0 )
        , m_sessionTimestamp( 0 )
        , m_sessionIndex( 0 )
        , m_sessionRND( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EmscriptenEnvironmentService::~EmscriptenEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenEnvironmentService::_initializeService()
    {
        m_userName.assign( "" );
        m_deviceModel.assign( "Browser" );
        m_osFamily.assign( "Web" );
        m_osVersion.assign( "" );
        m_bundleId.assign( "" );
        m_installVersion.assign( "" );

        m_sessionTimestamp = Helper::getSystemTimestamp();
        DATETIME_SYSTEM()
            ->getDateTimeFromMilliseconds( m_sessionTimestamp, &m_sessionDate );

        uint64_t randomSeed = 0;
        if( Helper::generateRandomSeed( &randomSeed ) == true )
        {
            m_sessionRND = (int64_t)randomSeed;
        }

        Char * sessionIdData = m_sessionId.data();
        size_t sessionIdCapacity = m_sessionId.capacity();

        if( Helper::generateRandomHexadecimal( sessionIdData, sessionIdCapacity, false ) == false )
        {
            return false;
        }

        Char * installIdData = m_installId.data();
        size_t installIdCapacity = m_installId.capacity();

        if( mengineLocalStorageGetItem( "mengine.install_id", installIdData, installIdCapacity ) == false )
        {
            m_installId.assign( m_sessionId );
            m_installTimestamp = m_sessionTimestamp;
            m_installRND = m_sessionRND;

            const Char * installId = m_installId.c_str();
            mengineLocalStorageSetItem( "mengine.install_id", installId );

            Detail::setLocalStorageValue( "mengine.install_timestamp", m_installTimestamp );
            Detail::setLocalStorageValue( "mengine.install_rnd", m_installRND );
        }
        else
        {
            Detail::getLocalStorageValue( "mengine.install_timestamp", &m_installTimestamp );
            Detail::getLocalStorageValue( "mengine.install_rnd", &m_installRND );
        }

        Detail::getLocalStorageValue( "mengine.session_index", &m_sessionIndex );

        int64_t nextSessionIndex = m_sessionIndex + 1;
        Detail::setLocalStorageValue( "mengine.session_index", nextSessionIndex );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenEnvironmentService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenEnvironmentService::getUserName( Char * const _userName ) const
    {
        m_userName.copy( _userName );
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenEnvironmentService::getDeviceModel( Char * const _deviceModel ) const
    {
        m_deviceModel.copy( _deviceModel );
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenEnvironmentService::getOSFamily( Char * const _osFamily ) const
    {
        m_osFamily.copy( _osFamily );
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenEnvironmentService::getOSVersion( Char * const _osVersion ) const
    {
        m_osVersion.copy( _osVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenEnvironmentService::getBundleId( Char * const _bundleId ) const
    {
        m_bundleId.copy( _bundleId );
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenEnvironmentService::getInstallId( Char * const _installId ) const
    {
        m_installId.copy( _installId );
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp EmscriptenEnvironmentService::getInstallTimestamp() const
    {
        return m_installTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenEnvironmentService::getInstallVersion( Char * const _installVersion ) const
    {
        m_installVersion.copy( _installVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t EmscriptenEnvironmentService::getInstallRND() const
    {
        return m_installRND;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp EmscriptenEnvironmentService::getSessionTimestamp() const
    {
        return m_sessionTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t EmscriptenEnvironmentService::getSessionIndex() const
    {
        return m_sessionIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t EmscriptenEnvironmentService::getSessionRND() const
    {
        return m_sessionRND;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenEnvironmentService::getSessionId( Char * const _sessionId ) const
    {
        m_sessionId.copy( _sessionId );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t EmscriptenEnvironmentService::getSessionDateYear() const
    {
        return m_sessionDate.year;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t EmscriptenEnvironmentService::getSessionDateMonth() const
    {
        return m_sessionDate.month;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t EmscriptenEnvironmentService::getSessionDateDay() const
    {
        return m_sessionDate.day;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t EmscriptenEnvironmentService::getSessionDateHour() const
    {
        return m_sessionDate.hour;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t EmscriptenEnvironmentService::getSessionDateWeekday() const
    {
        return m_sessionDate.weekday;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t EmscriptenEnvironmentService::getSessionGap() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
