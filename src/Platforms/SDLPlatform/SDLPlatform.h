#pragma once

#include "Interface/ApplicationInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"

#include "Kernel/FileLogger.h"
#include "Kernel/Factory.h"
#include "Kernel/ServiceBase.h"

#include "SDLInput.h"

#include "SDL.h"

namespace Mengine
{
    class SDLPlatform
        : public ServiceBase<PlatformInterface>
    {
    public:
        SDLPlatform();
        ~SDLPlatform() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool runPlatform()	override;
        void updatePlatform() override;
        void stopPlatform()	override;

    public:
        void setIcon( uint32_t _icon ) override;
        uint32_t getIcon() const override;

        void setProjectTitle( const Char * _projectTitle ) override;
        size_t getProjectTitle( Char * _projectTitle ) const override;

    public:
        bool createWindow( const Resolution & _resolution, bool _fullscreen ) override;
        Pointer getWindowHandle() const override;

    public:
        bool hasPlatformTag( const ConstString & _tag ) const override;
        const Tags & getPlatformTags() const override;

    public:
        bool hasTouchpad() const override;

    public:
        DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName ) override;

    public:
        bool getDesktopResolution( Resolution & _resolution ) const override;

        bool getNoFullscreen() const override;
        bool getAlwaysFullscreen() const override;

        bool setProcessDPIAware() override;
		bool createProcessDump( const Char * _dumpPath, void * _pExceptionPointers, bool _full ) override;

        size_t getCurrentPath( Char * _path ) const override;
        size_t getUserPath( Char * _path ) const override;

        void minimizeWindow() override;

        void setCursorPosition( const mt::vec2f & _pos ) override;

        void showKeyboard() override;
        void hideKeyboard() override;

        void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
        void notifyVsyncChanged( bool _vsync ) override;
        void notifyCursorModeChanged( bool _mode ) override;
        bool notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer ) override;

    public:
        void onEvent( const ConstString & _event, const MapWParams & _params ) override;

    public:
        float getJoystickAxis( uint32_t _index ) const override;

    public:
        size_t getShortPathName( const Char * _path, Char * _short, size_t _len ) const override;

    public:
        void getMaxClientResolution( Resolution & _resolution ) const override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;

    public:
        bool existDirectory( const Char * _path ) const override;
        bool createDirectory( const Char * _path ) override;

    public:
        bool existFile( const Char * _path ) override;
        bool removeFile( const Char * _path ) override;

    public:
        bool findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        uint64_t getFileTime( const Char * _filePath ) const override;

    public:
        void getDateTime( PlatformDateTime * _dateTime ) const override;        

    public:
        bool createDirectoryUserPicture( const Char * _path, const Char * _file, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const Char * _path, const Char * _file, const void * _data, size_t _size ) override;

    public:
        bool getErrorMessage( uint32_t _messageId, Char * _out, size_t _capacity ) const override;
        void sleep( uint32_t _ms ) override;
        bool createProcess( const Char * _process, const Char * _command, bool _wait, uint32_t * _code ) override;
        bool getLocalMachineRegValue( const Char* _path, const Char* _key, Char* _value, size_t _size ) override;
        void abort() override;

    protected:
        void changeWindow_( const Resolution & _resolution, bool _fullscreen );
        bool createWindow_( const Resolution & _resolution, bool _fullscreen );
        void destroyWindow_();

    protected:
        bool processEvents();

    protected:
        void setActive_( bool _active );

    protected:
        Tags m_platformName;

        SDL_Window * m_window;

        SDL_Joystick * m_accelerometer;

        FactoryPtr m_factoryDynamicLibraries;

        SDL_GLContext m_glContext;

        SDLInputPtr m_sdlInput;

        uint32_t m_icon;
        String m_projectTitle;

		uint64_t m_prevTime;

        bool m_shouldQuit;
        bool m_running;
        bool m_pause;

        bool m_touchpad;
    };
}
