#include <Windows.h>

#include <shellapi.h>

#include <stdio.h>

#include <string>
#include <vector>

#include "Interface/PluginServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FactorableUnique.h"

#include "ToolUtils/ToolUtils.h"
#include "ToolUtils/ToolLogger.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( Win32FileGroup );
PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider )
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorService );
SERVICE_EXTERN( DocumentService );
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
    //////////////////////////////////////////////////////////////////////////
    static bool initializeEngine()
    {
        ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( AllocatorService, nullptr );
        SERVICE_CREATE( DocumentService, nullptr );

        SERVICE_CREATE( FactoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( OptionsService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( UnicodeSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( StringizeService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ArchiveService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( LoggerService, MENGINE_DOCUMENT_FUNCTION );

        LOGGER_SERVICE()
            ->setVerboseLevel( LM_WARNING );

        LOGGER_SERVICE()
            ->registerLogger( Helper::makeFactorableUnique<ToolLogger>( MENGINE_DOCUMENT_FUNCTION ) );

        SERVICE_CREATE( CodecService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( DataService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ConfigService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( ThreadSystem, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( ThreadService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( MemoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PluginService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( Platform, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( FileService, MENGINE_DOCUMENT_FUNCTION );

        PLUGIN_CREATE( Win32FileGroup, MENGINE_DOCUMENT_FUNCTION );

        PLUGIN_CREATE( Zip, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( LZ4, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( ImageCodec, MENGINE_DOCUMENT_FUNCTION );

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "global" ), nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "global" ), nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool trimImage( const WString & in_path, const WString & out_path, const WString & result_path, bool flag_border, bool flag_trim, bool flag_premultiplied )
    {
        String utf8_in;
        Helper::unicodeToUtf8( in_path, &utf8_in );

        FilePath c_in = Helper::stringizeFilePath( utf8_in );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        InputStreamInterfacePtr input_stream = Helper::openInputStreamFile( fileGroup, c_in, false, false, MENGINE_DOCUMENT_FUNCTION );

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
            ->createDecoderT<ImageDecoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

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

        MemoryBufferInterfacePtr memory_texture = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        if( memory_texture == nullptr )
        {
            return false;
        }

        unsigned char * texture_buffer = memory_texture->newBuffer( bufferSize );

        if( texture_buffer == nullptr )
        {
            return false;
        }

        if( imageDecoder->decode( texture_buffer, bufferSize ) == 0U )
        {
            return false;
        }

        uint32_t new_width;
        uint32_t new_height;

        uint32_t min_i;
        uint32_t min_j;
        uint32_t max_i;
        uint32_t max_j;

        int32_t offset_i;
        int32_t offset_j;

        if( channels == 4 && flag_trim == true )
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
                    unsigned char alpha = texture_buffer[index * 4 + 3];

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

            if( flag_border == true )
            {
                new_width += 2;
                new_height += 2;

                offset_i = min_i - 1;
                offset_j = min_j - 1;
            }
            else
            {
                offset_i = min_i;
                offset_j = min_j;
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

            offset_i = min_i;
            offset_j = min_j;
        }

        if( out_path.empty() == false )
        {
            size_t new_bufferSize = new_width * new_height * channels;

            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

            if( memory == nullptr )
            {
                return false;
            }

            uint8_t * new_textureBuffer = memory->newBuffer( new_bufferSize );

            if( new_textureBuffer == nullptr )
            {
                return false;
            }

            if( channels == 4 )
            {
                if( flag_border == true && flag_trim == true )
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

                    if( flag_premultiplied == true )
                    {
                        for( uint32_t i = 1; i != copy_width_end; ++i )
                        {
                            for( uint32_t j = 1; j != copy_height_end; ++j )
                            {
                                uint32_t new_index = i + j * new_width;
                                uint32_t old_index = (min_i + (i - 1)) + (min_j + (j - 1)) * width;

                                uint32_t r = texture_buffer[old_index * 4 + 0];
                                uint32_t g = texture_buffer[old_index * 4 + 1];
                                uint32_t b = texture_buffer[old_index * 4 + 2];
                                uint32_t a = texture_buffer[old_index * 4 + 3];

                                new_textureBuffer[new_index * 4 + 0] = (uint8_t)(r * a / 255);
                                new_textureBuffer[new_index * 4 + 1] = (uint8_t)(g * a / 255);
                                new_textureBuffer[new_index * 4 + 2] = (uint8_t)(b * a / 255);
                                new_textureBuffer[new_index * 4 + 3] = (uint8_t)a;
                            }
                        }
                    }
                    else
                    {
                        for( uint32_t i = 1; i != copy_width_end; ++i )
                        {
                            for( uint32_t j = 1; j != copy_height_end; ++j )
                            {
                                uint32_t new_index = i + j * new_width;
                                uint32_t old_index = (min_i + (i - 1)) + (min_j + (j - 1)) * width;

                                new_textureBuffer[new_index * 4 + 0] = texture_buffer[old_index * 4 + 0];
                                new_textureBuffer[new_index * 4 + 1] = texture_buffer[old_index * 4 + 1];
                                new_textureBuffer[new_index * 4 + 2] = texture_buffer[old_index * 4 + 2];
                                new_textureBuffer[new_index * 4 + 3] = texture_buffer[old_index * 4 + 3];
                            }
                        }
                    }
                }
                else
                {
                    if( flag_premultiplied == true )
                    {
                        for( uint32_t i = 0; i != new_width; ++i )
                        {
                            for( uint32_t j = 0; j != new_height; ++j )
                            {
                                uint32_t new_index = i + j * new_width;
                                uint32_t old_index = (min_i + i) + (min_j + j) * width;

                                uint32_t r = texture_buffer[old_index * 4 + 0];
                                uint32_t g = texture_buffer[old_index * 4 + 1];
                                uint32_t b = texture_buffer[old_index * 4 + 2];
                                uint32_t a = texture_buffer[old_index * 4 + 3];

                                new_textureBuffer[new_index * 4 + 0] = (uint8_t)(r * a / 255);
                                new_textureBuffer[new_index * 4 + 1] = (uint8_t)(g * a / 255);
                                new_textureBuffer[new_index * 4 + 2] = (uint8_t)(b * a / 255);
                                new_textureBuffer[new_index * 4 + 3] = (uint8_t)a;
                            }
                        }
                    }
                    else
                    {
                        for( uint32_t i = 0; i != new_width; ++i )
                        {
                            for( uint32_t j = 0; j != new_height; ++j )
                            {
                                uint32_t new_index = i + j * new_width;
                                uint32_t old_index = (min_i + i) + (min_j + j) * width;

                                new_textureBuffer[new_index * 4 + 0] = texture_buffer[old_index * 4 + 0];
                                new_textureBuffer[new_index * 4 + 1] = texture_buffer[old_index * 4 + 1];
                                new_textureBuffer[new_index * 4 + 2] = texture_buffer[old_index * 4 + 2];
                                new_textureBuffer[new_index * 4 + 3] = texture_buffer[old_index * 4 + 3];
                            }
                        }
                    }
                }

                if( flag_premultiplied == false )
                {
                    for( uint32_t i = 0; i != new_width; ++i )
                    {
                        for( uint32_t j = 0; j != new_height; ++j )
                        {
                            uint32_t new_index = i + j * new_width;

                            uint8_t alpha = new_textureBuffer[new_index * 4 + 3];

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

                                uint8_t test_alpha = new_textureBuffer[test_index * 4 + 3];

                                if( test_alpha != 0 )
                                {
                                    ++colorNum;
                                    r += new_textureBuffer[test_index * 4 + 0];
                                    g += new_textureBuffer[test_index * 4 + 1];
                                    b += new_textureBuffer[test_index * 4 + 2];
                                }
                            }

                            if( i < new_width - 1 ) // right pixel
                            {
                                uint32_t test_index = (i + 1) + (j + 0) * new_width;

                                uint8_t test_alpha = new_textureBuffer[test_index * 4 + 3];

                                if( test_alpha != 0 )
                                {
                                    ++colorNum;
                                    r += new_textureBuffer[test_index * 4 + 0];
                                    g += new_textureBuffer[test_index * 4 + 1];
                                    b += new_textureBuffer[test_index * 4 + 2];
                                }
                            }

                            if( j > 0 ) // top pixel
                            {
                                uint32_t test_index = (i + 0) + (j - 1) * new_width;

                                uint8_t test_alpha = new_textureBuffer[test_index * 4 + 3];

                                if( test_alpha != 0 )
                                {
                                    ++colorNum;
                                    r += new_textureBuffer[test_index * 4 + 0];
                                    g += new_textureBuffer[test_index * 4 + 1];
                                    b += new_textureBuffer[test_index * 4 + 2];
                                }
                            }

                            if( j < new_height - 1 ) // bottom pixel
                            {
                                uint32_t test_index = (i + 0) + (j + 1) * new_width;

                                uint8_t test_alpha = new_textureBuffer[test_index * 4 + 3];

                                if( test_alpha != 0 )
                                {
                                    ++colorNum;
                                    r += new_textureBuffer[test_index * 4 + 0];
                                    g += new_textureBuffer[test_index * 4 + 1];
                                    b += new_textureBuffer[test_index * 4 + 2];
                                }
                            }

                            if( colorNum > 0 )
                            {
                                r /= colorNum;
                                g /= colorNum;
                                b /= colorNum;

                                new_textureBuffer[new_index * 4 + 0] = (uint8_t)r;
                                new_textureBuffer[new_index * 4 + 1] = (uint8_t)g;
                                new_textureBuffer[new_index * 4 + 2] = (uint8_t)b;
                            }
                            else
                            {
                                new_textureBuffer[new_index * 4 + 0] = 0;
                                new_textureBuffer[new_index * 4 + 1] = 0;
                                new_textureBuffer[new_index * 4 + 2] = 0;
                            }
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
                            new_textureBuffer[index * channels + k] = texture_buffer[index * channels + k];
                        }
                    }
                }
            }

            String utf8_out;
            Helper::unicodeToUtf8( out_path, &utf8_out );

            FilePath c_out = Helper::stringizeFilePath( utf8_out );
            
            OutputStreamInterfacePtr output_stream = Helper::openOutputStreamFile( fileGroup, c_out, MENGINE_DOCUMENT_FUNCTION );

            if( output_stream == nullptr )
            {
                return false;
            }

            ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE()
                ->createEncoderT<ImageEncoderInterfacePtr>( codecType, MENGINE_DOCUMENT_FUNCTION );

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

            fprintf_s( f_result, "%u\n", width );
            fprintf_s( f_result, "%u\n", height );
            fprintf_s( f_result, "%u\n", new_width );
            fprintf_s( f_result, "%u\n", new_height );
            fprintf_s( f_result, "%d\n", offset_i );
            fprintf_s( f_result, "%d\n", offset_j );

            fclose( f_result );
        }
        else
        {
            printf( "%u\n", width );
            printf( "%u\n", height );
            printf( "%u\n", new_width );
            printf( "%u\n", new_height );
            printf( "%d\n", offset_i );
            printf( "%d\n", offset_j );
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
        
    {
        Mengine::WString in_path = parse_kwds( lpCmdLine, L"--in_path", Mengine::WString() );
        Mengine::WString out_path = parse_kwds( lpCmdLine, L"--out_path", Mengine::WString() );
        Mengine::WString result_path = parse_kwds( lpCmdLine, L"--result_path", Mengine::WString() );
        Mengine::WString border = parse_kwds( lpCmdLine, L"--border", Mengine::WString() );
        Mengine::WString trim = parse_kwds( lpCmdLine, L"--trim", Mengine::WString() );
        Mengine::WString premultiplied = parse_kwds( lpCmdLine, L"--premultiplied", Mengine::WString() );

        if( in_path.empty() == true )
        {
            message_error( "not found 'in' param"
            );

            return EXIT_FAILURE;
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

        bool flag_border = false;
        if( border == L"1" )
        {
            flag_border = true;
        }

        bool flag_trim = false;
        if( trim == L"1" )
        {
            flag_trim = true;
        }

        bool flag_premultiplied = false;
        if( premultiplied == L"1" )
        {
            flag_premultiplied = true;
        }

        try
        {
            if( Mengine::initializeEngine() == false )
            {
                message_error( "ImageTrimmer invalid initialize" );

                return EXIT_FAILURE;
            }
        }
        catch( const std::exception & se )
        {
            message_error( "Mengine exception %s"
                , se.what()
            );

            return EXIT_FAILURE;
        }

        if( Mengine::trimImage( in_path, out_path, result_path, flag_border, flag_trim, flag_premultiplied ) == false )
        {
            message_error( "ImageTrimmer invalid trim %ls"
                , in_path.c_str()
            );

            return EXIT_FAILURE;
        }
    }

    stdex_allocator_finalize();

    return EXIT_SUCCESS;
}