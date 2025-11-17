#pragma once

#include "Interface/PlatformServiceInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/FactoryInterface.h"
#include "Interface/AnalyticsEventProviderInterface.h"
#include "Interface/LoggerInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidPlatformServiceExtensionInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/SHA1.h"
#include "Kernel/Unknowable.h"
#include "Kernel/StaticString.h"

#include "Config/Timestamp.h"

#include <EGL/egl.h>

namespace Mengine
{
    class AndroidPlatformService
        : public ServiceBase<PlatformServiceInterface>
        , public AndroidPlatformServiceExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        AndroidPlatformService();
        ~AndroidPlatformService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        bool _runService() override;
        void _stopService() override;

    protected:
        bool initializeFileService() override;
        void finalizeFileService() override;

    public:
        bool runPlatform() override;
        void loopPlatform() override;
        bool updatePlatform() override;
        void tickPlatform( float _frameTime, bool _render, bool _flush, bool _pause ) override;
        void stopPlatform()	override;
        void freezePlatform( bool _tick, bool _render, bool _sound ) override;
        void unfreezePlatform( bool _tick, bool _render, bool _sound ) override;

    public:
        void setSleepMode( bool _sleepMode ) override;
        bool getSleepMode() const override;

    public:
        Timestamp getPlatfomTime() const override;

    public:
        void setProjectTitle( const Char * _projectTitle ) override;
        size_t getProjectTitle( Char * const _projectTitle ) const override;

    public:
        bool alreadyRunningMonitor() override;

    public:
        bool createWindow( const Resolution & _resolution, bool _fullscreen ) override;

    public:
        bool hasPlatformTag( const ConstString & _tag ) const override;
        const Tags & getPlatformTags() const override;

    public:
        bool isDesktop() const override;
        bool hasTouchpad() const override;

    public:
        int32_t dpToWidthPx( int32_t _dp ) const override;
        int32_t dpToHeightPx( int32_t _dp ) const override;

        int32_t pxToWidthDp( int32_t _px ) const override;
        int32_t pxToHeightDp( int32_t _px ) const override;

