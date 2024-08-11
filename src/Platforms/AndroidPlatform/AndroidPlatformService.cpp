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
        MENGINE_UNUSED( cls );

        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( env, surface );

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->setAndroidNativeWindow( nativeWindow );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1surfaceDestroyed )(JNIEnv * env, jclass cls, jobject surface)
    {
        MENGINE_UNUSED( cls );

        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( env, surface );

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        platformExtension->destroyAndroidNativeWindow( nativeWindow );
    }
    ///////////////////////////////////////////////////////////////////////
    JNIEXPORT void JNICALL MENGINE_SURFACEVIEW_JAVA_INTERFACE( AndroidPlatform_1surfaceChanged )(JNIEnv * env, jclass cls, jobject surface, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate)
    {
        MENGINE_UNUSED( cls );

        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( env, surface );

        Mengine::AndroidPlatformServiceExtensionInterface * platformExtension = PLATFORM_SERVICE()
                ->getUnknown();

        platformExtension->changeAndroidNativeWindow( nativeWindow, surfaceWidth, surfaceHeight, deviceWidth, deviceHeight, rate );
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
        /*
        , m_sdlWindow( nullptr )
        , m_sdlAccelerometer( nullptr )
        , m_sdlInput( nullptr )
         */
        , m_prevTime( 0 )
        , m_pauseUpdatingTime( -1.f )
        , m_active( false )
        , m_sleepMode( true )
        , m_desktop( false )
        , m_touchpad( false )
    {
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
    float AndroidPlatformService::getJoystickAxis( uint32_t _index ) const
    {
        MENGINE_UNUSED( _index );

        /*
        if( m_sdlAccelerometer == nullptr )
        {
            return 0.f;
        }

        float axis = SDL_JoystickGetAxis( m_sdlAccelerometer, _index );

        const float inv_maxint32f = 1.f / 32767.f;

        axis *= inv_maxint32f;

        return axis;
         */

        return 0.f;
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

        /*
        int displayIndex = -1;
        
        if( m_sdlWindow == nullptr )
        {
            displayIndex = 0;
        }
        else
        {
            displayIndex = SDL_GetWindowDisplayIndex( m_sdlWindow );

            if( displayIndex == -1 )
            {
                LOGGER_ERROR( "invalid get window display: %s"
                    , SDL_GetError()
                );

                return false;
            }
        }

        SDL_Rect rect;
        if( SDL_GetDisplayUsableBounds( displayIndex, &rect ) != 0 )
        {
            LOGGER_ERROR( "invalid get max client resolution: %s"
                , SDL_GetError()
            );

            return false;
        }

        *_resolution = Resolution( (uint32_t)rect.w, (uint32_t)rect.h );

        return true;
         */

        return false;
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

        /*
        AndroidInputPtr androidInput = Helper::makeFactorableUnique<AndroidInput>( MENGINE_DOCUMENT_FACTORABLE );

        if( androidInput->initialize() == false )
        {
            return false;
        }

        m_androidInput = androidInput;
         */

        /*
#if defined(MENGINE_PLATFORM_ANDROID)
        int AndroidSDKVersion = SDL_GetAndroidSDKVersion();
        SDL_bool AndroidTV = SDL_IsAndroidTV();

        LOGGER_INFO( "platform", "Android SDK version: %d", AndroidSDKVersion );
        LOGGER_INFO( "platform", "Android TV: %d", AndroidTV );
#endif
         */

        uint32_t deviceSeed = Helper::generateRandomDeviceSeed();

        LOGGER_INFO_PROTECTED( "plarform", "Device Seed: %u"
            , deviceSeed
        );

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

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );

        m_factoryDynamicLibraries = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::runPlatform()
    {
        this->setActive_( true );

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
            if( this->updatePlatform() == false )
            {
                break;
            }

            Timestamp currentTime = Helper::getSystemTimestamp();

            float frameTime = (float)(currentTime - m_prevTime);

            m_prevTime = currentTime;

            if( this->tickPlatform( frameTime, true, true, true ) == false )
            {
                break;
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

        this->setupWindow_();

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
        /*
        SDL_GLContext glContext = SDL_GL_CreateContext( m_sdlWindow );

        if( glContext == nullptr )
        {
            LOGGER_ERROR( "invalid create GL context error: %s"
                , SDL_GetError()
            );

            SDL_DestroyWindow( m_sdlWindow );
            m_sdlWindow = nullptr;

            return false;
        }

        int attribute_GL_CONTEXT_PROFILE_MASK = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &attribute_GL_CONTEXT_PROFILE_MASK ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_CONTEXT_PROFILE_MASK error: %s"
                , SDL_GetError()
            );
        }

        int attribute_GL_CONTEXT_MAJOR_VERSION = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, &attribute_GL_CONTEXT_MAJOR_VERSION ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_CONTEXT_MAJOR_VERSION error: %s"
                , SDL_GetError()
            );
        }

        int attribute_GL_CONTEXT_MINOR_VERSION = 0;
        if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, &attribute_GL_CONTEXT_MINOR_VERSION ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_CONTEXT_MINOR_VERSION error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_RED_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &attribute_SDL_GL_RED_SIZE ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_RED_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_GREEN_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_GREEN_SIZE, &attribute_SDL_GL_GREEN_SIZE ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_GREEN_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_BLUE_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_BLUE_SIZE, &attribute_SDL_GL_BLUE_SIZE ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_BLUE_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_ALPHA_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_ALPHA_SIZE, &attribute_SDL_GL_ALPHA_SIZE ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_ALPHA_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_DEPTH_SIZE = 0;
        if( SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &attribute_SDL_GL_DEPTH_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_DEPTH_SIZE error: %s"
                , SDL_GetError()
            );
        }

        int attribute_SDL_GL_DOUBLEBUFFER = 0;
        if( SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &attribute_SDL_GL_DOUBLEBUFFER ) != 0 )
        {
            LOGGER_WARNING( "get attribute SDL_GL_DOUBLEBUFFER error: %s"
                , SDL_GetError()
            );
        }

        LOGGER_INFO( "platform", "SDL_GL_CONTEXT_PROFILE_MASK: %d"
            , attribute_GL_CONTEXT_PROFILE_MASK
        );

        LOGGER_INFO( "platform", "SDL_GL_CONTEXT_MAJOR_VERSION: %d"
            , attribute_GL_CONTEXT_MAJOR_VERSION
        );

        LOGGER_INFO( "platform", "SDL_GL_CONTEXT_MINOR_VERSION: %d"
            , attribute_GL_CONTEXT_MINOR_VERSION
        );

        LOGGER_INFO( "platform", "SDL_GL_RED_SIZE: %d"
            , attribute_SDL_GL_RED_SIZE
        );

        LOGGER_INFO( "platform", "SDL_GL_GREEN_SIZE: %d"
            , attribute_SDL_GL_GREEN_SIZE
        );

        LOGGER_INFO( "platform", "SDL_GL_BLUE_SIZE: %d"
            , attribute_SDL_GL_BLUE_SIZE
        );

        LOGGER_INFO( "platform", "SDL_GL_ALPHA_SIZE: %d"
            , attribute_SDL_GL_ALPHA_SIZE
        );

        LOGGER_INFO( "platform", "SDL_GL_DEPTH_SIZE: %d"
            , attribute_SDL_GL_DEPTH_SIZE
        );

        LOGGER_INFO( "platform", "SDL_GL_DOUBLEBUFFER: %d"
            , attribute_SDL_GL_DOUBLEBUFFER
        );

        m_glContext = glContext;

        int drawable_width;
        int drawable_height;
        SDL_GL_GetDrawableSize( m_sdlWindow, &drawable_width, &drawable_height );

        LOGGER_INFO( "platform", "SDL drawable size [%d, %d]"
            , drawable_width
            , drawable_height
        );

        int win_width;
        int win_height;
        SDL_GetWindowSize( m_sdlWindow, &win_width, &win_height );

        LOGGER_INFO( "platform", "SDL window size [%d, %d]"
            , win_width
            , win_height
        );

        int win_min_width;
        int win_min_height;
        SDL_GetWindowMinimumSize( m_sdlWindow, &win_min_width, &win_min_height );

        LOGGER_INFO( "platform", "SDL window min size [%d, %d]"
            , win_min_width
            , win_min_height
        );

        int win_max_width;
        int win_max_height;
        SDL_GetWindowMaximumSize( m_sdlWindow, &win_max_width, &win_max_height );

        LOGGER_INFO( "platform", "SDL window max size [%d, %d]"
            , win_max_width
            , win_max_height
        );
        */

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
        int32_t width = ANativeWindow_getWidth( m_nativeWindow );
        int32_t height = ANativeWindow_getHeight( m_nativeWindow );

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

        //ToDo https://github.com/irov/Mengine/issues/93
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::hasCursorIcon( const ConstString & _icon ) const
    {
        MENGINE_UNUSED( _icon );

        //ToDo https://github.com/irov/Mengine/issues/93

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
        /*
        if( m_sdlWindow == nullptr )
        {
            return true;
        }

        Uint32 flags = SDL_GetWindowFlags( m_sdlWindow );

        bool alreadyFullscreen = (flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN;

        if( _fullscreen != alreadyFullscreen )
        {
            if( this->changeWindow_( _resolution, _fullscreen ) == false )
            {
                return false;
            }
        }
         */

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::notifyVsyncChanged( bool _vsync )
    {
        MENGINE_UNUSED( _vsync );

        /*
        if( SDL_GetWindowFlags( m_sdlWindow ) & SDL_WINDOW_OPENGL )
        {
            int profile;
            if( SDL_GL_GetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, &profile ) == -1 )
            {
                return;
            }

            if( profile == SDL_GL_CONTEXT_PROFILE_ES )
            {
                return;
            }

            if( _vsync == false )
            {
                if( SDL_GL_SetSwapInterval( 0 ) != 0 )
                {
                    LOGGER_WARNING( "notify vsync changed error: %s"
                        , SDL_GetError()
                    );
                }
            }
            else
            {
                if( SDL_GL_SetSwapInterval( 1 ) != 0 )
                {
                    LOGGER_WARNING( "notify vsync changed error: %s"
                        , SDL_GetError()
                    );
                }
            }
        }
         */
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::notifyCursorModeChanged( bool _mode )
    {
        /*
        if( _mode == true )
        {
            SDL_ShowCursor( SDL_ENABLE );
        }
        else
        {
            SDL_ShowCursor( SDL_DISABLE );
        }
         */
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
    void AndroidPlatformService::sleep( uint32_t _ms )
    {
        /*
        SDL_Delay( _ms );
         */
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
    bool AndroidPlatformService::changeWindow_( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );

        if( RENDER_SYSTEM()
            ->onWindowChangeFullscreen( _fullscreen ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::setupWindow_()
    {
        /*
        uint32_t Engine_SDL_GL_RED_SIZE = CONFIG_VALUE( "SDL", "SDL_GL_RED_SIZE", 8 );

        if( SDL_GL_SetAttribute( SDL_GL_RED_SIZE, Engine_SDL_GL_RED_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_RED_SIZE to [%u] error: %s"
                , Engine_SDL_GL_RED_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_GREEN_SIZE = CONFIG_VALUE( "SDL", "SDL_GL_GREEN_SIZE", 8 );

        if( SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, Engine_SDL_GL_GREEN_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_GREEN_SIZE to [%u] error: %s"
                , Engine_SDL_GL_GREEN_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_BLUE_SIZE = CONFIG_VALUE( "SDL", "SDL_GL_BLUE_SIZE", 8 );

        if( SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, Engine_SDL_GL_BLUE_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_BLUE_SIZE to [%u] error: %s"
                , Engine_SDL_GL_BLUE_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_ALPHA_SIZE = CONFIG_VALUE( "SDL", "SDL_GL_ALPHA_SIZE", 0 );

        if( SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, Engine_SDL_GL_ALPHA_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_ALPHA_SIZE to [%u] error: %s"
                , Engine_SDL_GL_ALPHA_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_DEPTH_SIZE = CONFIG_VALUE( "SDL", "SDL_GL_DEPTH_SIZE", 24 );

        if( SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, Engine_SDL_GL_DEPTH_SIZE ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_DEPTH_SIZE to [%u] error: %s"
                , Engine_SDL_GL_DEPTH_SIZE
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_DOUBLEBUFFER = CONFIG_VALUE( "SDL", "SDL_GL_DOUBLEBUFFER", 1 );

        if( SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, Engine_SDL_GL_DOUBLEBUFFER ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_DOUBLEBUFFER to [%u] error: %s"
                , Engine_SDL_GL_DOUBLEBUFFER
                , SDL_GetError()
            );
        }

        SDL_SetHint( SDL_HINT_RENDER_DRIVER, "opengles2" );

        uint32_t Engine_SDL_GL_CONTEXT_PROFILE_MASK = CONFIG_VALUE( "SDL", "SDL_GL_CONTEXT_PROFILE_MASK", (uint32_t)SDL_GL_CONTEXT_PROFILE_ES );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, Engine_SDL_GL_CONTEXT_PROFILE_MASK ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_PROFILE_MASK to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_PROFILE_MASK
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MAJOR_VERSION = CONFIG_VALUE( "SDL", "SDL_GL_CONTEXT_MAJOR_VERSION", 2 );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, Engine_SDL_GL_CONTEXT_MAJOR_VERSION ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_MAJOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MAJOR_VERSION
                , SDL_GetError()
            );
        }

        uint32_t Engine_SDL_GL_CONTEXT_MINOR_VERSION = CONFIG_VALUE( "SDL", "Engine_SDL_GL_CONTEXT_MINOR_VERSION", 0 );

        if( SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, Engine_SDL_GL_CONTEXT_MINOR_VERSION ) != 0 )
        {
            LOGGER_WARNING( "set attribute SDL_GL_CONTEXT_MINOR_VERSION to [%u] error: %s"
                , Engine_SDL_GL_CONTEXT_MINOR_VERSION
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_RENDER_SCALE_QUALITY = CONFIG_VALUE( "SDL", "SDL_HINT_RENDER_SCALE_QUALITY", "linear" );

        if( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, Engine_SDL_HINT_RENDER_SCALE_QUALITY ) != SDL_TRUE )
        {
            LOGGER_WARNING( "set hint SDL_HINT_RENDER_SCALE_QUALITY to [%s] error: %s"
                , Engine_SDL_HINT_RENDER_SCALE_QUALITY
                , SDL_GetError()
            );
        }

        const Char * Engine_SDL_HINT_ORIENTATIONS = CONFIG_VALUE( "SDL", "SDL_HINT_ORIENTATIONS", "Portrait" );

        if( SDL_SetHint( SDL_HINT_ORIENTATIONS, Engine_SDL_HINT_ORIENTATIONS ) != SDL_TRUE )
        {
            LOGGER_WARNING( "set hint SDL_HINT_ORIENTATIONS to [%s] error: %s"
                , Engine_SDL_HINT_ORIENTATIONS
                , SDL_GetError()
            );
        }
         */
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::createWindow_( const Resolution & _windowResolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _windowResolution );
        MENGINE_UNUSED( _fullscreen );

        if( Mengine_JNI_ExistMengineApplication() == JNI_FALSE )
        {
            return false;
        }

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        jobject surface = Helper::AndroidGetActivitySurface( jenv );

        ANativeWindow * nativeWindow = ANativeWindow_fromSurface( jenv, surface );

        m_nativeWindow = nativeWindow;

        EGLDisplay eglDisplay = ::eglGetDisplay( EGL_DEFAULT_DISPLAY );

        EGLint major;
        EGLint minor;
        if( ::eglInitialize( eglDisplay, &major, &minor ) == EGL_FALSE )
        {
            return false;
        }

        m_eglDisplay = eglDisplay;

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
            return false;
        }

        EGLSurface eglSurface = ::eglCreateWindowSurface( m_eglDisplay, config, m_nativeWindow, NULL );

        if( eglSurface == EGL_NO_SURFACE )
        {
            return false;
        }

        m_eglSurface = eglSurface;

        const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };

        EGLContext eglShareContext = EGL_NO_CONTEXT;

        EGLContext context = ::eglCreateContext( m_eglDisplay, config, eglShareContext, contextAttribs );

        if( context == EGL_NO_CONTEXT )
        {
            return false;
        }

        m_eglContext = context;

        if( ::eglMakeCurrent( m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext ) == EGL_FALSE )
        {
            return false;
        }

        //::dlsym( handleGLESv2, "eglGetDisplay" );

        /*
        SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );

        Uint32 windowFlags = 0;

        windowFlags |= SDL_WINDOW_OPENGL;
        windowFlags |= SDL_WINDOW_SHOWN;
        windowFlags |= SDL_WINDOW_RESIZABLE;
        windowFlags |= SDL_WINDOW_FULLSCREEN;
        windowFlags |= SDL_WINDOW_BORDERLESS;

        LOGGER_INFO( "platform", "num video displays: %d"
            , SDL_GetNumVideoDisplays()
        );
        
        const Char * projectTitle_str = m_projectTitle.c_str();

        SDL_Window * window = SDL_CreateWindow( projectTitle_str
            , SDL_WINDOWPOS_UNDEFINED
            , SDL_WINDOWPOS_UNDEFINED
            , -1
            , -1
            , windowFlags );

        if( window == nullptr )
        {
            LOGGER_ERROR( "create window failed: %s"
                , SDL_GetError()
            );

            return false;
        }

        m_sdlWindow = window;

        LOGGER_INFO( "platform", "SDL_HINT_RENDER_DRIVER: %s", SDL_GetHint( SDL_HINT_RENDER_DRIVER ) );
        LOGGER_INFO( "platform", "SDL_HINT_RENDER_SCALE_QUALITY: %s", SDL_GetHint( SDL_HINT_RENDER_SCALE_QUALITY ) );
        LOGGER_INFO( "platform", "SDL_HINT_ORIENTATIONS: %s", SDL_GetHint( SDL_HINT_ORIENTATIONS ) );

        return true;
         */

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::destroyWindow_()
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_DETACH_WINDOW );

        /*
        if( m_glContext != nullptr )
        {
            SDL_GL_DeleteContext( m_glContext );
            m_glContext = nullptr;
        }

        if( m_sdlWindow != nullptr )
        {
            SDL_DestroyWindow( m_sdlWindow );
            m_sdlWindow = nullptr;
        }
         */
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidPlatformService::processEvents_()
    {
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
    void AndroidPlatformService::setActive_( bool _active )
    {
        if( m_active == _active )
        {
            return;
        }

        m_active = _active;

        bool nopause = APPLICATION_SERVICE()
            ->getNopause();

        /*
        mt::vec2f point;
        m_androidInput->getCursorPosition( m_sdlWindow, &point );

        if( m_active == false )
        {
            if( nopause == false )
            {
                Helper::pushMouseLeaveEvent( TC_TOUCH0, point.x, point.y, 0.f );
            }
        }
        else
        {
            Helper::pushMouseEnterEvent( TC_TOUCH0, point.x, point.y, 0.f );
        }
         */

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
    void AndroidPlatformService::setAndroidNativeWindow( ANativeWindow * _nativeWindow )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::destroyAndroidNativeWindow( ANativeWindow * _nativeWindow )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidPlatformService::changeAndroidNativeWindow( ANativeWindow * _nativeWindow, jint surfaceWidth, jint surfaceHeight, jint deviceWidth, jint deviceHeight, jfloat rate )
    {

    }
    //////////////////////////////////////////////////////////////////////////
}
