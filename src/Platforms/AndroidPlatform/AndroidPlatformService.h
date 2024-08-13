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

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidPlatformServiceExtensionInterface.h"

#include "AndroidInput.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/SHA1.h"
#include "Kernel/Unknowable.h"

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
        bool runPlatform()	override;
        void loopPlatform() override;
        bool updatePlatform() override;
        bool tickPlatform( float _time, bool _render, bool _flush, bool _pause ) override;
        void stopPlatform()	override;

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

    public:
        bool existDirectory( const Char * _path, const Char * _directory ) const override;
        bool createDirectory( const Char * _path, const Char * _directory ) override;

    public:
        bool existFile( const Char * _filePath ) override;
        bool removeFile( const Char * _filePath ) override;
        bool moveFile( const Char * _oldFilePath, const Char * _newFilePath ) override;

    public:
        bool findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        uint64_t getFileTime( const Char * _filePath ) const override;

    public:
        bool updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath ) override;

    public:
        bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _file, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _file, const void * _data, size_t _size ) override;

    public:
        void sleep( uint32_t _ms ) override;

    protected:
        void messageBox( const Char * _caption, const Char * _format, ... ) const override;

    protected:
        bool setClipboardText( const Char * _value ) const override;
        bool getClipboardText( Char * _value, size_t _capacity ) const override;

    protected:
        void androidNativeSurfaceCreated( ANativeWindow * _nativeWindow ) override;
        void androidNativeSurfaceDestroyed( ANativeWindow * _nativeWindow ) override;
        void androidNativeSurfaceChanged( ANativeWindow * _nativeWindow, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate ) override;

    protected:
        ETouchCode acquireFingerIndex_( jint _fingerId );
        ETouchCode releaseFingerIndex_( jint _fingerId );
        ETouchCode getFingerIndex_( jint _fingerId ) const;

    protected:
        void androidNativeTouchEvent( jint _action, jint _pointerId, jfloat _x, jfloat _y, jfloat _pressure ) override;

    protected:
        void androidNativeAccelerationEvent( jfloat _x, jfloat _y, jfloat _z ) override;

    protected:
        void androidNativeKeyEvent( jboolean _isDown, jint _keyCode, jint _repeatCount ) override;

    protected:
        void androidNativeTextEvent( jint _unicode ) override;

    protected:
        void androidNativePauseEvent() override;
        void androidNativeResumeEvent() override;

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
                PET_PAUSE,
                PET_RESUME,
                PET_SURFACE_CREATE,
                PET_SURFACE_DESTROY,
                PET_SURFACE_CHANGED,
            } type;

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

            struct PlatformSurfaceCreateEvent
            {
                ANativeWindow * nativeWindow;
            };

            struct PlatformSurfaceDestroyEvent
            {
                ANativeWindow * nativeWindow;
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

            union
            {
                PlatformPauseEvent pause;
                PlatformResumeEvent resume;
                PlatformSurfaceCreateEvent surfaceCreate;
                PlatformSurfaceDestroyEvent surfaceDestroy;
                PlatformSurfaceChangedEvent surfaceChanged;
            } data;
        };

    protected:
        void pauseEvent_( const PlatformUnionEvent::PlatformPauseEvent & _event );
        void resumeEvent_( const PlatformUnionEvent::PlatformResumeEvent & _event );
        void surfaceCreateEvent_( const PlatformUnionEvent::PlatformSurfaceCreateEvent & _event );
        void surfaceDestroyEvent_( const PlatformUnionEvent::PlatformSurfaceDestroyEvent & _event );
        void surfaceChangedEvent_( const PlatformUnionEvent::PlatformSurfaceChangedEvent & _event );

    protected:
        void pushEvent( const PlatformUnionEvent & _event );

    protected:
        Timestamp m_beginTime;

        Tags m_platformTags;

        ThreadMutexInterfacePtr m_nativeMutex;

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

        Timestamp m_prevTime;

        float m_pauseUpdatingTime;

        bool m_active;
        bool m_sleepMode;

        bool m_desktop;
        bool m_touchpad;
    };
}
