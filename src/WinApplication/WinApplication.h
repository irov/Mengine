#	pragma once

#	include "Interface/ApplicationInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/NotificationServiceInterface.h"
#   include "Interface/LoggerInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/MemoryInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#   include "Interface/PhysicSystemInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/SoundSystemInterface.h"
#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ParticleSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/DataInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/InputSystemInterface.h"
#   include "Interface/PluginInterface.h"
#	include "Interface/WindowsLayerInterface.h"
#	include "Interface/HttpSystemInterface.h"
#	include "Interface/PrefetcherInterface.h"
#	include "Interface/ConfigInterface.h"
#	include "Interface/TimerInterface.h"

#   include <stdex/stl_map.h>

namespace Menge
{
	class SystemDLL;
	class ConsoleLogger;
	class MessageBoxLogger;
	class Application;
	
	class FileLogger;

	//////////////////////////////////////////////////////////////////////////
	class WinApplication
	{
	public:
		WinApplication();
		~WinApplication();

	public:
		bool initialize();
		void finalize();

	public:
		void update();
		
	public:
		void loop();

	protected:												
		bool initializeFileEngine_();
		bool initializeUserDirectory_();
		bool initializeLogFile_();
		bool initializeLogEngine_();
		bool initializeConfigEngine_();
		bool initializeArchiveService_();
		bool initializeModuleEngine_();
		bool initializeDataManager_();
		bool initializeMemoryManager_();
		bool initializeHttpService_();
		bool initializePrefetcherService_();
		bool initializeTimerService_();

	protected:
		bool getApplicationPath_( const char * _section, const char * _key, ConstString & _value );

	protected:
		bool makeUserPath_( WString & _wstring ) const;

	private:
		ServiceProviderInterface * m_serviceProvider;
		
		FileLogger * m_fileLog;

		ConsoleLogger * m_loggerConsole;
		MessageBoxLogger * m_loggerMessageBox;

		typedef stdex::vector<PluginInterface *> TVectorPlugins;
		TVectorPlugins m_plugins;	
	};
}	// namespace Menge
