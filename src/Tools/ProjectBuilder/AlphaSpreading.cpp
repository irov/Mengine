#include "AlphaSpreading.h"

#include "Interface/ServiceInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

namespace Mengine
{
    bool spreadingPngAlpha( pybind::kernel_interface * _kernel, const wchar_t * pngPathIn, const wchar_t * pngPathOut )
    {
        (void)_kernel;

        LOGGER_INFO( "spreadingPngAlpha\n" );

        String utf8_inputFileName;
        Helper::unicodeToUtf8( pngPathIn, utf8_inputFileName );

        String utf8_outputFileName;
        Helper::unicodeToUtf8( pngPathOut, utf8_outputFileName );

        FilePath inputFileName = Helper::stringizeFilePath( utf8_inputFileName );
        FilePath outputFileName = Helper::stringizeFilePath( utf8_outputFileName );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        InputStreamInterfacePtr input_stream = FILE_SERVICE()
            ->openInputFile( fileGroup, inputFileName, false, MENGINE_DOCUMENT_FUNCTION );

        if( input_stream == nullptr )
        {
            LOGGER_ERROR( "invalid PNG file '%s' not found"
                , inputFileName.c_str()
            );

            return false;
        }

        ConstString codec = Helper::stringizeString( "pngImage" );

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE()
            ->createDecoderT<ImageDecoderInterfacePtr>( codec, MENGINE_DOCUMENT_FUNCTION );

        if( imageDecoder == nullptr )
        {
            LOGGER_ERROR( "not found decoder for file '%s'"
                , inputFileName.c_str()
            );

            return false;
        }

        if( imageDecoder->prepareData( input_stream ) == false )
        {
            LOGGER_ERROR( "not initialize decoder for file '%s'"
                , inputFileName.c_str()
            );

            return false;
        }

        const ImageCodecDataInfo* decode_dataInfo = imageDecoder->getCodecDataInfo();

        ImageCodecOptions decode_options;

        decode_options.channels = decode_dataInfo->channels;
        decode_options.pitch = decode_dataInfo->width * decode_dataInfo->channels;

        imageDecoder->setOptions( &decode_options );

        uint32_t width = decode_dataInfo->width;
        uint32_t height = decode_dataInfo->height;
        uint32_t channels = decode_dataInfo->channels;

        uint32_t bufferSize = width * height * channels;

        MemoryBufferInterfacePtr memory_textureBuffer = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        if( memory_textureBuffer == nullptr )
        {
            return false;
        }

        unsigned char * textureBuffer = memory_textureBuffer->newBuffer( bufferSize, "spreadingPngAlpha", __FILE__, __LINE__ );

        if( textureBuffer == nullptr )
        {
            return false;
        }

        if( imageDecoder->decode( textureBuffer, bufferSize ) == 0 )
        {
            LOGGER_ERROR( "invalid decode file '%s'"
                , inputFileName.c_str()
            );

            return false;
        }

        if( channels == 4 )
        {
            for( uint32_t i = 0; i != height; ++i )
            {
                for( uint32_t j = 0; j != width; ++j )
                {
                    uint32_t index = j + i * width;
                    unsigned char alpha = textureBuffer[index * 4 + 3];

                    if( alpha != 0 )
                    {
                        continue;
                    }

                    uint32_t count = 0;

                    float r = 0;
                    float g = 0;
                    float b = 0;

                    int di[] = { -1, 0, 1, 1, 1, 0, -1, -1 };
                    int dj[] = { 1, 1, 1, 0, -1, -1, -1, 0 };

                    for( uint32_t d = 0; d != 8; ++d )
                    {
                        uint32_t ni = i + di[d];
                        uint32_t nj = j + dj[d];

                        if( ni >= height )
                        {
                            continue;
                        }

                        if( nj >= width )
                        {
                            continue;
                        }

                        uint32_t index_kl = nj + ni * width;
                        unsigned char alpha_kl = textureBuffer[index_kl * 4 + 3];

                        if( alpha_kl != 0 )
                        {
                            r += textureBuffer[index_kl * 4 + 0];
                            g += textureBuffer[index_kl * 4 + 1];
                            b += textureBuffer[index_kl * 4 + 2];

                            ++count;
                        }
                    }

                    if( count != 0 )
                    {
                        textureBuffer[index * 4 + 0] = (unsigned char)(r / count);
                        textureBuffer[index * 4 + 1] = (unsigned char)(g / count);
                        textureBuffer[index * 4 + 2] = (unsigned char)(b / count);
                    }
                }
            }
        }

        OutputStreamInterfacePtr output_stream = FILE_SERVICE()
            ->openOutputFile( fileGroup, outputFileName, MENGINE_DOCUMENT_FUNCTION );

        if( output_stream == nullptr )
        {
            LOGGER_ERROR( "invalid create PNG file '%s'"
                , outputFileName.c_str()
            );

            return false;
        }

        ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE()
            ->createEncoderT<ImageEncoderInterfacePtr>( codec, MENGINE_DOCUMENT_FUNCTION );

        if( imageEncoder == nullptr )
        {
            LOGGER_ERROR( "spreadingPngAlpha not found encoder for file '%s'"
                , outputFileName.c_str()
            );

            return false;
        }

        if( imageEncoder->initialize( output_stream ) == false )
        {
            LOGGER_ERROR( "not found encoder for file '%s'"
                , outputFileName.c_str()
            );

            return false;
        }

        ImageCodecOptions encode_options;

        encode_options.pitch = width * channels;
        encode_options.channels = channels;

        imageEncoder->setOptions( &encode_options );

        ImageCodecDataInfo encode_dataInfo;

        encode_dataInfo.width = width;
        encode_dataInfo.height = height;
        encode_dataInfo.channels = channels;
        encode_dataInfo.depth = 1;
        encode_dataInfo.mipmaps = 1;

        size_t bytesWritten = imageEncoder->encode( textureBuffer, bufferSize, &encode_dataInfo );

        if( bytesWritten == 0 )
        {
            LOGGER_ERROR( "not found encoder for file '%s'"
                , outputFileName.c_str()
            );

            return false;
        }

        return true;
    }
}
