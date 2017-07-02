#	include <Windows.h>

#	include <shellapi.h>

#	include <stdio.h>

#	include <string>
#	include <vector>

#	include "Interface/PluginInterface.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/LoggerInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/DataInterface.h"
#	include "Interface/MemoryInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/ImageCodecInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/ConfigInterface.h"
#	include "Interface/XmlCodecInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#	include "Logger/Logger.h"
#	include "ToolUtils/ToolUtils.h"
	
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( MengeWin32FileGroup );
PLUGIN_EXPORT( MengeZip );
PLUGIN_EXPORT( MengeLZ4 );
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider )

SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( UnicodeService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( WindowsLayer );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( PluginSystem );
SERVICE_EXTERN( PluginService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	static bool initializeEngine( Menge::ServiceProviderInterface ** _serviceProvider )
	{
		Menge::ServiceProviderInterface * serviceProvider;
		SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

		SERVICE_CREATE( serviceProvider, FactoryService );

		SERVICE_CREATE( serviceProvider, UnicodeSystem );
		SERVICE_CREATE( serviceProvider, UnicodeService );

		SERVICE_CREATE( serviceProvider, StringizeService );
		SERVICE_CREATE( serviceProvider, ArchiveService );

		SERVICE_CREATE( serviceProvider, LoggerService );

		class MyLogger
			: public LoggerInterface
		{
		public:
			bool initialize() override 
			{ 
				return true; 
			}

			void finalize() override 
			{
			};

			void setServiceProvider(ServiceProviderInterface * _serviceProvider) override
			{
				m_serviceProvider = _serviceProvider;
			}

			ServiceProviderInterface * getServiceProvider() const override
			{
				return m_serviceProvider;
			}

			MyLogger()
				: m_verboseLevel(LM_WARNING)
				, m_verboseFlag(0xFFFFFFFF)
			{
			}

		public:
			void setVerboseLevel( EMessageLevel _level ) override 
			{
				m_verboseLevel = _level;
			};

			void setVerboseFlag( size_t _flag ) override 
			{
				m_verboseFlag = _flag;
			};

		public:
			bool validMessage( EMessageLevel _level, size_t _flag ) const override 
			{ 
				if( m_verboseLevel < _level )
				{
					return false;
				}

				if( _flag == 0 )
				{
					return true;
				}

				if( (m_verboseFlag & _flag) == 0 )
				{
					return false;
				}

				return true;
			};

		public:
			void log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count ) override
			{
				(void)_level;
				(void)_flag;
				(void)_count;

				message_error("%s"
					, _data
					);
			}

			void flush() override 
			{
			}

		protected:
			EMessageLevel m_verboseLevel;
			uint32_t m_verboseFlag;

		private:
			ServiceProviderInterface * m_serviceProvider;
		};

		LOGGER_SERVICE(serviceProvider)
			->setVerboseLevel( LM_WARNING );

		LOGGER_SERVICE( serviceProvider )
			->registerLogger( new MyLogger );
				
		SERVICE_CREATE( serviceProvider, CodecService );
		SERVICE_CREATE( serviceProvider, DataService );
		SERVICE_CREATE( serviceProvider, ConfigService );
		
		SERVICE_CREATE( serviceProvider, ThreadSystem );

		SERVICE_CREATE( serviceProvider, ThreadService );
		SERVICE_CREATE( serviceProvider, MemoryService );
		SERVICE_CREATE( serviceProvider, PluginSystem );
		SERVICE_CREATE( serviceProvider, PluginService );

		SERVICE_CREATE( serviceProvider, WindowsLayer );
		SERVICE_CREATE( serviceProvider, FileService );

		PLUGIN_CREATE( serviceProvider, MengeWin32FileGroup );
		
		PLUGIN_CREATE( serviceProvider, MengeZip );
		PLUGIN_CREATE( serviceProvider, MengeLZ4 );

		if( FILE_SERVICE(serviceProvider)
			->mountFileGroup( ConstString::none(), ConstString::none(), Helper::emptyPath(), Helper::stringizeString(serviceProvider, "dir") ) == false )
		{
			return false;
		}

		ConstString dev = Helper::stringizeString(serviceProvider, "dev");

		if( FILE_SERVICE(serviceProvider)
			->mountFileGroup( dev, ConstString::none(), Helper::emptyPath(), Helper::stringizeString(serviceProvider, "dir") ) == false )
		{
			return false;
		}

		*_serviceProvider = serviceProvider;

		return true;
	}
}
//////////////////////////////////////////////////////////////////////////
static void parse_arg( const std::wstring & _str, Menge::WString & _value )
{
	_value = Menge::WString( _str.begin(), _str.end() );
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)nShowCmd;
	
	stdex_allocator_initialize();

	Menge::WString protocol = parse_kwds( lpCmdLine, L"--protocol", Menge::WString() );
	Menge::WString in = parse_kwds( lpCmdLine, L"--in", Menge::WString() );
	Menge::WString out = parse_kwds( lpCmdLine, L"--out", Menge::WString() );

	if( in.empty() == true )
	{
		message_error("not found 'in' param"
			);

		return 0;
	}

	Menge::ServiceProviderInterface * serviceProvider;

	try
	{
		if( Menge::initializeEngine( &serviceProvider ) == false )
		{
			message_error( "ImageTrimmer invalid initialize" );

			return 0;
		}
	}
	catch( const std::exception & se )
	{		
		message_error( "Mengine exception %s"
			, se.what()
			);

		return 0;
	}

	Menge::FilePath fp_protocol = Menge::Helper::unicodeToFilePath( serviceProvider, protocol );
	Menge::FilePath fp_in = Menge::Helper::unicodeToFilePath( serviceProvider, in );
	Menge::FilePath fp_out = Menge::Helper::unicodeToFilePath( serviceProvider, out );

	if( PLUGIN_SERVICE( serviceProvider )
		->loadPlugin( L"MengeXmlCodecPlugin.dll" ) == false )
	{
		return 0;
	}

	Menge::XmlDecoderInterfacePtr decoder = CODEC_SERVICE( serviceProvider )
		->createDecoderT<Menge::XmlDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL( serviceProvider, "xml2bin" ) );

	if( decoder == nullptr )
	{
		LOGGER_ERROR( serviceProvider )("LoaderEngine::makeBin_ invalid create decoder xml2bin for %s"
			, fp_in.c_str()
			);

		return 0;
	}

	if( decoder->prepareData( nullptr ) == false )
	{
		LOGGER_ERROR( serviceProvider )("LoaderEngine::makeBin_ invalid initialize decoder xml2bin for %s"
			, fp_in.c_str()
			);

		return 0;
	}

	Menge::XmlCodecOptions options;
	options.pathProtocol = fp_protocol;

	Menge::FileGroupInterfacePtr fileGroup = FILE_SERVICE( serviceProvider )
		->getFileGroup( Menge::ConstString::none() );

	if( fileGroup == nullptr )
	{
		LOGGER_ERROR( serviceProvider )("LoaderEngine::makeBin_ invalid get file group"
			);

		return 0;
	}

	const Menge::FilePath & path = fileGroup->getPath();

	options.pathXml = Menge::Helper::concatenationFilePath( serviceProvider, path, fp_in );
	options.pathBin = Menge::Helper::concatenationFilePath( serviceProvider, path, fp_out );

	if( decoder->setOptions( &options ) == false )
	{
		return 0;
	}

	if( decoder->decode( 0, 0 ) == 0 )
	{
		return 0;
	}
	
	return 0;
}