#pragma once

#include "Interface/WebViewPlatformServiceInterface.h"
#include "Interface/WebViewBridgeSenderInterface.h"

#include "Environment/Android/AndroidWebViewPlatformServiceExtensionInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Map.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidWebViewPlatformService
        : public ServiceBase<WebViewPlatformServiceInterface>
        , public WebViewBridgeSenderInterface
        , public AndroidWebViewPlatformServiceExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        AndroidWebViewPlatformService();
        ~AndroidWebViewPlatformService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void processEvent( JNIEnv * _jenv, jclass _jclass, EAndroidWebViewEvent _id, jobject _event ) override;
        void postCommand( const jpp::object & _command ) const override;

    public:
        bool openContent( const FileGroupInterfacePtr & _fileGroup, const FilePath & _entry ) override;
        bool openURL( const String & _url ) override;
        void reload() override;
        bool goBack() override;
        bool goForward() override;
        bool sendJSON( const jpp::object & _message ) override;
        jpp::object getCapabilities() const override;
        void takeSnapshot( const LambdaSnapshot & _callback ) override;
        void setExternalNavigationCallback( const LambdaExternalNavigation & _callback ) override;
        void close() override;

    protected:
        bool onWebViewBridgeSenderSendJSON( const jpp::object & _message ) override;

    protected:
        typedef Map<String, LambdaSnapshot> MapSnapshots;

    protected:
        void cancelDocument_();

    protected:
        String m_document;
        String m_binding;
        uint64_t m_snapshotSequence;
        bool m_attached;
        bool m_canGoBack;
        bool m_canGoForward;
        LambdaExternalNavigation m_externalNavigation;
        MapSnapshots m_snapshots;
    };
    //////////////////////////////////////////////////////////////////////////
}
