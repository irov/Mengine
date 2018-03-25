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
PLUGIN_EXPORT( Win32FileGroup );
PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider )

SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( Platform );
SERVICE_EXTERN( WindowsLayer );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( PluginSystem );
SERVICE_EXTERN( PluginService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    static bool initializeEngine()
    {
        ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( FactoryService );

        SERVICE_CREATE( OptionsService );

        SERVICE_CREATE( UnicodeSystem );

        SERVICE_CREATE( StringizeService );
        SERVICE_CREATE( ArchiveService );

        SERVICE_CREATE( LoggerService );

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

            MyLogger()
                : m_verboseLevel( LM_WARNING )
                , m_verboseFlag( 0xFFFFFFFF )
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

                message_error( "%s"
                    , _data
                );
            }

            void flush() override
            {
            }

        protected:
            EMessageLevel m_verboseLevel;
            uint32_t m_verboseFlag;
        };

        LOGGER_SERVICE()
            ->setVerboseLevel( LM_WARNING );

        LOGGER_SERVICE()
            ->registerLogger( new MyLogger );

        SERVICE_CREATE( CodecService );
        SERVICE_CREATE( DataService );
        SERVICE_CREATE( ConfigService );

        SERVICE_CREATE( ThreadSystem );

        SERVICE_CREATE( ThreadService );
        SERVICE_CREATE( MemoryService );
        SERVICE_CREATE( PluginSystem );
        SERVICE_CREATE( PluginService );

        SERVICE_CREATE( WindowsLayer );
        SERVICE_CREATE( Platform );
        SERVICE_CREATE( FileService );

        PLUGIN_CREATE( Win32FileGroup );

        PLUGIN_CREATE( Zip );
        PLUGIN_CREATE( LZ4 );
        PLUGIN_CREATE( ImageCodec );

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), ConstString::none(), Helper::emptyPath(), Helper::stringizeString( "global" ) ) == false )
        {
            return false;
        }

        ConstString dev = Helper::stringizeString( "dev" );

        if( FILE_SERVICE()
            ->mountFileGroup( dev, ConstString::none(), Helper::emptyPath(), Helper::stringizeString( "global" ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool trimImage( const WString & in_path, const WString & out_path, const WString & result_path )
    {
        String utf8_in;
        Helper::unicodeToUtf8( in_path, utf8_in );

        FilePath c_in = Helper::stringizeFilePath( utf8_in );

        InputStreamInterfacePtr input_stream = FILE_SERVICE()
            ->openInputFile( ConstString::none(), c_in, false );

        if( input_stream == nullptr )
        {
            return false;
        }

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( c_in );

        if( codecType.empty() == true )
        {
            return false;
        }

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
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

        uint32_t width = decode_dataInfo->width;
        uint32_t height = decode_dataInfo->height;
        uint32_t channels = decode_dataInfo->channels;

        size_t bufferSize = width * height * channels;

        MemoryBufferInterfacePtr memory_textureBuffer = MEMORY_SERVICE()
            ->createMemoryBuffer();

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

        uint32_t new_width;
        uint32_t new_height;

        uint32_t min_i;
        uint32_t min_j;
        uint32_t max_i;
        uint32_t max_j;

        uint32_t offset_i;
        uint32_t offset_j;

        if( channels == 4 )
        {
            min_i = width;
            min_j = height;
            max_i = 0;
            max_j = 0;

            for( uint32_t i = 0; i != width; ++i )
            {
                for( uint32_t j = 0; j != height; ++j )
                {
                    uint32_t index = i + j * width;
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
                new_width = 0;
                new_height = 0;

                min_i = 0;
                min_j = 0;
                max_i = 0;
                max_j = 0;
            }
            else
            {
                new_width = max_i - min_i + 1;
                new_height = max_j - min_j + 1;
            }

            new_width += 2;
            new_height += 2;

            offset_i = min_i + 1;
            offset_j = min_j + 1;
        }
        else
        {
            new_width = width;
            new_height = height;

            min_i = 0;
            min_j = 0;
            max_i = width;
            max_j = height;

            offset_i = min_i + 1;
            offset_j = min_j + 1;
        }

        if( out_path.empty() == false )
        {
            size_t new_bufferSize = new_width * new_height * channels;

            MemoryBufferInterfacePtr buffer = MEMORY_SERVICE()
                ->createMemoryBuffer();

            if( buffer == nullptr )
            {
                return false;
            }

            uint8_t * new_textureBuffer = buffer->newMemory( new_bufferSize, __FILE__, __LINE__ );

            if( new_textureBuffer == nullptr )
            {
                return false;
            }

            if( channels == 4 )
            {
                uint32_t copy_width_end = new_width - 1;
                uint32_t copy_height_end = new_height - 1;

                for( uint32_t i = 0; i != new_width; ++i )
                {
                    for( uint32_t j = 0; j != new_height; ++j )
                    {
                        uint32_t new_index = i + j * new_width;

                        if( (i == 0 || j == 0 || i == copy_width_end || j == copy_height_end) )
                        {
                            for( uint32_t k = 0; k != channels; ++k )
                            {
                                new_textureBuffer[new_index * channels + k] = 0;
                            }
                        }
                    }
                }

                for( uint32_t i = 1; i != copy_width_end; ++i )
                {
                    for( uint32_t j = 1; j != copy_height_end; ++j )
                    {
                        uint32_t new_index = i + j * new_width;
                        uint32_t old_index = (min_i + (i - 1)) + (min_j + (j - 1)) * width;

                        for( uint32_t k = 0; k != channels; ++k )
                        {
                            new_textureBuffer[new_index * channels + k] = textureBuffer[old_index * channels + k];
                        }
                    }
                }

                for( uint32_t i = 0; i != new_width; ++i )
                {
                    for( uint32_t j = 0; j != new_height; ++j )
                    {
                        uint32_t new_index = i + j * new_width;

                        uint8_t alpha = new_textureBuffer[new_index * channels + 3];

                        if( alpha != 0 )
                        {
                            continue;
                        }

                        uint32_t r = 0;
                        uint32_t g = 0;
                        uint32_t b = 0;
                        uint32_t colorNum = 0;

                        if( i > 0 ) // left pixel
                        {
                            uint32_t test_index = (i - 1) + (j + 0) * new_width;

                            uint8_t test_alpha = new_textureBuffer[test_index * channels + 3];

                            if( test_alpha != 0 )
                            {
                                ++colorNum;
                                r += new_textureBuffer[test_index * channels + 0];
                                g += new_textureBuffer[test_index * channels + 1];
                                b += new_textureBuffer[test_index * channels + 2];
                            }
                        }

                        if( i < new_width - 1 ) // right pixel
                        {
                            uint32_t test_index = (i + 1) + (j + 0) * new_width;

                            uint8_t test_alpha = new_textureBuffer[test_index * channels + 3];

                            if( test_alpha != 0 )
                            {
                                ++colorNum;
                                r += new_textureBuffer[test_index * channels + 0];
                                g += new_textureBuffer[test_index * channels + 1];
                                b += new_textureBuffer[test_index * channels + 2];
                            }
                        }

                        if( j > 0 ) // top pixel
                        {
                            uint32_t test_index = (i + 0) + (j - 1) * new_width;

                            uint8_t test_alpha = new_textureBuffer[test_index * channels + 3];

                            if( test_alpha != 0 )
                            {
                                ++colorNum;
                                r += new_textureBuffer[test_index * channels + 0];
                                g += new_textureBuffer[test_index * channels + 1];
                                b += new_textureBuffer[test_index * channels + 2];
                            }
                        }

                        if( j < new_height - 1 ) // bottom pixel
                        {
                            uint32_t test_index = (i + 0) + (j + 1) * new_width;

                            uint8_t test_alpha = new_textureBuffer[test_index * channels + 3];

                            if( test_alpha != 0 )
                            {
                                ++colorNum;
                                r += new_textureBuffer[test_index * channels + 0];
                                g += new_textureBuffer[test_index * channels + 1];
                                b += new_textureBuffer[test_index * channels + 2];
                            }
                        }

                        if( colorNum > 0 )
                        {
                            r /= colorNum;
                            g /= colorNum;
                            b /= colorNum;

                            new_textureBuffer[new_index * channels + 0] = (uint8_t)r;
                            new_textureBuffer[new_index * channels + 1] = (uint8_t)g;
                            new_textureBuffer[new_index * channels + 2] = (uint8_t)b;
                        }
                    }
                }
            }
            else
            {
                for( uint32_t i = 0; i != new_width; ++i )
                {
                    for( uint32_t j = 0; j != new_height; ++j )
                    {
                        uint32_t index = i + j * new_width;

                        for( uint32_t k = 0; k != channels; ++k )
                        {
                            new_textureBuffer[index * channels + k] = textureBuffer[index * channels + k];
                        }
                    }
                }
            }

            String utf8_out;
            Helper::unicodeToUtf8( out_path, utf8_out );

            FilePath c_out = Helper::stringizeFilePath( utf8_out );

            OutputStreamInterfacePtr output_stream = FILE_SERVICE()
                ->openOutputFile( ConstString::none(), c_out );

            if( output_stream == nullptr )
            {
                return false;
            }

            ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE()
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

        if( result_path.empty() == false )
        {
            WCHAR infoCanonicalizeQuote[MAX_PATH];
            ForcePathQuoteSpaces( infoCanonicalizeQuote, result_path.c_str() );
            PathUnquoteSpaces( infoCanonicalizeQuote );

            FILE * f_result;
            errno_t err = _wfopen_s( &f_result, infoCanonicalizeQuote, L"wt" );

            if( err != 0 )
            {
                message_error( "invalid _wfopen %ls err %d\n"
                    , infoCanonicalizeQuote
                    , err
                );

                return 0;
            }

            fprintf_s( f_result, "base_width=%u\n", width );
            fprintf_s( f_result, "base_height=%u\n", height );
            fprintf_s( f_result, "trim_width=%u\n", new_width );
            fprintf_s( f_result, "trim_height=%u\n", new_height );
            fprintf_s( f_result, "offset_x=%u\n", offset_i );
            fprintf_s( f_result, "offset_y=%u\n", offset_j );
        }
        else
        {
            printf( "base_width=%u\n", width );
            printf( "base_height=%u\n", height );
            printf( "trim_width=%u\n", new_width );
            printf( "trim_height=%u\n", new_height );
            printf( "offset_x=%u\n", offset_i );
            printf( "offset_y=%u\n", offset_j );
        }

        return true;
    }
}
//////////////////////////////////////////////////////////////////////////
static void parse_arg( const std::wstring & _str, Mengine::WString & _value )
{
    _value = Mengine::WString( _str.begin(), _str.end() );
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)nShowCmd;

    stdex_allocator_initialize();

    Mengine::WString in_path = parse_kwds( lpCmdLine, L"--in_path", Mengine::WString() );
    Mengine::WString out_path = parse_kwds( lpCmdLine, L"--out_path", Mengine::WString() );
    Mengine::WString result_path = parse_kwds( lpCmdLine, L"--result_path", Mengine::WString() );

    if( in_path.empty() == true )
    {
        message_error( "not found 'in' param"
        );

        return 0;
    }

    if( in_path.front() == L'\"' && in_path.back() == L'\"' )
    {
        in_path = in_path.substr( 1, in_path.size() - 2 );
    }

    if( out_path.empty() == false && out_path.front() == L'\"' && out_path.back() == L'\"' )
    {
        out_path = out_path.substr( 1, out_path.size() - 2 );
    }

    if( result_path.empty() == false && result_path.front() == L'\"' && result_path.back() == L'\"' )
    {
        result_path = result_path.substr( 1, result_path.size() - 2 );
    }

    try
    {
        if( Mengine::initializeEngine() == false )
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

    if( Mengine::trimImage( in_path, out_path, result_path ) == false )
    {
        message_error( "ImageTrimmer invalid trim %ls"
            , in_path.c_str()
        );

        return 0;
    }

    return 0;
}