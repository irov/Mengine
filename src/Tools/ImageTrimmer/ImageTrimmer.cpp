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

#   include "WindowsLayer/VistaWindowsLayer.h"

#	include "Logger/Logger.h"
	
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( MengeWin32FileGroup );
PLUGIN_EXPORT( MengeImageCodec );
PLUGIN_EXPORT( MengeZip );
PLUGIN_EXPORT( MengeLZ4 );
//////////////////////////////////////////////////////////////////////////
static void message_error( const char * _format, ... )
{
	va_list argList;

	va_start(argList, _format);

	char str[2048];

	vsnprintf( str, 2048 - 1, _format, argList );	

	va_end(argList);

	MessageBoxA( NULL, str, "ImageTrimmer", MB_OK );
}
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider )

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
		PLUGIN_CREATE( serviceProvider, MengeImageCodec );

		if( FILE_SERVICE(serviceProvider)
			->mountFileGroup( ConstString::none(), ConstString::none(), Helper::stringizeString(serviceProvider, "dir") ) == false )
		{
			return false;
		}

		ConstString dev = Helper::stringizeString(serviceProvider, "dev");

		if( FILE_SERVICE(serviceProvider)
			->mountFileGroup( dev, ConstString::none(), Helper::stringizeString(serviceProvider, "dir") ) == false )
		{
			return false;
		}

		*_serviceProvider = serviceProvider;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool trimImage( Menge::ServiceProviderInterface * serviceProvider, const WString & in, const WString & out, const WString & info )
	{
		String utf8_in;
		Helper::unicodeToUtf8(serviceProvider, in, utf8_in);

		ConstString c_in = Helper::stringizeString(serviceProvider, utf8_in);

		InputStreamInterfacePtr input_stream = 
			FILE_SERVICE(serviceProvider)->openInputFile( ConstString::none(), c_in, false );

		if( input_stream == nullptr )
		{
			return false;
		}

		const ConstString & codecType = CODEC_SERVICE(serviceProvider)
			->findCodecType( c_in );

		if( codecType.empty() == true )
		{
			return false;
		}

		//message_error( "test" );

		ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( codecType );

		if( imageDecoder == nullptr )
		{
			return false;
		}

		if( imageDecoder->prepareData( input_stream ) == false )
		{
			return false;
		}

		const ImageCodecDataInfo* decode_dataInfo = imageDecoder->getCodecDataInfo();

		if( decode_dataInfo->width == 0 ||
			decode_dataInfo->height == 0 )
		{
			return false;
		}

		ImageCodecOptions decode_options;

		decode_options.channels = decode_dataInfo->channels;
		decode_options.pitch = decode_dataInfo->width * decode_dataInfo->channels;
		
		imageDecoder->setOptions( &decode_options );
				
		size_t width = decode_dataInfo->width;
		size_t height = decode_dataInfo->height;
		
		size_t bufferSize = width * height * decode_dataInfo->channels;

		MemoryInterfacePtr memory_textureBuffer = MEMORY_SERVICE( serviceProvider )
			->createMemory();

		if( memory_textureBuffer == nullptr )
		{
			return false;
		}

		unsigned char * textureBuffer = memory_textureBuffer->newMemory( bufferSize );

		if( textureBuffer == nullptr )
		{
			return false;
		}

		if( imageDecoder->decode( textureBuffer, bufferSize ) == 0U )
		{
			return false; 
		}

		size_t new_width;
		size_t new_height;

		size_t min_i;
		size_t min_j;
		size_t max_i;
		size_t max_j;
		
		if( decode_dataInfo->channels == 4 )
		{
			min_i = width - 1;
			min_j = height - 1;
			max_i = 0;
			max_j = 0;

			for( size_t i = 0; i != width; ++i )
			{
				for( size_t j = 0; j != height; ++j )
				{
					size_t index =  i + j * width;
					unsigned char alpha = textureBuffer[index * 4 + 3];

					if( alpha == 0 )
					{
						continue;
					}

					if( min_i > i )
					{
						min_i = i;
					}

					if( min_j > j )
					{
						min_j = j;
					}

					if( max_i < i )
					{
						max_i = i;
					}

					if( max_j < j )
					{
						max_j = j;
					}
				}
			}

			if( min_i > max_i || min_j > max_j )
			{
				new_width = 1;
				new_height = 1;

				min_i = 0;
				min_j = 0;
				max_i = 1;
				max_j = 1;
			}
			else
			{
				new_width = max_i - min_i + 1;
				new_height = max_j - min_j + 1;
			}
		}
		else
		{
			new_width = width;
			new_height = height;

			min_i = 0;
			min_j = 0;
			max_i = width;
			max_j = height;
		}

		size_t new_bufferSize = new_width * new_height * decode_dataInfo->channels;

		MemoryInterfacePtr buffer = MEMORY_SERVICE( serviceProvider )
			->createMemory();

		if( buffer == nullptr )
		{
			return false;
		}

		unsigned char * new_textureBuffer = buffer->newMemory( new_bufferSize );

		if( new_textureBuffer == nullptr )
		{
			return false;
		}
		
		for( size_t i = 0; i != new_width; ++i )
		{
			for( size_t j = 0; j != new_height; ++j )
			{
				size_t old_index = (min_i + i) + (min_j + j) * width;
				size_t new_index = i + j * new_width;

				for( size_t k = 0; k != decode_dataInfo->channels; ++k )
				{
					new_textureBuffer[new_index * decode_dataInfo->channels + k] = textureBuffer[old_index * decode_dataInfo->channels + k];
				}
			}
		}

		String utf8_out;
		Helper::unicodeToUtf8(serviceProvider, out, utf8_out);

		ConstString c_out = Helper::stringizeString(serviceProvider, utf8_out);

		OutputStreamInterfacePtr output_stream = FILE_SERVICE(serviceProvider)
			->openOutputFile( ConstString::none(), c_out );

		if( output_stream == nullptr )
		{
			return false;
		}

		ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE(serviceProvider)
			->createEncoderT<ImageEncoderInterfacePtr>( codecType );

		if( imageEncoder == nullptr )
		{
			return false;
		}

		if( imageEncoder->initialize( output_stream ) == false )
		{
			return false;
		}

		ImageCodecOptions encode_options;

		encode_options.pitch = new_width * decode_dataInfo->channels;
		encode_options.channels = decode_dataInfo->channels;

		imageEncoder->setOptions( &encode_options );

		ImageCodecDataInfo encode_dataInfo;
		//dataInfo.format = _image->getHWPixelFormat();
		encode_dataInfo.width = new_width;
		encode_dataInfo.height = new_height;
		encode_dataInfo.channels = decode_dataInfo->channels;
		encode_dataInfo.depth = 1;
		encode_dataInfo.mipmaps = 1;

		if( imageEncoder->encode( new_textureBuffer, new_bufferSize, &encode_dataInfo ) == 0 )
		{
			return false;
		}

		String utf8_info;
		Helper::unicodeToUtf8(serviceProvider, info, utf8_info);

		ConstString c_info = Helper::stringizeString(serviceProvider, utf8_info);

		OutputStreamInterfacePtr info_stream = FILE_SERVICE(serviceProvider)
			->openOutputFile( ConstString::none(), c_info );

		if( info_stream == nullptr )
		{
			return false;
		}

		char info_buffer[1024];

		sprintf(info_buffer, "%u\n%u\n%u\n%u\n%u\n%u\n"
			, width
			, height
			, new_width
			, new_height
			, min_i
			, min_j
			);

		size_t info_buffer_size = strlen( info_buffer );

		if( info_stream->write( info_buffer, info_buffer_size + 1 ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool recanvasImage( Menge::ServiceProviderInterface * serviceProvider, const WString & in, const WString & out, const WString & params )
	{
		String utf8_in;
		Helper::unicodeToUtf8(serviceProvider, in, utf8_in);

		ConstString c_in = Helper::stringizeString(serviceProvider, utf8_in);

		InputStreamInterfacePtr input_stream = 
			FILE_SERVICE(serviceProvider)->openInputFile( ConstString::none(), c_in, false );

		if( input_stream == nullptr )
		{
			return false;
		}

		const ConstString & codecType = CODEC_SERVICE(serviceProvider)
			->findCodecType( c_in );

		if( codecType.empty() == true )
		{
			return false;
		}

		ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(serviceProvider)
			->createDecoderT<ImageDecoderInterfacePtr>( codecType );

		if( imageDecoder == nullptr )
		{
			return false;
		}

		if( imageDecoder->prepareData( input_stream ) == false )
		{
			return false;
		}

		const ImageCodecDataInfo* decode_dataInfo = imageDecoder->getCodecDataInfo();

		if( decode_dataInfo->width == 0 ||
			decode_dataInfo->height == 0 )
		{
			return false;
		}

		ImageCodecOptions decode_options;

		decode_options.channels = decode_dataInfo->channels;
		decode_options.pitch = decode_dataInfo->width * decode_dataInfo->channels;

		imageDecoder->setOptions( &decode_options );

		size_t width = decode_dataInfo->width;
		size_t height = decode_dataInfo->height;

		size_t bufferSize = width * height * decode_dataInfo->channels;

		MemoryInterfacePtr memory_textureBuffer = MEMORY_SERVICE( serviceProvider )
			->createMemory();
		
		if( memory_textureBuffer == nullptr )
		{
			return false;
		}

		unsigned char * textureBuffer = memory_textureBuffer->newMemory( bufferSize );

		if( imageDecoder->decode( textureBuffer, bufferSize ) == 0U )
		{
			return false;
		}

		uint32_t new_width;
		uint32_t new_height;

		if( params == L"pow2" )
		{
			new_width = Helper::getTexturePOW2( width );
			new_height = Helper::getTexturePOW2( height );
		}
		else
		{
			if( swscanf( params.c_str(), L"%d;%d", &new_width, &new_height ) != 2 )
			{
				return false;
			}
		}

		if( new_width < width || new_height < height )
		{
			return false;
		}
		
		size_t new_bufferSize = new_width * new_height * decode_dataInfo->channels;

		MemoryInterfacePtr memory_new_textureBuffer = MEMORY_SERVICE( serviceProvider )
			->createMemory();

		if( memory_new_textureBuffer == nullptr )
		{
			return false;
		}

		unsigned char * new_textureBuffer = memory_new_textureBuffer->newMemory( new_bufferSize );

		if( new_textureBuffer == nullptr )
		{
			return false;
		}
		
		uint32_t channels = decode_dataInfo->channels;

		for( size_t i = 0; i != width; ++i )
		{
			for( size_t j = 0; j != height; ++j )
			{
				size_t old_index = i + j * width;
				size_t new_index = i + j * new_width;

				for( size_t k = 0; k != channels; ++k )
				{
					new_textureBuffer[new_index * channels + k] = textureBuffer[old_index * channels + k];
				}
			}
		}

		String utf8_out;
		Helper::unicodeToUtf8(serviceProvider, out, utf8_out);

		ConstString c_out = Helper::stringizeString(serviceProvider, utf8_out);

		OutputStreamInterfacePtr output_stream = FILE_SERVICE(serviceProvider)
			->openOutputFile( ConstString::none(), c_out );

		if( output_stream == nullptr )
		{
			return false;
		}

		ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE(serviceProvider)
			->createEncoderT<ImageEncoderInterfacePtr>( codecType );

		if( imageEncoder == nullptr )
		{
			return false;
		}

		if( imageEncoder->initialize( output_stream ) == false )
		{
			return false;
		}

		ImageCodecOptions encode_options;

		encode_options.pitch = new_width * decode_dataInfo->channels;
		encode_options.channels = decode_dataInfo->channels;

		imageEncoder->setOptions( &encode_options );

		ImageCodecDataInfo encode_dataInfo;
		//dataInfo.format = _image->getHWPixelFormat();
		encode_dataInfo.width = new_width;
		encode_dataInfo.height = new_height;
		encode_dataInfo.channels = decode_dataInfo->channels;
		encode_dataInfo.depth = 1;
		encode_dataInfo.mipmaps = 1;

		if( imageEncoder->encode( new_textureBuffer, new_bufferSize, &encode_dataInfo ) == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool lz4hcImage( Menge::ServiceProviderInterface * serviceProvider, const WString & in, const WString & out )
	{
		String utf8_in;
		Helper::unicodeToUtf8(serviceProvider, in, utf8_in);

		ConstString c_in = Helper::stringizeString(serviceProvider, utf8_in);

		InputStreamInterfacePtr input_stream = 
			FILE_SERVICE(serviceProvider)->openInputFile( ConstString::none(), c_in, false );

		if( input_stream == nullptr )
		{
			return false;
		}

		size_t bufferSize = input_stream->size();

		MemoryInterfacePtr memory_textureBuffer = MEMORY_SERVICE( serviceProvider )
			->createMemory();

		if( memory_textureBuffer == nullptr )
		{
			return false;
		}

		unsigned char * textureBuffer = memory_textureBuffer->newMemory( bufferSize );

		if( textureBuffer == nullptr )
		{
			return false;
		}

		if( input_stream->read( textureBuffer, bufferSize ) == 0U )
		{
			return false;
		}

		ArchivatorInterfacePtr archivator = ARCHIVE_SERVICE(serviceProvider)
			->getArchivator( STRINGIZE_STRING_LOCAL(serviceProvider, "lz4") );

		if( archivator == nullptr )
		{
			return false;
		}

		size_t compressBound = archivator->compressBound( bufferSize );

		MemoryInterfacePtr memory_compressBuffer = MEMORY_SERVICE( serviceProvider )
			->createMemory();

		if( memory_compressBuffer == nullptr )
		{
			return false;
		}

		unsigned char * compressBuffer = memory_compressBuffer->newMemory( compressBound );

		if( compressBuffer == nullptr )
		{
			return false;
		}

		size_t compressSize = 0;
		archivator->compress( compressBuffer, compressBound, textureBuffer, bufferSize, compressSize );
		
		String utf8_out;
		Helper::unicodeToUtf8(serviceProvider, out, utf8_out);

		ConstString c_out = Helper::stringizeString(serviceProvider, utf8_out);

		OutputStreamInterfacePtr output_stream = FILE_SERVICE(serviceProvider)
			->openOutputFile( ConstString::none(), c_out );

		if( output_stream == nullptr )
		{
			return false;
		}
		
		if( output_stream->write( compressBuffer, compressSize ) == 0 )
		{
			return false;
		}

		return true;
	}	
}

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)nShowCmd;

	stdex_allocator_initialize();

	int cmd_num;
	LPWSTR * cmd_args = CommandLineToArgvW( lpCmdLine, &cmd_num );

	Menge::WString command;	
	Menge::WString in;
	Menge::WString out;
	Menge::WString params;
	Menge::WString info;

	for( int i = 0; i < cmd_num; i += 2 )
	{
		LPWSTR arg = cmd_args[i + 0];
		LPWSTR value = cmd_args[i + 1];

		if( wcscmp( arg, L"-command" ) == 0 )
		{
			command = value;
		}
		else if( wcscmp( arg, L"-in" ) == 0 )
		{
			in = value;
		}
		else if( wcscmp( arg, L"-out" ) == 0 )
		{
			out = value;
		}
		else if( wcscmp( arg, L"-params" ) == 0 )
		{
			params = value;
		}
		else if( wcscmp( arg, L"-info" ) == 0 )
		{
			info = value;
		}
	}

	if( command.empty() == true )
	{
		command = L"trim";
	}

	if( in.empty() == true )
	{
		message_error("not found 'in' param"
			);

		return 0;
	}

	if( out.empty() == true )
	{
		message_error("not found 'out' param"
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
		MessageBoxA( NULL, se.what(), "Mengine exception", MB_OK );

		return 0;
	}
	
	if( command == L"trim" )
	{
		if( Menge::trimImage( serviceProvider, in, out, info ) == false )
		{
			message_error( "ImageTrimmer invalid trim %ls"
				, in.c_str()
				);

			return 0;
		}
	}
	else if( command == L"recanvas" )
	{
		if( Menge::recanvasImage( serviceProvider, in, out, params ) == false )
		{
			message_error( "ImageTrimmer invalid recanvas %ls"
				, in.c_str()
				);

			return 0;
		}		
	}
	else if( command == L"lz4hc" )
	{
		if( Menge::lz4hcImage( serviceProvider, in, out ) == false )
		{
			message_error( "ImageTrimmer invalid lz4hc %ls"
				, in.c_str()
				);

			return 0;
		}		
	}


	return 0;
}