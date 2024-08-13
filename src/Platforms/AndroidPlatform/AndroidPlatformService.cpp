#include "AndroidPlatformService.h"

#include "Interface/LoggerInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/DateTimeSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/EnvironmentServiceInterface.h"
#include "Interface/AndroidKernelServiceInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidDeclaration.h"
#include "Environment/Android/AndroidHelper.h"
#include "Environment/Android/AndroidPlatformServiceExtensionInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/PathHelper.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"
#include "Kernel/StringHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/RandomDevice.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"
#include "Config/Algorithm.h"
#include "Config/Switch.h"

#include <clocale>
#include <ctime>
#include <iomanip>

#include <sys/stat.h>
#include <dlfcn.h>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SETLOCALE_ENABLE
#define MENGINE_SETLOCALE_ENABLE 1
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SETLOCALE_VALUE
#define MENGINE_SETLOCALE_VALUE "C"
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_DEVELOPMENT_USER_FOLDER_NAME
#define MENGINE_DEVELOPMENT_USER_FOLDER_NAME "User"
#endif
//////////////////////////////////////////////////////////////////////////
extern "C"
{
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1surfaceCreated )(JNIEnv * env, jclass cls, jobject surface)
    {
        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( env, surface );

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeSurfaceCreated( nativeWindow );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1surfaceDestroyed )(JNIEnv * env, jclass cls, jobject surface)
    {
        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( env, surface );

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeSurfaceDestroyed( nativeWindow );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1surfaceChanged )(JNIEnv * env, jclass cls, jobject surface, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate)
    {
        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( env, surface );

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeSurfaceChanged( nativeWindow, surfaceWidth, surfaceHeight, deviceWidth, deviceHeight, rate );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1keyEvent )(JNIEnv * env, jclass cls, jboolean isDown, jint keyCode, jint repeatCount)
    {
        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
                ->getUnknown();

        platformExtension->androidNativeKeyEvent( isDown, keyCode, repeatCount );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1textEvent )(JNIEnv * env, jclass cls, jint unicode)
    {
        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeTextEvent( unicode );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1touchEvent )(JNIEnv * env, jclass cls, jint action, jint pointerId, jfloat x, jfloat y, jfloat pressure)
    {
        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeTouchEvent( action, pointerId, x, y, pressure );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1accelerationEvent )(JNIEnv * env, jclass cls, jfloat x, jfloat y, jfloat z)
    {
        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeAccelerationEvent( x, y, z );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1pauseEvent )(JNIEnv * env, jclass cls)
    {
        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativePauseEvent();
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1resumeEvent )(JNIEnv * env, jclass cls)
    {
        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->androidNativeResumeEvent();
    }
    ///////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformService, Mengine::AndroidPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidPlatformService::AndroidPlatformService()
        : m_beginTime( 0 )
        , m_nativeWindow( nullptr )
        , m_eglDisplay( EGL_NO_DISPLAY )
        , m_eglSurface( EGL_NO_SURFACE )
        , m_eglContext( EGL_NO_CONTEXT )
        , m_prevTime( 0 )
        , m_pauseUpdatingTime( -1.f )
        , m_active( false )
        , m_sleepMode( true )
        , m_desktop( false )
        , m_touchpad( false )
    {
        Algorithm::fill_n( m_fingers, MENGINE_INPUT_MAX_TOUCH, -1 );

        Algorithm::fill_n( m_currentFingersX, MENGINE_INPUT_MAX_TOUCH, 0.f );
        Algorithm::fill_n( m_currentFingersY, MENGINE_INPUT_MAX_TOUCH, 0.f );
        Algorithm::fill_n( m_currentFingersPressure, MENGINE_INPUT_MAX_TOUCH, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidPlatformService::~AndroidPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    size_t AndroidPlatformService::getCurrentPath( Char * const _currentPath ) const
    {
        _currentPath[0] = L'\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidPlatformService::getUserPath( Char * const _userPath ) const
    {
        if( Mengine_JNI_ExistMengineActivity() == JNI_FALSE )
        {
            LOGGER_ERROR( "invalid get user path [not exist activity]" );

            return 0;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        if( Helper::AndroidGetActivityFilesDirCanonicalPath( jenv, _userPath ) == false )
        {
            LOGGER_ERROR( "invalid get user path" );

            return 0;
        }

        Char extraPreferencesFolderName[MENGINE_MAX_PATH] = {'\0'};
        size_t ExtraPreferencesFolderNameLen = this->getExtraPreferencesFolderName( extraPreferencesFolderName );

        if( ExtraPreferencesFolderNameLen != 0 )
        {
            StdString::strcat( _userPath, extraPreferencesFolderName );
            StdString::strcat( _userPath, "/" );
        }

        size_t userPathLen = StdString::strlen( _userPath );

        return userPathLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidPlatformService::getExtraPreferencesFolderName( Char * const _folderName ) const
    {
        const Char * Project_ExtraPreferencesFolderName = CONFIG_VALUE( "Project", "ExtraPreferencesFolderName", "" );

        StdString::strcpy( _folderName, Project_ExtraPreferencesFolderName );
        
        size_t Project_ExtraPreferencesFolderNameLen = StdString::strlen( Project_ExtraPreferencesFolderName );

        return Project_ExtraPreferencesFolderNameLen;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidPlatformService::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        MENGINE_UNUSED( _fontName );
        MENGINE_UNUSED( _fontPath );
        MENGINE_UNUSED( _groupName );

        _fontPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getMaxClientResolution( Resolution * const _resolution ) const
    {
        MENGINE_UNUSED( _resolution );

        m_nativeMutex->lock();
        int32_t width = ANativeWindow_getWidth( m_nativeWindow );
        int32_t height = ANativeWindow_getHeight( m_nativeWindow );
        m_nativeMutex->unlock();

        *_resolution = Resolution( width, height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::_initializeService()
    {
        m_beginTime = Helper::getSystemTimestamp();

#if defined(MENGINE_SETLOCALE_ENABLE)
        ::setlocale( LC_ALL, MENGINE_SETLOCALE_VALUE );
#endif

        m_platformTags.clear();

        m_touchpad = true;
        m_desktop = false;

        m_platformTags.addTag( STRINGIZE_STRING_LOCAL( "ANDROID" ) );

        Char platformTags[1024] = {'\0'};

        for( const ConstString & tag : m_platformTags.getValues() )
        {
            const Char * tag_str = tag.c_str();

            StdString::strcat( platformTags, tag_str );
            StdString::strcat( platformTags, "-" );
        }

        LOGGER_INFO( "platform", "platform tags: %s"
            , platformTags
        );

        LOGGER_INFO( "platform", "touchpad: %u"
            , m_touchpad
        );

        LOGGER_INFO( "platform", "desktop: %u"
            , m_desktop
        );

        uint32_t deviceSeed = Helper::generateRandomDeviceSeed();

        LOGGER_INFO_PROTECTED( "plarform", "Device Seed: %u"
            , deviceSeed
        );

        m_nativeMutex = Helper::createThreadMutex(MENGINE_DOCUMENT_FACTORABLE );
        m_eventsMutex = Helper::createThreadMutex(MENGINE_DOCUMENT_FACTORABLE );

        EGLDisplay eglDisplay = ::eglGetDisplay( EGL_DEFAULT_DISPLAY );

        EGLint major;
        EGLint minor;
        if( ::eglInitialize( eglDisplay, &major, &minor ) == EGL_FALSE )
        {
            return false;
        }

        m_eglDisplay = eglDisplay;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::_runService()
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        LOGGER_INFO( "platform", "start date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::_stopService()
    {
        PlatformDateTime dateTime;
        DATETIME_SYSTEM()
            ->getLocalDateTime( &dateTime );

        LOGGER_INFO( "platform", "stop date: %02u.%02u.%u, %02u:%02u:%02u"
            , dateTime.day
            , dateTime.month
            , dateTime.year
            , dateTime.hour
            , dateTime.minute
            , dateTime.second
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::_finalizeService()
    {
        m_active = false;

        this->destroyWindow_();

        m_platformTags.clear();

        m_eventsMutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::runPlatform()
    {
        this->setActive_( 0, 0, true );

        if( this->updatePlatform() == false )
        {
            return false;
        }

        if( this->tickPlatform( 0.f, false, false, false ) == false )
        {
            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::tickPlatform( float _time, bool _render, bool _flush, bool _pause )
    {
        MENGINE_UNUSED( _pause );
        MENGINE_UNUSED( _flush );

        if( m_eglSurface == EGL_NO_SURFACE || m_eglContext == EGL_NO_CONTEXT )
        {
            return true;
        }

        bool updating = APPLICATION_SERVICE()
            ->beginUpdate( _time );

        if( updating == true )
        {
            if( m_pauseUpdatingTime >= 0.f )
            {
                _time = m_pauseUpdatingTime;
                m_pauseUpdatingTime = -1.f;
            }

            APPLICATION_SERVICE()
                ->tick( _time );
        }

        if( _render == true )
        {
            bool sucessful = APPLICATION_SERVICE()
                ->render();

            if( sucessful == true && _flush == true )
            {
                APPLICATION_SERVICE()
                    ->flush();

                ::eglSwapBuffers( m_eglDisplay, m_eglSurface );
            }
        }

        APPLICATION_SERVICE()
            ->endUpdate();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::loopPlatform()
    {
        m_prevTime = Helper::getSystemTimestamp();

        for( ;; )
        {
            Timestamp currentTime = Helper::getSystemTimestamp();

            if( this->updatePlatform() == false )
            {
                break;
            }

            float frameTime = (float)(currentTime - m_prevTime);

            m_prevTime = currentTime;

            if( this->tickPlatform( frameTime, true, true, true ) == false )
            {
                break;
            }

            if( m_sleepMode == true )
            {
                Timestamp endTime = Helper::getSystemTimestamp();

                Timestamp deltaTime = endTime - currentTime;

                const Timestamp frameTime60Ms = 1000 / 60;

                if( deltaTime < frameTime60Ms )
                {
                    Timestamp sleepTime = frameTime60Ms - deltaTime;

                    THREAD_SYSTEM()
                        ->sleep( sleepTime );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::updatePlatform()
    {
        bool quitRequest = this->processEvents_();

        if( quitRequest == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::openUrlInDefaultBrowser( const Char * _url )
    {
        MENGINE_UNUSED( _url );

        LOGGER_INFO( "platform", "open url in default browser '%s'"
            , _url
        );

        if( ANDROID_KERNEL_SERVICE()
            ->openUrlInDefaultBrowser( _url ) == false )
        {
            LOGGER_ERROR( "error open url in default browser '%s'"
                , _url
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body )
    {
        MENGINE_UNUSED( _email );
        MENGINE_UNUSED( _subject );
        MENGINE_UNUSED( _body );

        LOGGER_INFO( "platform", "open mail '%s' subject '%s' body '%s'"
            , _email
            , _subject
            , _body
        );

        if( ANDROID_KERNEL_SERVICE()
            ->openMail( _email, _subject, _body ) == false )
        {
            LOGGER_ERROR( "error open mail '%s' subject '%s' body '%s'"
                , _email
                , _subject
                , _body
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::stopPlatform()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );

        /*
        if( m_sdlWindow != nullptr )
        {
            SDL_HideWindow( m_sdlWindow );
        }
         */
        
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setSleepMode( bool _sleepMode )
    {
        m_sleepMode = _sleepMode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getSleepMode() const
    {
        return m_sleepMode;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp AndroidPlatformService::getPlatfomTime() const
    {
        Timestamp currentTime = Helper::getSystemTimestamp();

        Timestamp platformTime = currentTime - m_beginTime;

        return platformTime;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setProjectTitle( const Char * _projectTitle )
    {
        if( _projectTitle == nullptr )
        {
            m_projectTitle.clear();

            return;
        }

        m_projectTitle.assign( _projectTitle );

        LOGGER_INFO( "platform", "project title: %s"
            , m_projectTitle.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t AndroidPlatformService::getProjectTitle( Char * const _projectTitle ) const
    {
        m_projectTitle.copy( _projectTitle );

        size_t len = m_projectTitle.size();

        return len;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::alreadyRunningMonitor()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::createWindow( const Resolution & _windowResolution, bool _fullscreen )
    {
        LOGGER_INFO( "platform", "create window size %u:%u fullscreen %d"
            , _windowResolution.getWidth()
            , _windowResolution.getHeight()
            , _fullscreen
        );

        if( this->createWindow_( _windowResolution, _fullscreen ) == false )
        {
            return false;
        }

        if( this->applyWindow_() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::applyWindow_()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_ATACH_WINDOW );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::hasPlatformTag( const ConstString & _tag ) const
    {
        bool exist = m_platformTags.hasTag( _tag );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & AndroidPlatformService::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::isDesktop() const
    {
        return m_desktop;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::hasTouchpad() const
    {
        return m_touchpad;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr AndroidPlatformService::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "platform", "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getDesktopResolution( Resolution * const _resolution ) const
    {
        m_nativeMutex->lock();
        int32_t width = ANativeWindow_getWidth( m_nativeWindow );
        int32_t height = ANativeWindow_getHeight( m_nativeWindow );
        m_nativeMutex->unlock();

        *_resolution = Resolution( width, height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getNoFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getAlwaysFullscreen() const
    {
#if defined(MENGINE_PLATFORM_IOS)
        return true;
#elif defined(MENGINE_PLATFORM_ANDROID)
        return true;
#endif

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::isDebuggerPresent() const
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        if( ::IsDebuggerPresent() == FALSE )
        {
            return false;
        }

        return true;
#elif defined(MENGINE_PLATFORM_ANDROID)
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jclass debugClass = jenv->FindClass("android/os/Debug");
        jmethodID isDebuggerConnectedMethod = jenv->GetStaticMethodID( debugClass, "isDebuggerConnected", "()Z" );

        jboolean isDebuggerConnected = jenv->CallStaticBooleanMethod( debugClass, isDebuggerConnectedMethod );

        Helper::AndroidEnvExceptionCheck( jenv );

        jenv->DeleteLocalRef( debugClass );

        if( isDebuggerConnected == JNI_FALSE )
        {
            return false;
        }

        return true;
#else

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::debugBreak()
    {
#if defined(MENGINE_PLATFORM_WINDOWS)
        ::DebugBreak();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::closeWindow()
    {
        /*
        if( m_sdlWindow != nullptr )
        {
            SDL_HideWindow( m_sdlWindow );
        }
         */
        
        this->pushQuitEvent_();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::minimizeWindow()
    {
        /*
        SDL_MinimizeWindow( m_sdlWindow );
         */
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setCursorPosition( const mt::vec2f & _pos )
    {
        Resolution resolution;
        if( this->getDesktopResolution( &resolution ) == false )
        {
            return;
        }

        uint32_t width = resolution.getWidth();
        uint32_t height = resolution.getHeight();

        int32_t wndPosX = static_cast<int32_t>(_pos.x * width);
        int32_t wndPosY = static_cast<int32_t>(_pos.y * height);

        // ! This function generates a mouse motion event !
        /*
        SDL_WarpMouseInWindow( m_sdlWindow, wndPosX, wndPosY );
         */
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setCursorIcon( const ConstString & _icon )
    {
        MENGINE_UNUSED( _icon );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::hasCursorIcon( const ConstString & _icon ) const
    {
        MENGINE_UNUSED( _icon );

        // Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::showKeyboard()
    {
        /*
        SDL_StartTextInput();
         */
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::hideKeyboard()
    {
        /*
        SDL_StopTextInput();
         */
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::isShowKeyboard() const
    {
        /*
        SDL_bool active = SDL_IsTextInputActive();

        return active;
         */

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::notifyVsyncChanged( bool _vsync )
    {
        MENGINE_UNUSED( _vsync );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::notifyCursorModeChanged( bool _mode )
    {
        MENGINE_UNUSED( _mode );

        // Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer )
    {
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _content );
        MENGINE_UNUSED( _buffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::onEvent( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool s_createDirectoryFullpath( const Char * _fullpath )
        {
            int status = ::mkdir( _fullpath, S_IRWXU );

            if( status != 0 )
            {
                LOGGER_WARNING( "'%s' already exists"
                    , _fullpath
                );

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isDirectoryFullpath( const Char * _fullpath )
        {
            struct stat sb;
            int err = ::stat( _fullpath, &sb );

            if( err != 0 )
            {
                return false;
            }

            if( (sb.st_mode & S_IFMT) != S_IFDIR )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isFileFullpath( const Char * _fullpath )
        {
            struct stat sb;
            int err = ::stat( _fullpath, &sb );

            if( err != 0 )
            {
                return false;
            }

            if( (sb.st_mode & S_IFMT) == S_IFDIR )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::existDirectory( const Char * _path, const Char * _directory ) const
    {
        Char pathDirectory[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( pathDirectory, _directory );

        Helper::pathRemoveFileSpecA( pathDirectory );

        size_t len = StdString::strlen( pathDirectory );

        if( len == 0 )	// current dir
        {
            return true;
        }

        Char pathFull[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_SNPRINTF( pathFull, MENGINE_MAX_PATH, "%s%s", _path, pathDirectory );

        if( pathFull[len - 1] == ':' )	// root dir
        {
            return true;	// let it be
        }

        bool exist = Detail::s_isDirectoryFullpath( pathFull );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::createDirectory( const Char * _path, const Char * _directory )
    {
        size_t directorySize = StdString::strlen( _directory );

        if( directorySize == 0 )
        {
            return true;
        }

        Char pathDirectory[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( pathDirectory, _directory );

        Helper::pathRemoveFileSpecA( pathDirectory );

        if( StdString::strlen( pathDirectory ) == 0 )
        {
            return true;
        }

        Char pathTestDirectory[MENGINE_MAX_PATH] = {'\0'};
        MENGINE_SNPRINTF( pathTestDirectory, MENGINE_MAX_PATH, "%s%s", _path, pathDirectory );

        if( Detail::s_isDirectoryFullpath( pathTestDirectory ) == true )
        {
            return true;
        }

        Helper::pathRemoveBackslashA( pathDirectory );

        VectorString paths;

        for( ;; )
        {
            paths.push_back( pathDirectory );

            if( Helper::pathRemoveFileSpecA( pathDirectory ) == false )
            {
                break;
            }

            if( StdString::strlen( _path ) == 0 )
            {
                break;
            }

            Helper::pathRemoveBackslashA( pathDirectory );

            MENGINE_SNPRINTF( pathTestDirectory, MENGINE_MAX_PATH, "%s%s", _path, pathDirectory );

            if( Detail::s_isDirectoryFullpath( pathTestDirectory ) == true )
            {
                break;
            }
        }

        for( VectorString::reverse_iterator
            it = paths.rbegin(),
            it_end = paths.rend();
            it != it_end;
            ++it )
        {
            const String & path = *it;

            const Char * path_str = path.c_str();

            Char pathCreateDirectory[MENGINE_MAX_PATH] = {'\0'};
            MENGINE_SNPRINTF( pathCreateDirectory, MENGINE_MAX_PATH, "%s%s", _path, path_str );

            if( Detail::s_createDirectoryFullpath( pathCreateDirectory ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::existFile( const Char * _filePath )
    {
        Char pathCorrect[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        bool exist = Detail::s_isFileFullpath( pathCorrect );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::removeFile( const Char * _filePath )
    {
        Char pathCorrect[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( pathCorrect, _filePath );

        int result = ::remove( pathCorrect );

        if( result != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::moveFile( const Char * _oldFilePath, const Char * _newFilePath )
    {
        Char oldPathCorrect[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( oldPathCorrect, _oldFilePath );

        Char newPathCorrect[MENGINE_MAX_PATH] = {'\0'};
        Helper::pathCorrectBackslashToA( newPathCorrect, _newFilePath );

        struct stat sb;
        if( stat( newPathCorrect, &sb ) == 0 && ((sb.st_mode) & S_IFMT) != S_IFDIR )
        {
            int result_remove = ::remove( newPathCorrect );

            if( result_remove != 0 )
            {
                const char * msg = ::strerror( errno );

                LOGGER_ASSERTION( "invalid remove new move file from '%s' to '%s' error '%s' [%u]"
                    , _oldFilePath
                    , _newFilePath
                    , msg
                    , errno
                );
            }
        }

        int result_rename = ::rename( oldPathCorrect, newPathCorrect );

        if( result_rename != 0 )
        {
            const char * msg = ::strerror( errno );

            LOGGER_ASSERTION( "invalid move file from '%s' to '%s' error '%s' [%u]"
                , _oldFilePath
                , _newFilePath
                , msg
                , errno
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        MENGINE_UNUSED( _base );
        MENGINE_UNUSED( _path );
        MENGINE_UNUSED( _mask );
        MENGINE_UNUSED( _lambda );

        LOGGER_WARNING("AndroidPlatformService::findFiles not support");

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t AndroidPlatformService::getFileTime( const Char * _filePath ) const
    {
        MENGINE_UNUSED( _filePath );

        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::messageBox( const Char * _caption, const Char * _format, ... ) const
    {
        Char str[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        int32_t size_vsnprintf = MENGINE_VSNPRINTF( str, MENGINE_LOGGER_MAX_MESSAGE - 1, _format, args );

        MENGINE_VA_LIST_END( args );

        /*
        if( size_vsnprintf < 0 )
        {
            LOGGER_ERROR( "invalid message box format message '%s'"
                , _format
            );

            SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, _caption, "invalid message box format message", nullptr );

            return;
        }

        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, _caption, str, nullptr );
         */
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::setClipboardText( const Char * _value ) const
    {
        MENGINE_UNUSED( _value );

        /*
        if( SDL_SetClipboardText( _value ) != 0 )
        {
            LOGGER_WARNING( "set clipboard text [%s] error: %s"
                , _value
                , SDL_GetError()
            );

            return false;
        }

        return true;
         */

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::getClipboardText( Char * _value, size_t _capacity ) const
    {
        /*
        char * text = SDL_GetClipboardText();

        if( text == nullptr )
        {
            return false;
        }

        MENGINE_STRNCPY( _value, text, _capacity );

        SDL_free( text );

        return true;
         */

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::createWindow_( const Resolution & _windowResolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _windowResolution );
        MENGINE_UNUSED( _fullscreen );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::destroyWindow_()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );

        ::eglTerminate( m_eglDisplay );
        m_eglDisplay = EGL_NO_DISPLAY;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::processEvents_()
    {
        m_eventsMutex->lock();
        std::swap( m_events, m_eventsAux );
        m_eventsAux.clear();
        m_eventsMutex->unlock();

        for( const PlatformUnionEvent & ev : m_events )
        {
            switch( ev.type )
            {
            case PlatformUnionEvent::PET_PAUSE:
                {
                    this->pauseEvent_( ev.data.pause );
                }break;
            case PlatformUnionEvent::PET_RESUME:
                {
                    this->resumeEvent_( ev.data.resume );
                }break;
            case PlatformUnionEvent::PET_SURFACE_CREATE:
                {
                    this->surfaceCreateEvent_( ev.data.surfaceCreate );
                }break;
            case PlatformUnionEvent::PET_SURFACE_DESTROY:
                {
                    this->surfaceDestroyEvent_( ev.data.surfaceDestroy );
                }break;
            case PlatformUnionEvent::PET_SURFACE_CHANGED:
                {
                    this->surfaceChangedEvent_( ev.data.surfaceChanged );
                }break;
            default:
                break;
            }
        }

        /*
        bool shouldQuit = false;
        
        SDL_Event sdlEvent;
        while( SDL_PollEvent( &sdlEvent ) != 0 )
        {
            LOGGER_INFO( "platform", "platform event: %s (%u)"
                , Detail::getPlatformEventMessage( sdlEvent.type )
                , sdlEvent.type
            );

            for( const SDLEventHandlerDesc & desc : m_sdlEventHandlers )
            {
                desc.handler( &sdlEvent );
            }

            m_androidInput->handleEvent( m_sdlWindow, sdlEvent );

            switch( sdlEvent.type )
            {
            case SDL_KEYDOWN:
                {
                    SDL_Keycode keyCode = sdlEvent.key.keysym.sym;
                    Uint16 keyMod = sdlEvent.key.keysym.mod;

                    switch( keyCode )
                    {
                    case SDLK_RETURN:
                        {
                            if( keyMod & KMOD_ALT )
                            {
                                bool fullscreen = APPLICATION_SERVICE()
                                    ->getFullscreenMode();

                                APPLICATION_SERVICE()
                                    ->setFullscreenMode( !fullscreen );
                            }
                        }break;
                    default:
                        break;
                    }
                }break;
            case SDL_WINDOWEVENT:
                {
                    Uint32 windowID = SDL_GetWindowID( m_sdlWindow );

                    if( sdlEvent.window.windowID != windowID )
                    {
                        continue;
                    }

                    SDL_WindowEventID windowEventId = (SDL_WindowEventID)sdlEvent.window.event;

                    LOGGER_INFO( "platform", "window event: %s (%u)"
                        , Detail::getWindowEventMessage( windowEventId )
                        , windowEventId
                    );

                    switch( windowEventId )
                    {
                    case SDL_WINDOWEVENT_SHOWN:
                        {
                            //TODO
                        }break;
                    case SDL_WINDOWEVENT_HIDDEN:
                        {
                            //TODO
                        }break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        {
                            //TODO
                        }break;
                    case SDL_WINDOWEVENT_MOVED:
                        {
                            //TODO
                        }break;
                    case SDL_WINDOWEVENT_RESIZED:
                        {
                            Sint32 width = sdlEvent.window.data1;
                            Sint32 height = sdlEvent.window.data2;

                            Resolution windowResolution( width, height );

                            APPLICATION_SERVICE()
                                ->setWindowResolution( windowResolution );
                        }break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        {
                            Sint32 width = sdlEvent.window.data1;
                            Sint32 height = sdlEvent.window.data2;

                            MENGINE_UNUSED( width );
                            MENGINE_UNUSED( height );
                            //TODO
                        }break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                    case SDL_WINDOWEVENT_MAXIMIZED:
                    case SDL_WINDOWEVENT_RESTORED:
                    case SDL_WINDOWEVENT_ENTER:
                        {
                            this->setActive_( true );
                        }break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                    case SDL_WINDOWEVENT_MINIMIZED:
                    case SDL_WINDOWEVENT_LEAVE:
                        {
                            this->setActive_( false );
                        }break;
                    case SDL_WINDOWEVENT_CLOSE:
                        {
                            this->pushQuitEvent_();
                        }break;
                    case SDL_WINDOWEVENT_TAKE_FOCUS:
                        {
                            //TODO
                        }break;
                    default:
                        break;
                    }
                }break;
            case SDL_QUIT:
                {
                    shouldQuit = true;
                }break;
#if defined(MENGINE_PLATFORM_IOS) || defined(MENGINE_PLATFORM_ANDROID)
            case SDL_APP_TERMINATING:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_TERMINATE );
                }break;
            case SDL_APP_LOWMEMORY:
                {                    
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_RECEIVE_MEMORY_WARNING );
                }break;
            case SDL_APP_WILLENTERBACKGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_RESIGN_ACTIVE );
                }break;
            case SDL_APP_DIDENTERBACKGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_ENTER_BACKGROUD );
                }break;
            case SDL_APP_WILLENTERFOREGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_ENTER_FOREGROUND );
                }break;
            case SDL_APP_DIDENTERFOREGROUND:
                {
                    NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_BECOME_ACTIVE );
                }break;
#endif
            default:
                break;
            }
        }

        if( shouldQuit == true )
        {
            return true;
        }
        */

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::pushQuitEvent_()
    {
        /*
        SDL_Event e;
        e.type = SDL_QUIT;
        e.quit.timestamp = SDL_GetTicks();

        if( SDL_PushEvent( &e ) == -1 )
        {
            LOGGER_WARNING( "invalid push event [SDL_QUIT] error: %s"
                , SDL_GetError()
            );
        }
         */
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setActive_( float _x, float _y, bool _active )
    {
        if( m_active == _active )
        {
            return;
        }

        m_active = _active;

        bool nopause = APPLICATION_SERVICE()
            ->getNopause();

        if( m_active == false )
        {
            if( nopause == false )
            {
                Helper::pushMouseLeaveEvent( TC_TOUCH0, _x, _y, 0.f );
            }
        }
        else
        {
            Helper::pushMouseEnterEvent( TC_TOUCH0, _x, _y, 0.f );
        }

        if( nopause == false )
        {
            APPLICATION_SERVICE()
                ->setFocus( m_active );

            INPUT_SERVICE()
                ->onFocus( m_active );

            APPLICATION_SERVICE()
                ->turnSound( m_active );
        }
        else
        {
            APPLICATION_SERVICE()
                ->setFocus( true );

            INPUT_SERVICE()
                ->onFocus( true );

            APPLICATION_SERVICE()
                ->turnSound( true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::initializeFileService()
    {
        FileGroupInterfacePtr defaultFileGroup = nullptr;

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), &defaultFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory" );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none(), defaultFileGroup, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        FileGroupInterfacePtr globalFileGroup = nullptr;

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), &globalFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory" );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ), globalFileGroup, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::finalizeFileService()
    {
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        FILE_SERVICE()
            ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );
#endif

        FILE_SERVICE()
            ->unmountFileGroup( ConstString::none() );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeSurfaceCreated( ANativeWindow * _nativeWindow )
    {
        m_nativeMutex->lock();
        m_nativeWindow = _nativeWindow;
        m_nativeMutex->unlock();

        PlatformUnionEvent ev;
        ev.type = PlatformUnionEvent::PET_SURFACE_CREATE;
        ev.data.surfaceCreate.nativeWindow = _nativeWindow;

        LOGGER_INFO( "platform", "push surface create event" );

        this->pushEvent( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeSurfaceDestroyed( ANativeWindow * _nativeWindow )
    {
        m_nativeMutex->lock();
        m_nativeWindow = _nativeWindow;
        m_nativeMutex->unlock();

        PlatformUnionEvent ev;
        ev.type = PlatformUnionEvent::PET_SURFACE_DESTROY;
        ev.data.surfaceDestroy.nativeWindow = _nativeWindow;

        LOGGER_INFO( "platform", "push surface destroy event" );

        this->pushEvent( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeSurfaceChanged( ANativeWindow * _nativeWindow, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate )
    {
        m_nativeMutex->lock();
        m_nativeWindow = _nativeWindow;
        m_nativeMutex->unlock();

        PlatformUnionEvent ev;
        ev.type = PlatformUnionEvent::PET_SURFACE_CHANGED;
        ev.data.surfaceChanged.nativeWindow = _nativeWindow;
        ev.data.surfaceChanged.surfaceWidth = surfaceWidth;
        ev.data.surfaceChanged.surfaceHeight = surfaceHeight;
        ev.data.surfaceChanged.deviceWidth = deviceWidth;
        ev.data.surfaceChanged.deviceHeight = deviceHeight;

        LOGGER_INFO( "platform", "push surface changed event surface: %d;%d device: %d;%d rate: %f"
            , surfaceWidth
            , surfaceHeight
            , deviceWidth
            , deviceHeight
            , rate
        );

        this->pushEvent( ev );
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode AndroidPlatformService::acquireFingerIndex_( jint _pointerId )
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            jint fingerId = m_fingers[index];

            if( fingerId == -1 )
            {
                m_fingers[index] = _pointerId;

                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode AndroidPlatformService::releaseFingerIndex_( jint _pointerId )
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            jint pointerId = m_fingers[index];

            if( pointerId == _pointerId )
            {
                m_fingers[index] = -1;

                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode AndroidPlatformService::getFingerIndex_( jint _pointerId ) const
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            jint pointerId = m_fingers[index];

            if( pointerId == _pointerId )
            {
                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeTouchEvent( jint _action, jint _pointerId, jfloat _x, jfloat _y, jfloat _pressure )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeMutex );

        switch( _action )
        {
        case 0: //ACTION_DOWN
        case 5: //ACTION_POINTER_DOWN
            {
                ETouchCode fingerIndex = this->acquireFingerIndex_( _pointerId );

                m_currentFingersX[fingerIndex] = _x;
                m_currentFingersY[fingerIndex] = _y;
                m_currentFingersPressure[fingerIndex] = _pressure;

                LOGGER_INFO( "platform", "touch [%d] down position: %.4f;%.4f pressure: %.4f"
                    , fingerIndex
                    , _x
                    , _y
                    , _pressure
                );

                Helper::pushMouseButtonEvent( fingerIndex, _x, _y, MC_LBUTTON, _pressure, true );
            }break;
        case 1: //ACTION_UP
        case 6: //ACTION_POINTER_UP
            {
                ETouchCode fingerIndex = this->releaseFingerIndex_( _pointerId );

                m_currentFingersX[fingerIndex] = _x;
                m_currentFingersY[fingerIndex] = _y;
                m_currentFingersPressure[fingerIndex] = _pressure;

                LOGGER_INFO( "platform", "touch [%d] up position: %.4f;%.4f pressure: %.4f"
                    , fingerIndex
                    , _x
                    , _y
                    , _pressure
                );

                Helper::pushMouseButtonEvent( fingerIndex, _x, _y, MC_LBUTTON, _pressure, false );
            }break;
        case 2: //ACTION_MOVE
            {
                ETouchCode fingerIndex = this->getFingerIndex_( _pointerId );

                jfloat previousX = m_currentFingersX[fingerIndex];
                jfloat previousY = m_currentFingersY[fingerIndex];
                jfloat previousPressure = m_currentFingersPressure[fingerIndex];

                jfloat dx = _x - previousX;
                jfloat dy = _y - previousY;
                jfloat dpressure = _pressure - previousPressure;

                m_currentFingersX[fingerIndex] = _x;
                m_currentFingersY[fingerIndex] = _y;
                m_currentFingersPressure[fingerIndex] = _pressure;

                LOGGER_INFO( "platform", "touch [%d] move position: %.4f;%.4f pressure: %.4f delta: %.4f;%.4f dpressure: %.4f"
                    , fingerIndex
                    , _x
                    , _y
                    , _pressure
                    , dx
                    , dy
                    , dpressure
                );

                Helper::pushMouseMoveEvent( fingerIndex, _x, _y, dx, dy, _pressure, dpressure );
            }break;
        default:
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeAccelerationEvent( jfloat _dx, jfloat _dy, jfloat _dz )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeMutex );

        Helper::pushAccelerometerEvent( _dx, _dy, _dz );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeKeyEvent( jboolean _isDown, jint _keyCode, jint _repeatCount )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeMutex );

        static EKeyCode Android_Keycodes[] = {
            KC_UNASSIGNED,          /* AKEYCODE_UNKNOWN */
            KC_UNASSIGNED,         /* AKEYCODE_SOFT_LEFT */
            KC_UNASSIGNED,        /* AKEYCODE_SOFT_RIGHT */
            KC_UNASSIGNED,          /* AKEYCODE_HOME */
            KC_BROWSER_BACK,          /* AKEYCODE_BACK */
            KC_UNASSIGNED,             /* AKEYCODE_CALL */
            KC_UNASSIGNED,          /* AKEYCODE_ENDCALL */
            KC_0,                /* AKEYCODE_0 */
            KC_1,                /* AKEYCODE_1 */
            KC_2,                /* AKEYCODE_2 */
            KC_3,                /* AKEYCODE_3 */
            KC_4,                /* AKEYCODE_4 */
            KC_5,                /* AKEYCODE_5 */
            KC_6,                /* AKEYCODE_6 */
            KC_7,                /* AKEYCODE_7 */
            KC_8,                /* AKEYCODE_8 */
            KC_9,                /* AKEYCODE_9 */
            KC_UNASSIGNED,          /* AKEYCODE_STAR */
            KC_UNASSIGNED,          /* AKEYCODE_POUND */
            KC_UP,               /* AKEYCODE_DPAD_UP */
            KC_DOWN,             /* AKEYCODE_DPAD_DOWN */
            KC_LEFT,             /* AKEYCODE_DPAD_LEFT */
            KC_RIGHT,            /* AKEYCODE_DPAD_RIGHT */
            KC_UNASSIGNED,           /* AKEYCODE_DPAD_CENTER */
            KC_UNASSIGNED,         /* AKEYCODE_VOLUME_UP */
            KC_UNASSIGNED,       /* AKEYCODE_VOLUME_DOWN */
            KC_UNASSIGNED,            /* AKEYCODE_POWER */
            KC_UNASSIGNED,          /* AKEYCODE_CAMERA */
            KC_UNASSIGNED,            /* AKEYCODE_CLEAR */
            KC_A,                /* AKEYCODE_A */
            KC_B,                /* AKEYCODE_B */
            KC_C,                /* AKEYCODE_C */
            KC_D,                /* AKEYCODE_D */
            KC_E,                /* AKEYCODE_E */
            KC_F,                /* AKEYCODE_F */
            KC_G,                /* AKEYCODE_G */
            KC_H,                /* AKEYCODE_H */
            KC_I,                /* AKEYCODE_I */
            KC_J,                /* AKEYCODE_J */
            KC_K,                /* AKEYCODE_K */
            KC_L,                /* AKEYCODE_L */
            KC_M,                /* AKEYCODE_M */
            KC_N,                /* AKEYCODE_N */
            KC_O,                /* AKEYCODE_O */
            KC_P,                /* AKEYCODE_P */
            KC_Q,                /* AKEYCODE_Q */
            KC_R,                /* AKEYCODE_R */
            KC_S,                /* AKEYCODE_S */
            KC_T,                /* AKEYCODE_T */
            KC_U,                /* AKEYCODE_U */
            KC_V,                /* AKEYCODE_V */
            KC_W,                /* AKEYCODE_W */
            KC_X,                /* AKEYCODE_X */
            KC_Y,                /* AKEYCODE_Y */
            KC_Z,                /* AKEYCODE_Z */
            KC_UNASSIGNED,            /* AKEYCODE_COMMA */
            KC_UNASSIGNED,           /* AKEYCODE_PERIOD */
            KC_LMENU,             /* AKEYCODE_ALT_LEFT */
            KC_RMENU,             /* AKEYCODE_ALT_RIGHT */
            KC_LSHIFT,           /* AKEYCODE_SHIFT_LEFT */
            KC_RSHIFT,           /* AKEYCODE_SHIFT_RIGHT */
            KC_TAB,              /* AKEYCODE_TAB */
            KC_SPACE,            /* AKEYCODE_SPACE */
            KC_UNASSIGNED,          /* AKEYCODE_SYM */
            KC_UNASSIGNED,              /* AKEYCODE_EXPLORER */
            KC_UNASSIGNED,             /* AKEYCODE_ENVELOPE */
            KC_RETURN,           /* AKEYCODE_ENTER */
            KC_BACK,        /* AKEYCODE_DEL */
            KC_UNASSIGNED,            /* AKEYCODE_GRAVE */
            KC_OEM_MINUS,            /* AKEYCODE_MINUS */
            KC_OEM_PLUS,           /* AKEYCODE_EQUALS */
            KC_OEM_4,      /* AKEYCODE_LEFT_BRACKET */
            KC_OEM_6,     /* AKEYCODE_RIGHT_BRACKET */
            KC_UNASSIGNED,        /* AKEYCODE_BACKSLASH */
            KC_UNASSIGNED,        /* AKEYCODE_SEMICOLON */
            KC_UNASSIGNED,       /* AKEYCODE_APOSTROPHE */
            KC_UNASSIGNED,            /* AKEYCODE_SLASH */
            KC_UNASSIGNED,          /* AKEYCODE_AT */
            KC_UNASSIGNED,          /* AKEYCODE_NUM */
            KC_UNASSIGNED,          /* AKEYCODE_HEADSETHOOK */
            KC_UNASSIGNED,          /* AKEYCODE_FOCUS */
            KC_UNASSIGNED,          /* AKEYCODE_PLUS */
            KC_MENU,             /* AKEYCODE_MENU */
            KC_UNASSIGNED,          /* AKEYCODE_NOTIFICATION */
            KC_UNASSIGNED,        /* AKEYCODE_SEARCH */
            KC_UNASSIGNED,        /* AKEYCODE_MEDIA_PLAY_PAUSE */
            KC_UNASSIGNED,        /* AKEYCODE_MEDIA_STOP */
            KC_UNASSIGNED,        /* AKEYCODE_MEDIA_NEXT */
            KC_UNASSIGNED,        /* AKEYCODE_MEDIA_PREVIOUS */
            KC_UNASSIGNED,      /* AKEYCODE_MEDIA_REWIND */
            KC_UNASSIGNED, /* AKEYCODE_MEDIA_FAST_FORWARD */
            KC_UNASSIGNED,             /* AKEYCODE_MUTE */
            KC_PRIOR,           /* AKEYCODE_PAGE_UP */
            KC_NEXT,         /* AKEYCODE_PAGE_DOWN */
            KC_UNASSIGNED,          /* AKEYCODE_PICTSYMBOLS */
            KC_UNASSIGNED,          /* AKEYCODE_SWITCH_CHARSET */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_A */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_B */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_C */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_X */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_Y */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_Z */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_L1 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_R1 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_L2 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_R2 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_THUMBL */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_THUMBR */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_START */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_SELECT */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_MODE */
            KC_ESCAPE,           /* AKEYCODE_ESCAPE */
            KC_DELETE,           /* AKEYCODE_FORWARD_DEL */
            KC_LCONTROL,            /* AKEYCODE_CTRL_LEFT */
            KC_RCONTROL,            /* AKEYCODE_CTRL_RIGHT */
            KC_CAPITAL,         /* AKEYCODE_CAPS_LOCK */
            KC_UNASSIGNED,       /* AKEYCODE_SCROLL_LOCK */
            KC_UNASSIGNED,             /* AKEYCODE_META_LEFT */
            KC_UNASSIGNED,             /* AKEYCODE_META_RIGHT */
            KC_UNASSIGNED,          /* AKEYCODE_FUNCTION */
            KC_UNASSIGNED,      /* AKEYCODE_SYSRQ */
            KC_PAUSE,            /* AKEYCODE_BREAK */
            KC_HOME,             /* AKEYCODE_MOVE_HOME */
            KC_END,              /* AKEYCODE_MOVE_END */
            KC_INSERT,           /* AKEYCODE_INSERT */
            KC_UNASSIGNED,       /* AKEYCODE_FORWARD */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_PLAY */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_PAUSE */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_CLOSE */
            KC_UNASSIGNED,            /* AKEYCODE_MEDIA_EJECT */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_RECORD */
            KC_F1,               /* AKEYCODE_F1 */
            KC_F2,               /* AKEYCODE_F2 */
            KC_F3,               /* AKEYCODE_F3 */
            KC_F4,               /* AKEYCODE_F4 */
            KC_F5,               /* AKEYCODE_F5 */
            KC_F6,               /* AKEYCODE_F6 */
            KC_F7,               /* AKEYCODE_F7 */
            KC_F8,               /* AKEYCODE_F8 */
            KC_F9,               /* AKEYCODE_F9 */
            KC_F10,              /* AKEYCODE_F10 */
            KC_F11,              /* AKEYCODE_F11 */
            KC_F12,              /* AKEYCODE_F12 */
            KC_UNASSIGNED,          /* AKEYCODE_NUM_LOCK */
            KC_NUMPAD0,             /* AKEYCODE_NUMPAD_0 */
            KC_NUMPAD1,             /* AKEYCODE_NUMPAD_1 */
            KC_NUMPAD2,             /* AKEYCODE_NUMPAD_2 */
            KC_NUMPAD3,             /* AKEYCODE_NUMPAD_3 */
            KC_NUMPAD4,             /* AKEYCODE_NUMPAD_4 */
            KC_NUMPAD5,             /* AKEYCODE_NUMPAD_5 */
            KC_NUMPAD6,             /* AKEYCODE_NUMPAD_6 */
            KC_NUMPAD7,             /* AKEYCODE_NUMPAD_7 */
            KC_NUMPAD8,             /* AKEYCODE_NUMPAD_8 */
            KC_NUMPAD9,             /* AKEYCODE_NUMPAD_9 */
            KC_DIVIDE,        /* AKEYCODE_NUMPAD_DIVIDE */
            KC_UNASSIGNED,      /* AKEYCODE_NUMPAD_MULTIPLY */
            KC_SUBTRACT,         /* AKEYCODE_NUMPAD_SUBTRACT */
            KC_ADD,          /* AKEYCODE_NUMPAD_ADD */
            KC_UNASSIGNED,        /* AKEYCODE_NUMPAD_DOT */
            KC_UNASSIGNED,         /* AKEYCODE_NUMPAD_COMMA */
            KC_UNASSIGNED,         /* AKEYCODE_NUMPAD_ENTER */
            KC_UNASSIGNED,        /* AKEYCODE_NUMPAD_EQUALS */
            KC_UNASSIGNED,     /* AKEYCODE_NUMPAD_LEFT_PAREN */
            KC_UNASSIGNED,    /* AKEYCODE_NUMPAD_RIGHT_PAREN */
            KC_UNASSIGNED,          /* AKEYCODE_VOLUME_MUTE */
            KC_UNASSIGNED,          /* AKEYCODE_INFO */
            KC_UNASSIGNED,          /* AKEYCODE_CHANNEL_UP */
            KC_UNASSIGNED,          /* AKEYCODE_CHANNEL_DOWN */
            KC_UNASSIGNED,          /* AKEYCODE_ZOOM_IN */
            KC_UNASSIGNED,          /* AKEYCODE_ZOOM_OUT */
            KC_UNASSIGNED,          /* AKEYCODE_TV */
            KC_UNASSIGNED,          /* AKEYCODE_WINDOW */
            KC_UNASSIGNED,          /* AKEYCODE_GUIDE */
            KC_UNASSIGNED,          /* AKEYCODE_DVR */
            KC_UNASSIGNED,     /* AKEYCODE_BOOKMARK */
            KC_UNASSIGNED,          /* AKEYCODE_CAPTIONS */
            KC_UNASSIGNED,          /* AKEYCODE_SETTINGS */
            KC_UNASSIGNED,          /* AKEYCODE_TV_POWER */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT */
            KC_UNASSIGNED,          /* AKEYCODE_STB_POWER */
            KC_UNASSIGNED,          /* AKEYCODE_STB_INPUT */
            KC_UNASSIGNED,          /* AKEYCODE_AVR_POWER */
            KC_UNASSIGNED,          /* AKEYCODE_AVR_INPUT */
            KC_UNASSIGNED,          /* AKEYCODE_PROG_RED */
            KC_UNASSIGNED,          /* AKEYCODE_PROG_GREEN */
            KC_UNASSIGNED,          /* AKEYCODE_PROG_YELLOW */
            KC_UNASSIGNED,          /* AKEYCODE_PROG_BLUE */
            KC_UNASSIGNED,          /* AKEYCODE_APP_SWITCH */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_1 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_2 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_3 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_4 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_5 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_6 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_7 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_8 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_9 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_10 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_11 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_12 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_13 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_14 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_15 */
            KC_UNASSIGNED,          /* AKEYCODE_BUTTON_16 */
            KC_UNASSIGNED,          /* AKEYCODE_LANGUAGE_SWITCH */
            KC_UNASSIGNED,          /* AKEYCODE_MANNER_MODE */
            KC_UNASSIGNED,          /* AKEYCODE_3D_MODE */
            KC_UNASSIGNED,          /* AKEYCODE_CONTACTS */
            KC_UNASSIGNED,          /* AKEYCODE_CALENDAR */
            KC_UNASSIGNED,          /* AKEYCODE_MUSIC */
            KC_UNASSIGNED,       /* AKEYCODE_CALCULATOR */
            KC_UNASSIGNED,            /* AKEYCODE_ZENKAKU_HANKAKU */
            KC_UNASSIGNED,          /* AKEYCODE_EISU */
            KC_UNASSIGNED,   /* AKEYCODE_MUHENKAN */
            KC_UNASSIGNED,   /* AKEYCODE_HENKAN */
            KC_UNASSIGNED,            /* AKEYCODE_KATAKANA_HIRAGANA */
            KC_UNASSIGNED,   /* AKEYCODE_YEN */
            KC_UNASSIGNED,          /* AKEYCODE_RO */
            KC_UNASSIGNED,          /* AKEYCODE_KANA */
            KC_UNASSIGNED,          /* AKEYCODE_ASSIST */
            KC_UNASSIGNED,   /* AKEYCODE_BRIGHTNESS_DOWN */
            KC_UNASSIGNED,     /* AKEYCODE_BRIGHTNESS_UP */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_AUDIO_TRACK */
            KC_UNASSIGNED,            /* AKEYCODE_SLEEP */
            KC_UNASSIGNED,          /* AKEYCODE_WAKEUP */
            KC_UNASSIGNED,          /* AKEYCODE_PAIRING */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_TOP_MENU */
            KC_UNASSIGNED,          /* AKEYCODE_11 */
            KC_UNASSIGNED,          /* AKEYCODE_12 */
            KC_UNASSIGNED,          /* AKEYCODE_LAST_CHANNEL */
            KC_UNASSIGNED,          /* AKEYCODE_TV_DATA_SERVICE */
            KC_UNASSIGNED,          /* AKEYCODE_VOICE_ASSIST */
            KC_UNASSIGNED,          /* AKEYCODE_TV_RADIO_SERVICE */
            KC_UNASSIGNED,          /* AKEYCODE_TV_TELETEXT */
            KC_UNASSIGNED,          /* AKEYCODE_TV_NUMBER_ENTRY */
            KC_UNASSIGNED,          /* AKEYCODE_TV_TERRESTRIAL_ANALOG */
            KC_UNASSIGNED,          /* AKEYCODE_TV_TERRESTRIAL_DIGITAL */
            KC_UNASSIGNED,          /* AKEYCODE_TV_SATELLITE */
            KC_UNASSIGNED,          /* AKEYCODE_TV_SATELLITE_BS */
            KC_UNASSIGNED,          /* AKEYCODE_TV_SATELLITE_CS */
            KC_UNASSIGNED,          /* AKEYCODE_TV_SATELLITE_SERVICE */
            KC_UNASSIGNED,          /* AKEYCODE_TV_NETWORK */
            KC_UNASSIGNED,          /* AKEYCODE_TV_ANTENNA_CABLE */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_HDMI_1 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_HDMI_2 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_HDMI_3 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_HDMI_4 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_COMPOSITE_1 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_COMPOSITE_2 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_COMPONENT_1 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_COMPONENT_2 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_INPUT_VGA_1 */
            KC_UNASSIGNED,          /* AKEYCODE_TV_AUDIO_DESCRIPTION */
            KC_UNASSIGNED,          /* AKEYCODE_TV_AUDIO_DESCRIPTION_MIX_UP */
            KC_UNASSIGNED,          /* AKEYCODE_TV_AUDIO_DESCRIPTION_MIX_DOWN */
            KC_UNASSIGNED,          /* AKEYCODE_TV_ZOOM_MODE */
            KC_UNASSIGNED,          /* AKEYCODE_TV_CONTENTS_MENU */
            KC_UNASSIGNED,          /* AKEYCODE_TV_MEDIA_CONTEXT_MENU */
            KC_UNASSIGNED,          /* AKEYCODE_TV_TIMER_PROGRAMMING */
            KC_UNASSIGNED,             /* AKEYCODE_HELP */
            KC_UNASSIGNED,          /* AKEYCODE_NAVIGATE_PREVIOUS */
            KC_UNASSIGNED,          /* AKEYCODE_NAVIGATE_NEXT */
            KC_UNASSIGNED,          /* AKEYCODE_NAVIGATE_IN */
            KC_UNASSIGNED,          /* AKEYCODE_NAVIGATE_OUT */
            KC_UNASSIGNED,          /* AKEYCODE_STEM_PRIMARY */
            KC_UNASSIGNED,          /* AKEYCODE_STEM_1 */
            KC_UNASSIGNED,          /* AKEYCODE_STEM_2 */
            KC_UNASSIGNED,          /* AKEYCODE_STEM_3 */
            KC_UNASSIGNED,          /* AKEYCODE_DPAD_UP_LEFT */
            KC_UNASSIGNED,          /* AKEYCODE_DPAD_DOWN_LEFT */
            KC_UNASSIGNED,          /* AKEYCODE_DPAD_UP_RIGHT */
            KC_UNASSIGNED,          /* AKEYCODE_DPAD_DOWN_RIGHT */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_SKIP_FORWARD */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_SKIP_BACKWARD */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_STEP_FORWARD */
            KC_UNASSIGNED,          /* AKEYCODE_MEDIA_STEP_BACKWARD */
            KC_UNASSIGNED,          /* AKEYCODE_SOFT_SLEEP */
            KC_UNASSIGNED,              /* AKEYCODE_CUT */
            KC_UNASSIGNED,             /* AKEYCODE_COPY */
            KC_UNASSIGNED,            /* AKEYCODE_PASTE */
        };

        if( _keyCode >= sizeof(Android_Keycodes) )
        {
            return;
        }

        EKeyCode keyCode = Android_Keycodes[_keyCode];

        if( keyCode == KC_UNASSIGNED )
        {
            return;
        }

        jfloat x = m_currentFingersX[0];
        jfloat y = m_currentFingersY[0];
        jfloat pressure = m_currentFingersPressure[0];

        LOGGER_INFO( "platform", "key event: %d down: %d repeat: %d"
            , keyCode
            , _isDown
            , _repeatCount
        );

        Helper::pushKeyEvent( x, y, pressure, keyCode, _isDown, false );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeTextEvent( jint _unicode )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeMutex );

        jfloat x = m_currentFingersX[0];
        jfloat y = m_currentFingersY[0];
        jfloat pressure = m_currentFingersPressure[0];

        WChar text[2] = {(WChar)_unicode, L'\0'};

        LOGGER_INFO( "platform", "text event: %ls"
            , text
        );

        Helper::pushTextEvent( x, y, pressure, text );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativePauseEvent()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeMutex );

        float x = m_currentFingersX[0];
        float y = m_currentFingersY[0];

        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_PAUSE;

        event.data.pause.x = x;
        event.data.pause.y = y;

        LOGGER_INFO( "platform", "pause event: %.4f;%.4f"
            , x
            , y
        );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::androidNativeResumeEvent()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_nativeMutex );

        float x = m_currentFingersX[0];
        float y = m_currentFingersY[0];

        PlatformUnionEvent event;
        event.type = PlatformUnionEvent::PET_RESUME;

        event.data.resume.x = x;
        event.data.resume.y = y;

        LOGGER_INFO( "platform", "resume event: %.4f;%.4f"
            , x
            , y
        );

        this->pushEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::pauseEvent_( const PlatformUnionEvent::PlatformPauseEvent & _event )
    {
        float x = _event.x;
        float y = _event.y;

        this->setActive_( x, y, false );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::resumeEvent_( const PlatformUnionEvent::PlatformResumeEvent & _event )
    {
        float x = _event.x;
        float y = _event.y;

        this->setActive_( x, y, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::surfaceCreateEvent_( const PlatformUnionEvent::PlatformSurfaceCreateEvent & _event )
    {
        MENGINE_ASSERTION_FATAL( m_eglDisplay != nullptr, "display not created" );
        MENGINE_ASSERTION_FATAL( m_eglSurface == nullptr, "already created egl surface" );
        MENGINE_ASSERTION_FATAL( m_eglContext == nullptr, "already created egl context" );

        int32_t format_got = ANativeWindow_getFormat( m_nativeWindow );

        const EGLint configAttribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
        };

        EGLConfig config;
        EGLint numConfigs;
        if( ::eglChooseConfig( m_eglDisplay, configAttribs, &config, 1, &numConfigs ) == EGL_FALSE )
        {
            return;
        }

        EGLint format_wanted;
        ::eglGetConfigAttrib( m_eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format_wanted );

        ANativeWindow_setBuffersGeometry( m_nativeWindow, 0, 0, format_wanted );

        EGLSurface eglSurface = ::eglCreateWindowSurface( m_eglDisplay, config, m_nativeWindow, nullptr );

        if( eglSurface == EGL_NO_SURFACE )
        {
            return;
        }

        m_eglSurface = eglSurface;

        EGLint width;
        EGLint height;
        eglQuerySurface( m_eglDisplay, m_eglSurface, EGL_WIDTH, &width );
        eglQuerySurface( m_eglDisplay, m_eglSurface, EGL_HEIGHT, &height );

        ::eglBindAPI( EGL_OPENGL_ES_API );

        const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        EGLContext eglShareContext = EGL_NO_CONTEXT;

        EGLContext context = ::eglCreateContext( m_eglDisplay, config, eglShareContext, contextAttribs );

        if( context == EGL_NO_CONTEXT )
        {
            return;
        }

        m_eglContext = context;

        ::eglBindAPI( EGL_OPENGL_ES_API );

        if( ::eglMakeCurrent( m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext ) == EGL_FALSE )
        {
            return;
        }

        if( RENDER_SERVICE()
            ->onDeviceLostRestore() == false )
        {
            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::surfaceDestroyEvent_( const PlatformUnionEvent::PlatformSurfaceDestroyEvent & _event )
    {
        if( m_eglDisplay == EGL_NO_DISPLAY )
        {
            return;
        }

        RENDER_SERVICE()
            ->onDeviceLostPrepare();

        ::eglBindAPI( EGL_OPENGL_ES_API );

        ::eglMakeCurrent( m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );

        if( m_eglContext != EGL_NO_CONTEXT )
        {
            ::eglDestroyContext( m_eglDisplay, m_eglContext );
            m_eglContext = EGL_NO_CONTEXT;
        }

        if( m_eglSurface != EGL_NO_SURFACE )
        {
            ::eglDestroySurface( m_eglDisplay, m_eglSurface );
            m_eglSurface = EGL_NO_SURFACE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::surfaceChangedEvent_( const PlatformUnionEvent::PlatformSurfaceChangedEvent & _event )
    {
        // ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::pushEvent( const PlatformUnionEvent & _event )
    {
        m_eventsMutex->lock();
        m_eventsAux.emplace_back( _event );
        m_eventsMutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
}
