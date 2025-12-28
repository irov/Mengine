#pragma once

#include "Interface/GameServiceInterface.h"
#include "Interface/SettingInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/AnalyticsEventProviderInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseLifecycle.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GameService
        : public ServiceBase<GameServiceInterface>
        , public AnalyticsEventProviderInterface
        , protected BaseEventation
        , protected BaseLifecycle
    {
        DECLARE_EVENTABLE();
        DECLARE_LIFECYCLEABLE();

    public:
        GameService();
        ~GameService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool preparation() override;
        void run() override;
        void interruption() override;
        void stop() override;

    protected:
        void _update() override;

    public:
        void render( const RenderPipelineInterfacePtr & _renderPipeline ) override;

    public:
        void initializeRenderResources() override;
        void finalizeRenderResources() override;

    public:
        bool loadPersonality() override;
        void removePersonality() override;

    public:
        void setCursorMode( bool _mode ) override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    public:
        bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    public:
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    public:
        void setFocus( bool _focus ) override;
        void setFullscreen( bool _fullscreen ) override;
        void setFixedContentResolution( bool _fixed ) override;
        void setFixedDisplayResolution( bool _fixed ) override;
        void setFixedViewportResolution( bool _fixed ) override;
        void setRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) override;
        void setGameViewport( const Viewport & _viewport, float _aspect ) override;

        bool close() override;

        void userEvent( const ConstString & _id, const Params & _params ) override;

        void turnSound( bool _turn ) override;

    protected:
        void notifyTimeFactorChange_( float _timeFactor );
        void notifySettingChange_( const SettingInterfacePtr & _setting, const Char * _key );

        void notifyApplicationDidBecomeActive_();
        void notifyApplicationWillEnterForeground_();
        void notifyApplicationDidEnterBackground_();
        void notifyApplicationWillResignActive_();
        void notifyApplicationWillTerminate_();

    protected:
        void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) override;
        void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) override;
        void onAnalyticsFlush() override;

    protected:
        ConstString m_currentPackName;
        String m_currentResourcePath;

        FilePath m_iconPath;

        struct UserEvent
        {
            ConstString id;
            Params params;
        };

        typedef Vector<UserEvent> VectorUserEvents;
        VectorUserEvents m_userEventsAdd;
        VectorUserEvents m_userEvents;
    };
}
