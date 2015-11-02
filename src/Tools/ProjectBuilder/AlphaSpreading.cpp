#	include "AlphaSpreading.h"

#   include "Interface/ServiceInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/LoggerInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/PluginInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ImageCodecInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#   include "Logger\Logger.h"

namespace Menge
{
	extern ServiceProviderInterface * serviceProvider;    

    PyObject * spreadingPngAlpha( const wchar_t * pngPathIn, const wchar_t * pngPathOut )
    {
        LOGGER_INFO(serviceProvider)("spreadingPngAlpha\n");

        String utf8_inputFileName;
        Helper::unicodeToUtf8(serviceProvider, pngPathIn, utf8_inputFileName);

        String utf8_outputFileName;
        Helper::unicodeToUtf8(serviceProvider, pngPathOut, utf8_outputFileName);

        ConstString inputFileName = Helper::stringizeString(serviceProvider, utf8_inputFileName);
        ConstString outputFileName = Helper::stringizeString(serviceProvider, utf8_outputFileName);

        InputStreamInterfacePtr input_stream = FILE_SERVICE(serviceProvider)
			->openInputFile( ConstString::none(), inputFileName, false );
        
        if( input_stream == nullptr )
        {
            LOGGER_ERROR(serviceProvider)("spreadingPngAlpha invalid PNG file '%s' not found"
                , inputFileName.c_str()
                );

            return NULL;
        }

        ConstString codec = Helper::stringizeString(serviceProvider, "pngImage");

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(serviceProvider)
            ->createDecoderT<ImageDecoderInterfacePtr>( codec );

        if( imageDecoder == nullptr )
        {
            LOGGER_ERROR(serviceProvider)("spreadingPngAlpha not found decoder for file '%s'"
                , inputFileName.c_str()
                );

            return nullptr;
        }

		if( imageDecoder->prepareData( input_stream ) == false )
		{
			LOGGER_ERROR(serviceProvider)("spreadingPngAlpha not initialize decoder for file '%s'"
				, inputFileName.c_str()
				);

			return nullptr;
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

		MemoryInterfacePtr memory_textureBuffer = MEMORY_SERVICE( serviceProvider )
			->createMemory();
		
		if( memory_textureBuffer == nullptr )
		{
			return nullptr;
		}

		unsigned char * textureBuffer = memory_textureBuffer->newMemory( bufferSize );

		if( textureBuffer == nullptr )
		{
			return nullptr;
		}

		if( imageDecoder->decode( textureBuffer, bufferSize ) == 0 )
		{
			LOGGER_ERROR( serviceProvider )("spreadingPngAlpha invalid decode file '%s'"
				, inputFileName.c_str()
				);

			return nullptr;
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

					int di[] = {-1, 0, 1, 1, 1, 0, -1, -1};
					int dj[] = {1, 1, 1, 0, -1, -1, -1, 0};

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
						textureBuffer[index * 4 + 0] = unsigned char( r / count );
						textureBuffer[index * 4 + 1] = unsigned char( g / count );
						textureBuffer[index * 4 + 2] = unsigned char( b / count );
					}
				}
			}
		}

        OutputStreamInterfacePtr output_stream = FILE_SERVICE(serviceProvider)
			->openOutputFile( ConstString::none(), outputFileName );

        if( output_stream == nullptr )
        {
            LOGGER_ERROR(serviceProvider)("spreadingPngAlpha invalid create PNG file '%s'"
                , outputFileName.c_str()
                );

            return nullptr;
        }

        ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE(serviceProvider)
            ->createEncoderT<ImageEncoderInterfacePtr>( codec );

        if( imageEncoder == nullptr )
        {
            LOGGER_ERROR(serviceProvider)("spreadingPngAlpha not found encoder for file '%s'"
                , outputFileName.c_str()
                );

            return nullptr;
        }

		if( imageEncoder->initialize( output_stream ) == false )
		{
			LOGGER_ERROR(serviceProvider)("spreadingPngAlpha not found encoder for file '%s'"
				, outputFileName.c_str()
				);

			return nullptr;
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

		unsigned int bytesWritten = imageEncoder->encode( textureBuffer, bufferSize, &encode_dataInfo );
		
        if( bytesWritten == 0 )
        {
            LOGGER_ERROR(serviceProvider)("spreadingPngAlpha not found encoder for file '%s'"
                , outputFileName.c_str()
                );

			return nullptr;
        }
               
        return pybind::ret_none();
    } 
}
