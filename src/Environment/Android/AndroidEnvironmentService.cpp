#include "AndroidEnvironmentService.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidActivityHelper.h"
#include "Environment/Android/AndroidApplicationHelper.h"

#include "Kernel/SHA1.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"
#include "Config/StdIntTypes.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( EnvironmentService, Mengine::AndroidEnvironmentService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<size_t N>
        static void getAndroidInfo( JNIEnv * _jenv, const Char * _method, StaticString<N> * const _info )
        {
            jstring jinfo = (jstring)Helper::AndroidCallObjectApplicationMethod( _jenv, _method, "()Ljava/lang/String;" );

            Helper::AndroidCopyStringFromJString( _jenv, jinfo, _info );

            _jenv->DeleteLocalRef( jinfo );
        }
        //////////////////////////////////////////////////////////////////////////
        static void getAndroidInfo( JNIEnv * _jenv, const Char * _method, int64_t * const _info )
        {
            jlong jinfo = Helper::AndroidCallLongApplicationMethod( _jenv, _method, "()J" );

            *_info = (int64_t)jinfo;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidEnvironmentService::AndroidEnvironmentService()
        : m_installTimestamp( 0 )
        , m_installRND( 0 )
        , m_sessionIndex( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidEnvironmentService::~AndroidEnvironmentService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidEnvironmentService::_initializeService()
    {
        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Detail::getAndroidInfo( jenv, "getDeviceModel", &m_deviceModel );
        Detail::getAndroidInfo( jenv, "getDeviceLanguage", &m_deviceLanguage );
        Detail::getAndroidInfo( jenv, "getOSVersion", &m_osVersion );
        Detail::getAndroidInfo( jenv, "getPackageName", &m_bundleId );
        Detail::getAndroidInfo( jenv, "getSessionId", &m_sessionId );
        Detail::getAndroidInfo( jenv, "getInstallKey", &m_installKey );
        Detail::getAndroidInfo( jenv, "getInstallVersion", &m_installVersion );

        Detail::getAndroidInfo( jenv, "getInstallTimestamp", &m_installTimestamp );
        Detail::getAndroidInfo( jenv, "getInstallRND", &m_installRND );
        Detail::getAndroidInfo( jenv, "getSessionIndex", &m_sessionIndex );

        m_osVersion.assign( "Android" );

        StaticString<256> androidId;
        Detail::getAndroidInfo( jenv, "getAndroidId", &androidId );

        StaticString<MENGINE_ENVIRONMENT_USER_MAXNAME + MENGINE_ENVIRONMENT_DEVICE_MODEL_MAXNAME + 256 + 64> fingerprintGarbage;

        fingerprintGarbage.assign( "FINGERPRINT" );
        fingerprintGarbage.append( "_" );
        fingerprintGarbage.append( m_userName );
        fingerprintGarbage.append( "_" );
        fingerprintGarbage.append( m_deviceModel );
        fingerprintGarbage.append( "_" );
        fingerprintGarbage.append( androidId );

        Helper::makeSHA1HEX( fingerprintGarbage.data(), fingerprintGarbage.size(), m_fingerprint.data(), false );

        m_userName.assign( m_deviceModel );
        m_userName.append( ' ' );
        m_userName.append( m_fingerprint.data(), 6 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::getUserName( Char * const _deviceName ) const
    {
        m_userName.copy( _deviceName );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::getDeviceModel( Char * const _deviceModel ) const
    {
        m_deviceModel.copy( _deviceModel );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::getDeviceLanguage( Char * const _deviceLanguage ) const
    {
        m_deviceLanguage.copy( _deviceLanguage );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::getOSFamily( Char * _osFamily ) const
    {
        m_osFamily.copy( _osFamily );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::getOSVersion( Char * const _osVersion ) const
    {
        m_osVersion.copy( _osVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::getBundleId( Char * const _bundleId ) const
    {
        m_bundleId.copy( _bundleId );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::getFingerprint( Char * const _fingerprint ) const
    {
        m_fingerprint.copy( _fingerprint );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::getSessionId( Char * const _sessionId ) const
    {
        m_sessionId.copy( _sessionId );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::getInstallKey( Char * const _installKey ) const
    {
        m_installKey.copy( _installKey );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidEnvironmentService::getInstallTimestamp() const
    {
        return m_installTimestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidEnvironmentService::getInstallVersion( Char * const _installVersion ) const
    {
        m_installVersion.copy( _installVersion );
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidEnvironmentService::getInstallRND() const
    {
        return m_installRND;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t AndroidEnvironmentService::getSessionIndex() const
    {
        return m_sessionIndex;
    }
    //////////////////////////////////////////////////////////////////////////
}