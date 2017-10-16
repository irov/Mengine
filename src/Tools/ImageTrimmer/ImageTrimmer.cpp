#	include <Windows.h>

#	include <shellapi.h>

#	include <stdio.h>

#	include <string>
#	include <vector>

#	include "Interface/PluginInterface.h"

#	include "Interface/FactoryInterface.h"
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
#	include "ToolUtils/ToolUtils.h"
	
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( MengeWin32FileGroup );
PLUGIN_EXPORT( MengeImageCodec );
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
		PLUGIN_CREATE( serviceProvider, MengeImageCodec );

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
	//////////////////////////////////////////////////////////////////////////
	static bool trimImage( Menge::ServiceProviderInterface * serviceProvider, const WString & in, const WString & out, const WString & info )
	{
		String utf8_in;
		Helper::unicodeToUtf8(serviceProvider, in, utf8_in);

		FilePath c_in = Helper::stringizeFilePath(serviceProvider, utf8_in);

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

		unsigned char * textureBuffer = memory_textureBuffer->newMemory( bufferSize, __FILE__, __LINE__ );

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

            new_width += 2;
            new_height += 2;
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

		unsigned char * new_textureBuffer = buffer->newMemory( new_bufferSize, __FILE__, __LINE__ );

		if( new_textureBuffer == nullptr )
		{
			return false;
		}
		
		for( size_t i = 0; i != new_width; ++i )
		{
			for( size_t j = 0; j != new_height; ++j )
			{
                size_t new_index = i + j * new_width;

                if( decode_dataInfo->channels == 4 && (i == 0 || j == 0 || i == new_width - 1 || j == new_height - 1) )
                {
                    for( size_t k = 0; k != decode_dataInfo->channels; ++k )
                    {
                        new_textureBuffer[new_index * decode_dataInfo->channels + k] = 0;
                    }
                }

                size_t old_index = decode_dataInfo->channels == 4 ? (min_i + (i - 1)) + (min_j + (j - 1)) * width : (min_i + i) + (min_j + j) * width;

                for( size_t k = 0; k != decode_dataInfo->channels; ++k )
				{
					new_textureBuffer[new_index * decode_dataInfo->channels + k] = textureBuffer[old_index * decode_dataInfo->channels + k];
				}
			}
		}

		if( out.empty() == false )
		{
			String utf8_out;
			Helper::unicodeToUtf8( serviceProvider, out, utf8_out );

			FilePath c_out = Helper::stringizeFilePath( serviceProvider, utf8_out );

			OutputStreamInterfacePtr output_stream = FILE_SERVICE( serviceProvider )
				->openOutputFile( ConstString::none(), c_out );

			if( output_stream == nullptr )
			{
				return false;
			}

			ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE( serviceProvider )
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
		}

		if( info.empty() == false )
		{
			String utf8_info;
			Helper::unicodeToUtf8( serviceProvider, info, utf8_info );

			FilePath c_info = Helper::stringizeFilePath( serviceProvider, utf8_info );

			OutputStreamInterfacePtr info_stream = FILE_SERVICE( serviceProvider )
				->openOutputFile( ConstString::none(), c_info );

			if( info_stream == nullptr )
			{
				return false;
			}

			char info_buffer[1024];

			sprintf( info_buffer, "%u\n%u\n%u\n%u\n%u\n%u\n"
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
		}
		else
		{
			printf( "base_width=%u\n", width );
			printf( "base_height=%u\n", height );
			printf( "trim_width=%u\n", new_width );
			printf( "trim_height=%u\n", new_height );
			printf( "offset_x=%u\n", min_i );
			printf( "offset_y=%u\n", min_j );
		}

		return true;
	}
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, Menge::WString & _value )
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

	Menge::WString in = parse_kwds( lpCmdLine, L"--in", Menge::WString() );
	Menge::WString out = parse_kwds( lpCmdLine, L"--out", Menge::WString() );
	Menge::WString info = parse_kwds( lpCmdLine, L"--info", Menge::WString() );

	if( in.empty() == true )
	{
		message_error("not found 'in' param"
			);

		return 0;
	}

	if( in.front() == L'\"' && in.back() == L'\"' )
	{
		in = in.substr( 1, in.size() - 2 );
	}

	if( out.empty() == false && out.front() == L'\"' && out.back() == L'\"' )
	{
		out = out.substr( 1, out.size() - 2 );
	}

	if( info.empty() == false && info.front() == L'\"' && info.back() == L'\"' )
	{
		info = info.substr( 1, info.size() - 2 );
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

	if( Menge::trimImage( serviceProvider, in, out, info ) == false )
	{
		message_error( "ImageTrimmer invalid trim %ls"
			, in.c_str()
			);

		return 0;
	}	
	
	return 0;
}