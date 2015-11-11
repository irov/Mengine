#pragma once

#	include "Interface/ApplicationInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/NotificationServiceInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/SoundSystemInterface.h"
#	include "Interface/PhysicSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/PluginInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/ModuleInterface.h"
#	include "Interface/DataInterface.h"
#	include "Interface/MemoryInterface.h"
#	include "Interface/ConfigInterface.h"
#	include "Interface/PrefetcherInterface.h"

#   include "MarmaladeInput.h"

#	include "Core/FileLogger.h"

namespace Menge
{
    class MarmaladePlatform
		: public ServiceBase<PlatformInterface>
    {
    public:
		MarmaladePlatform();
		~MarmaladePlatform();

    public:
        bool _initialize() override;
        void _finalize() override;

	public:
        void update() override;
        void stop()	override;

	public:
		bool createWindow( uint32_t _icon, const Menge::WString & _projectTitle, const Resolution & _resolution, bool _fullscreen ) override;
		WindowHandle getWindowHandle() const override;

	public:
		const ConstString & getPlatformName() const override;

    public:
        void getDesktopResolution( Resolution & _resolution ) const override;

		size_t getCurrentPath( WChar * _path, size_t _len ) const override;

        void minimizeWindow() override;

        void setCursorPosition( const mt::vec2f & _pos ) override;

        void showKeyboard() override;
        void hideKeyboard() override;

        void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
        void notifyVsyncChanged( bool _vsync ) override;
        void notifyCursorModeChanged( bool _mode ) override;
		bool notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer ) override;

	public:
		void onEvent( const ConstString & _event, const TMapParams & _params ) override;

	public:
		size_t getShortPathName( const WString & _path, WChar * _short, size_t _len ) const override;

	public:
		void getMaxClientResolution( Resolution & _resolution ) const override;

	public:
		bool openUrlInDefaultBrowser( const WString & _url ) override;
		
	public:
		bool createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;
		bool createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size ) override;
		
	public:
		bool concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, WChar * _filePath, size_t _capacity ) override;

	protected:
		bool getApplicationPath_( const char * _section, const char * _key, ConstString & _path ) const;

	protected:
		bool initializeConfigEngine_();
		bool initializeLogEngine_();
		bool initializeArchiveService_();
		bool initializeFileEngine_();
		bool initializeRenderEngine_();
		void initializeMarmaladePauseCallback_();
		void initializeMarmaladeSurfaceScreenSizeCallback_();

	public:
		void changePause_( bool _value );
		void changeScreenSize_( const Resolution & resolution );

	protected:
		MarmaladeInput * m_marmaladeInput;
				
		ConstString m_platformName;

		bool m_running;
		bool m_pause;
    };
}