    public:
        DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc ) override;

    public:
        bool getDesktopResolution( Resolution * const _resolution ) const override;

        bool getNoFullscreen() const override;
        bool getAlwaysFullscreen() const override;

        bool isDebuggerPresent() const override;
        void debugBreak() override;

        size_t getCurrentPath( Char * const _currentPath ) const override;
        size_t getUserPath( Char * const _userPath ) const override;
        size_t getExtraPreferencesFolderName( Char * const _folderName ) const override;
        bool getUserLocaleLanguage( Char * const _userLocaleLanguage ) const override;

        void closeWindow() override;
        void minimizeWindow() override;

        void setCursorPosition( const mt::vec2f & _pos ) override;
        void setCursorIcon( const ConstString & _icon ) override;
        bool hasCursorIcon( const ConstString & _icon ) const override;

        void showKeyboard() override;
        void hideKeyboard() override;
        bool isShowKeyboard() const override;

        bool notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
        void notifyVsyncChanged( bool _vsync ) override;
        void notifyCursorModeChanged( bool _mode ) override;
        bool notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer ) override;

    public:
        void onEvent( const ConstString & _event, const Params & _params ) override;

    public:
        size_t getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const override;

    public:
        bool getMaxClientResolution( Resolution * const _resolution ) const override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body ) override;
        bool openDeleteAccount() override;

    public:
        bool updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath ) override;

    public:
        bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _file, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _file, const void * _data, size_t _size ) override;

    protected:
        void messageBox( const Char * _caption, const Char * _format, ... ) const override;

    protected:
        bool setClipboardText( const Char * _value ) const override;
        bool getClipboardText( Char * _value, size_t _capacity ) const override;

    protected:
        size_t androidNativeGetAndroidId( Char * _androidId, size_t _capacity ) const override;

    protected:
        void androidNativeSurfaceCreatedEvent( ANativeWindow * _nativeWindow ) override;
        void androidNativeSurfaceDestroyedEvent() override;
        void androidNativeSurfaceChangedEvent( ANativeWindow * _nativeWindow, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate ) override;

    protected:
        void notifyBootstrapperInitializeBaseServices_();
        void notifyBootstrapperCreateApplication_();

    protected:
        ETouchCode acquireFingerIndex_( jint _fingerId );
        ETouchCode releaseFingerIndex_( jint _fingerId );
        ETouchCode getFingerIndex_( jint _fingerId ) const;

    protected:
        void androidNativeTouchEvent( jint _action, jint _pointerId, jfloat _x, jfloat _y, jfloat _pressure ) override;
        void androidNativeAccelerationEvent( jfloat _x, jfloat _y, jfloat _z ) override;
        void androidNativeKeyEvent( jboolean _isDown, jint _keyCode, jint _repeatCount ) override;
        void androidNativeTextEvent( jint _unicode ) override;
        void androidNativePauseEvent() override;
        void androidNativeResumeEvent() override;
        void androidNativeStopEvent() override;
        void androidNativeStartEvent() override;
        void androidNativeRestartEvent() override;
        void androidNativeDestroyEvent() override;
        void androidNativeFreezeEvent( bool _tick, bool _render ) override;
        void androidNativeUnfreezeEvent( bool _tick, bool _render ) override;
        void androidNativeClipboardChangedEvent() override;
        void androidNativeWindowFocusChangedEvent( jboolean _focus ) override;
        void androidNativeQuitEvent() override;
        void androidNativeLowMemoryEvent() override;
        void androidNativeTrimMemoryEvent( jint _level ) override;
        void androidNativeChangeLocale( const Mengine::Char * _language ) override;

    protected:
        bool createWindow_( const Resolution & _resolution, bool _fullscreen );
        bool applyWindow_();
        void destroyWindow_();

    protected:
        bool processEvents_();
        
    protected:
        void pushQuitEvent_();

    protected:
        void setActive_( float _x, float _y, bool _active );

    protected:
        struct PlatformUnionEvent
        {
            enum EPlatformEventType
            {
                PET_QUIT,
                PET_PAUSE,
                PET_RESUME,
                PET_STOP,
                PET_START,
                PET_RESTART,
                PET_DESTROY,
                PET_FREEZE,
                PET_UNFREEZE,
                PET_SURFACE_CREATE,
                PET_SURFACE_DESTROY,
                PET_SURFACE_CHANGED,
                PET_CLIPBOARD_CHANGED,
                PET_WINDOW_FOCUS_CHANGED,
                PET_LOW_MEMORY,
                PET_TRIM_MEMORY,
                PET_CHANGE_LOCALE,
            } type;

            struct PlatformQuitEvent
            {
                int32_t dummy;
            };

            struct PlatformPauseEvent
            {
                float x;
                float y;
            };

            struct PlatformResumeEvent
            {
                float x;
                float y;
            };

            struct PlatformStopEvent
            {
                int32_t dummy;
            };

            struct PlatformStartEvent
            {
                int32_t dummy;
            };

            struct PlatformRestartEvent
            {
                int32_t dummy;
            };

            struct PlatformDestroyEvent
            {
                int32_t dummy;
            };

            struct PlatformFreezeEvent
            {
                bool tick;
                bool render;
                bool sound;
            };

            struct PlatformUnfreezeEvent
            {
                bool tick;
                bool render;
                bool sound;
            };

            struct PlatformSurfaceCreateEvent
            {
                ANativeWindow * nativeWindow;
            };

            struct PlatformSurfaceDestroyEvent
            {
                int32_t dummy;
            };

            struct PlatformSurfaceChangedEvent
            {
                ANativeWindow * nativeWindow;
                jint surfaceWidth;
                jint surfaceHeight;
                jint deviceWidth;
                jint deviceHeight;
                jfloat rate;
            };

            struct PlatformClipboardChangedEvent
            {
                int32_t dummy;
            };

            struct PlatformWindowFocusChangedEvent
            {
                jboolean focus;
            };

            struct PlatformLowMemoryEvent
            {
                int32_t dummy;
            };

            struct PlatformTrimMemoryEvent
            {
                jint level;
            };

            struct PlatformChangeLocale
            {
                Char language[MENGINE_LOCALE_LANGUAGE_SIZE + 1];
            };

            union
            {
                PlatformQuitEvent quit;
                PlatformPauseEvent pause;
                PlatformResumeEvent resume;
                PlatformStopEvent stop;
                PlatformStartEvent start;
                PlatformRestartEvent restart;
                PlatformDestroyEvent destroy;
                PlatformFreezeEvent freeze;
                PlatformUnfreezeEvent unfreeze;
                PlatformSurfaceCreateEvent surfaceCreate;
                PlatformSurfaceDestroyEvent surfaceDestroy;
                PlatformSurfaceChangedEvent surfaceChanged;
                PlatformClipboardChangedEvent clipboardChanged;
                PlatformWindowFocusChangedEvent windowFocusChanged;
                PlatformLowMemoryEvent lowMemory;
                PlatformTrimMemoryEvent trimMemory;
                PlatformChangeLocale changeLocale;
            } data;
        };

    protected:
        void pauseEvent_( const PlatformUnionEvent::PlatformPauseEvent & _event );
        void resumeEvent_( const PlatformUnionEvent::PlatformResumeEvent & _event );
        void stopEvent_( const PlatformUnionEvent::PlatformStopEvent & _event );
        void startEvent_( const PlatformUnionEvent::PlatformStartEvent & _event );
        void restartEvent_( const PlatformUnionEvent::PlatformRestartEvent & _event );
        void destroyEvent_( const PlatformUnionEvent::PlatformDestroyEvent & _event );
        void freezeEvent_( const PlatformUnionEvent::PlatformFreezeEvent & _event );
        void unfreezeEvent_( const PlatformUnionEvent::PlatformUnfreezeEvent & _event );
        void surfaceCreateEvent_( const PlatformUnionEvent::PlatformSurfaceCreateEvent & _event );
        void surfaceDestroyEvent_( const PlatformUnionEvent::PlatformSurfaceDestroyEvent & _event );
        void surfaceChangedEvent_( const PlatformUnionEvent::PlatformSurfaceChangedEvent & _event );
        void clipboardChangedEvent_( const PlatformUnionEvent::PlatformClipboardChangedEvent & _event );
        void windowFocusChangedEvent_( const PlatformUnionEvent::PlatformWindowFocusChangedEvent & _event );
        void lowMemoryEvent_( const PlatformUnionEvent::PlatformLowMemoryEvent & _event );
        void trimMemoryEvent_( const PlatformUnionEvent::PlatformTrimMemoryEvent & _event );
        void changeLocaleEvent_( const PlatformUnionEvent::PlatformChangeLocale & _event );

    protected:
        void pushEvent( const PlatformUnionEvent & _event );
        bool hasEvent( PlatformUnionEvent::EPlatformEventType _type ) const;

    protected:
        Timestamp m_beginTime;

        Tags m_platformTags;

        AnalyticsEventProviderInterfacePtr m_analyticsEventProvider;

        FactoryInterfacePtr m_factoryDynamicLibraries;

        LoggerInterfacePtr m_proxyLogger;

        ThreadMutexInterfacePtr m_nativeWindowMutex;
        ThreadMutexInterfacePtr m_eglSurfaceMutex;

        enum EActivityState
        {
            EAS_INITIALIZE,
            EAS_RESUME,
            EAS_START,
            EAS_PAUSE,
            EAS_STOP,
            EAS_RESTART,
            EAS_FREEZE,
            EAS_DESTROY
        };

        AtomicInt32 m_activityState;

        ANativeWindow * m_nativeWindow;

        EGLDisplay m_eglDisplay;
        EGLSurface m_eglSurface;
        EGLContext m_eglContext;

        jint m_fingers[MENGINE_INPUT_MAX_TOUCH];

        jfloat m_currentFingersX[MENGINE_INPUT_MAX_TOUCH];
        jfloat m_currentFingersY[MENGINE_INPUT_MAX_TOUCH];
        jfloat m_currentFingersPressure[MENGINE_INPUT_MAX_TOUCH];

        ThreadMutexInterfacePtr m_eventsMutex;

        typedef Vector<PlatformUnionEvent> VectorInputEvents;
        VectorInputEvents m_eventsAux;
        VectorInputEvents m_events;

        StaticString<MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME> m_projectTitle;

        double m_prevTime;

        float m_pauseUpdatingTime;

        bool m_active;
        
        AtomicInt32 m_freezedTick;
        AtomicInt32 m_freezedRender;
        AtomicInt32 m_freezedSound;

        bool m_desktop;
        bool m_touchpad;
    };
}
