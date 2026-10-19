#include "AndroidWebViewPlatformService.h"

#include "Interface/WebViewBridgeServiceInterface.h"

#include "Environment/Android/AndroidPluginHelper.h"
#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidJSONHelper.h"
#include "Environment/Android/AndroidAssetServiceInterface.h"

#include "Kernel/Base64.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/StringHelper.h"

#include "Config/Path.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( WebViewPlatformService, Mengine::AndroidWebViewPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const size_t WEBVIEW_MAX_PENDING_SNAPSHOTS = 128;
    //////////////////////////////////////////////////////////////////////////
    AndroidWebViewPlatformService::AndroidWebViewPlatformService()
        : m_snapshotSequence( 0 )
        , m_attached( false )
        , m_canGoBack( false )
        , m_canGoForward( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidWebViewPlatformService::~AndroidWebViewPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewPlatformService::_initializeService()
    {
        WebViewBridgeSenderInterfacePtr sender = WebViewBridgeSenderInterfacePtr::from( this );

        WEBVIEW_BRIDGE_SERVICE()
            ->setSender( sender );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidWebViewPlatformService::_finalizeService()
    {
        this->close();

        WEBVIEW_BRIDGE_SERVICE()
            ->setSender( nullptr );

        m_externalNavigation = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidWebViewPlatformService::postCommand( const jpp::object & _command ) const
    {
        jpp::object envelope = jpp::make_object();
        envelope.set( "binding", m_binding );
        envelope.set( "command", _command );

        Data json;
        if( Helper::writeJSONDataCompact( envelope, &json ) == false )
        {
            LOGGER_ERROR( "failed to serialize WebView command" );

            return;
        }

        JNIEnv * env = Mengine_JNI_GetEnv();
        jsize size = static_cast<jsize>(json.size());
        jbyteArray value = Mengine_JNI_NewByteArray( env, size );

        if( value == nullptr )
        {
            return;
        }

        const uint8_t * jsonBuffer = json.data();
        const jbyte * bytes = reinterpret_cast<const jbyte *>(jsonBuffer);
        Mengine_JNI_SetByteArrayRegion( env, value, 0, size, bytes );

        Helper::AndroidCallVoidPluginMethod( env, "AndroidWebViewPlugin", "postWebViewCommand", "([B)V", value );

        Mengine_JNI_DeleteLocalRef( env, value );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidWebViewPlatformService::cancelDocument_()
    {
        WEBVIEW_BRIDGE_SERVICE()
            ->closeDocument();

        m_document.clear();
        m_canGoBack = false;
        m_canGoForward = false;

        MapSnapshots snapshots;
        snapshots.swap( m_snapshots );

        for( const auto & [id, callback] : snapshots )
        {
            MENGINE_UNUSED( id );

            Data data;
            callback( data, 0, 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidWebViewPlatformService::processEvent( JNIEnv * _jenv, jclass _jclass, EAndroidWebViewEvent _id, jobject _event )
    {
        switch( _id )
        {
        case EAndroidWebViewEvent::ATTACH:
            {
                this->cancelDocument_();

                m_binding = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, _event, "binding", "" );
                m_canGoBack = false;
                m_canGoForward = false;
                m_attached = true;
            }break;
        case EAndroidWebViewEvent::DETACH:
            {
                m_attached = false;

                this->cancelDocument_();
            }break;
        case EAndroidWebViewEvent::NAVIGATION:
            {
                this->cancelDocument_();

                m_document = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, _event, "document", "" );

                WEBVIEW_BRIDGE_SERVICE()
                    ->beginDocument();
            }break;
        case EAndroidWebViewEvent::FAILED:
            {
                String document = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, _event, "document", "" );

                if( document != m_document )
                {
                    return;
                }

                this->cancelDocument_();
            }break;
        case EAndroidWebViewEvent::LOADED:
            {
                String document = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, _event, "document", "" );

                if( document != m_document )
                {
                    return;
                }

                if( m_attached == false )
                {
                    return;
                }

                uint64_t generation = WEBVIEW_BRIDGE_SERVICE()
                    ->getDocumentGeneration();
                String generationString = Helper::stringUInt64( generation );

                jpp::object command = jpp::make_object();
                command.set( "kind", "connect" );
                command.set( "document", m_document );
                command.set( "generation", generationString );

                this->postCommand( command );
            }break;
        case EAndroidWebViewEvent::MESSAGE:
            {
                String document = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, _event, "document", "" );

                if( document != m_document )
                {
                    return;
                }

                if( m_attached == false )
                {
                    return;
                }

                jobject javaMessage = Helper::AndroidGetJavaJSONObjectObject( _jenv, _jclass, _event, "message" );

                if( javaMessage == nullptr )
                {
                    return;
                }

                uint64_t generation = WEBVIEW_BRIDGE_SERVICE()
                    ->getDocumentGeneration();

                String expectedGeneration = Helper::stringUInt64( generation );
                String messageGeneration = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, javaMessage, "generation", "" );

                if( messageGeneration != expectedGeneration )
                {
                    Mengine_JNI_DeleteLocalRef( _jenv, javaMessage );

                    return;
                }

                jpp::object message;
                bool successful = Helper::AndroidMakeJSONFromJObject( _jenv, javaMessage, &message );

                Mengine_JNI_DeleteLocalRef( _jenv, javaMessage );

                if( successful == false )
                {
                    LOGGER_ERROR( "invalid WebView message" );

                    return;
                }

                WEBVIEW_BRIDGE_SERVICE()
                    ->receiveMessage( generation, message );

                bool appReady = WEBVIEW_BRIDGE_SERVICE()
                    ->isAppReady();

                if( appReady == false )
                {
                    return;
                }

                jpp::object command = jpp::make_object();
                command.set( "kind", "appReady" );
                command.set( "document", m_document );

                this->postCommand( command );
            }break;
        case EAndroidWebViewEvent::HISTORY:
            {
                String document = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, _event, "document", "" );

                if( document != m_document )
                {
                    return;
                }

                jobject history = Helper::AndroidGetJavaJSONObjectObject( _jenv, _jclass, _event, "history" );

                m_canGoBack = Helper::AndroidGetJavaJSONObjectBoolean( _jenv, _jclass, history, "back", false );
                m_canGoForward = Helper::AndroidGetJavaJSONObjectBoolean( _jenv, _jclass, history, "forward", false );

                Mengine_JNI_DeleteLocalRef( _jenv, history );
            }break;
        case EAndroidWebViewEvent::SNAPSHOT:
            {
                String document = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, _event, "document", "" );

                if( document != m_document )
                {
                    return;
                }

                jobject snapshot = Helper::AndroidGetJavaJSONObjectObject( _jenv, _jclass, _event, "snapshot" );

                if( snapshot == nullptr )
                {
                    return;
                }

                String id = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, snapshot, "id", "" );

                MapSnapshots::iterator it = m_snapshots.find( id );

                if( it == m_snapshots.end() )
                {
                    Mengine_JNI_DeleteLocalRef( _jenv, snapshot );

                    return;
                }

                String encoded = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, snapshot, "data", "" );
                jint javaWidth = Helper::AndroidGetJavaJSONObjectInt( _jenv, _jclass, snapshot, "width", 0 );
                jint javaHeight = Helper::AndroidGetJavaJSONObjectInt( _jenv, _jclass, snapshot, "height", 0 );

                Mengine_JNI_DeleteLocalRef( _jenv, snapshot );

                uint32_t width = static_cast<uint32_t>(javaWidth);
                uint32_t height = static_cast<uint32_t>(javaHeight);

                LambdaSnapshot callback = it->second;
                m_snapshots.erase( it );

                Data data;
                if( Helper::decodeBase64( encoded, &data ) == false )
                {
                    data.clear();
                }

                callback( data, width, height );
            }break;
        case EAndroidWebViewEvent::EXTERNAL:
            {
                if( m_externalNavigation == nullptr )
                {
                    break;
                }

                String url = Helper::AndroidGetJavaJSONObjectString( _jenv, _jclass, _event, "url", "" );
                m_externalNavigation( url );
            }break;
        default:
            {
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewPlatformService::openContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _entry )
    {
        if( _fileGroup == nullptr )
        {
            return false;
        }

        if( _entry.empty() == true )
        {
            return false;
        }

        if( _fileGroup->existFile( _entry, false ) == false )
        {
            return false;
        }

        Path path = {'\0'};
        _fileGroup->getFullPath( _entry, path );

        if( m_attached == false )
        {
            return false;
        }

        if( ANDROID_ASSET_SERVICE()
            ->existFile( path ) == false )
        {
            return false;
        }

        jpp::object command = jpp::make_object();
        command.set( "kind", "openAsset" );
        command.set( "path", path );

        this->postCommand( command );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewPlatformService::openURL( const String & _url )
    {
        if( m_attached == false )
        {
            return false;
        }

        const Char httpsScheme[] = "https://";
        size_t httpsSchemeLength = sizeof( httpsScheme ) - 1;

        if( _url.compare( 0, httpsSchemeLength, httpsScheme ) != 0 )
        {
            return false;
        }

        jpp::object command = jpp::make_object();
        command.set( "kind", "open" );
        command.set( "url", _url );

        this->postCommand( command );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidWebViewPlatformService::reload()
    {
        jpp::object command = jpp::make_object();
        command.set( "kind", "reload" );

        this->postCommand( command );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewPlatformService::goBack()
    {
        if( m_attached == false )
        {
            return false;
        }

        if( m_canGoBack == false )
        {
            return false;
        }

        jpp::object command = jpp::make_object();
        command.set( "kind", "back" );

        this->postCommand( command );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewPlatformService::goForward()
    {
        if( m_attached == false )
        {
            return false;
        }

        if( m_canGoForward == false )
        {
            return false;
        }

        jpp::object command = jpp::make_object();
        command.set( "kind", "forward" );

        this->postCommand( command );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewPlatformService::sendJSON( const jpp::object & _message )
    {
        if( m_attached == false )
        {
            return false;
        }

        if( m_document.empty() == true )
        {
            return false;
        }

        bool ready = WEBVIEW_BRIDGE_SERVICE()
            ->isDocumentReady();

        if( ready == false )
        {
            return false;
        }

        if( _message.is_type_object() == false )
        {
            return false;
        }

        jpp::object command = jpp::make_object();
        command.set( "kind", "send" );
        command.set( "document", m_document );

        jpp::object message = jpp::copy( _message );
        uint64_t generation = WEBVIEW_BRIDGE_SERVICE()
            ->getDocumentGeneration();

        String generationString = Helper::stringUInt64( generation );
        message.set( "generation", generationString );
        command.set( "message", message );

        this->postCommand( command );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object AndroidWebViewPlatformService::getCapabilities() const
    {
        jpp::object capabilities = jpp::make_object();
        capabilities.set( "snapshot", true );
        capabilities.set( "persistentStorage", true );
        capabilities.set( "localOrigin", "https://mengine.localhost" );

        return capabilities;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidWebViewPlatformService::takeSnapshot( const LambdaSnapshot & _callback )
    {
        if( m_attached == false )
        {
            Data data;
            _callback( data, 0, 0 );

            return;
        }

        if( m_document.empty() == true )
        {
            Data data;
            _callback( data, 0, 0 );

            return;
        }

        if( m_snapshots.size() >= WEBVIEW_MAX_PENDING_SNAPSHOTS )
        {
            Data data;
            _callback( data, 0, 0 );

            return;
        }

        ++m_snapshotSequence;
        String id = Helper::stringUInt64( m_snapshotSequence );
        m_snapshots.emplace( id, _callback );

        jpp::object command = jpp::make_object();
        command.set( "kind", "snapshot" );
        command.set( "document", m_document );
        command.set( "id", id );

        this->postCommand( command );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidWebViewPlatformService::setExternalNavigationCallback( const LambdaExternalNavigation & _callback )
    {
        m_externalNavigation = _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidWebViewPlatformService::close()
    {
        m_attached = false;

        this->cancelDocument_();

        jpp::object command = jpp::make_object();
        command.set( "kind", "close" );

        this->postCommand( command );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidWebViewPlatformService::onWebViewBridgeSenderSendJSON( const jpp::object & _message )
    {
        if( this->sendJSON( _message ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
