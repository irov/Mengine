#	include "AlphaSpreadingPlugin.h"

#   include "Interface/ServiceInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/LogSystemInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/PluginInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ImageCodecInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#   include "Logger\Logger.h"

#   include <Python.h>

extern PyObject * PyToolException;

namespace Menge
{
	extern ServiceProviderInterface * serviceProvider;    

    PyObject * spreadingPngAlpha( PyObject * _self, PyObject * _args )
    {
        LOGGER_INFO(serviceProvider)("spreadingPngAlpha\n");

        const wchar_t * pngPathIn;
        const wchar_t * pngPathOut;

        if( !PyArg_ParseTuple(_args, "uu", &pngPathIn, &pngPathOut) )
        {
            char error[512];
            sprintf( error, "spreadingPngAlpha error parse args"
                );

            PyErr_SetString( PyToolException, error );

            return NULL;
        }

        String utf8_inputFileName;
        Helper::unicodeToUtf8(serviceProvider, pngPathIn, utf8_inputFileName);

        String utf8_outputFileName;
        Helper::unicodeToUtf8(serviceProvider, pngPathOut, utf8_outputFileName);

        ConstString inputFileName = Helper::stringizeString(serviceProvider, utf8_inputFileName);
        ConstString outputFileName = Helper::stringizeString(serviceProvider, utf8_outputFileName);

        InputStreamInterfacePtr input_stream = 
            FILE_SERVICE(serviceProvider)->openInputFile( ConstString::none(), inputFileName, false );
        
        if( input_stream == nullptr )
        {
            char error[512];
            sprintf( error, "spreadingPngAlpha invalid PNG file '%s' not found"
                , inputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return NULL;
        }

        ConstString codec = Helper::stringizeString(serviceProvider, "pngImage");

        ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE(serviceProvider)
            ->createDecoderT<ImageDecoderInterfacePtr>( codec );

        if( imageDecoder == nullptr )
        {
            char error[512];
            sprintf( error, "spreadingPngAlpha not found decoder for file '%s'"
                , inputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return nullptr;
        }

		if( imageDecoder->prepareData( input_stream ) == false )
		{
			char error[512];
			sprintf( error, "spreadingPngAlpha not initialize decoder for file '%s'"
				, inputFileName.c_str()
				);

			PyErr_SetString( PyToolException, error );

			return nullptr;
		}

        const ImageCodecDataInfo* decode_dataInfo = imageDecoder->getCodecDataInfo();

        if( decode_dataInfo->channels != 4 )
        {
            Py_RETURN_NONE;
        }
        
        ImageCodecOptions decode_options;

        decode_options.channels = decode_dataInfo->channels;
        decode_options.pitch = decode_dataInfo->width * 4;

        imageDecoder->setOptions( &decode_options );

        size_t width = decode_dataInfo->width;
        size_t height = decode_dataInfo->height;

        unsigned int bufferSize = width * height * 4;

        unsigned char * textureBuffer = new unsigned char [bufferSize];

        if( imageDecoder->decode( textureBuffer, bufferSize ) == 0 )
        {
            char error[512];
            sprintf( error, "spreadingPngAlpha invalid decode file '%s'"
                , inputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return nullptr;
        }

        for( int i = 0; i != height; ++i)
        {
            for( int j = 0; j != width; ++j )
            {
                size_t index =  j + i * width;
                unsigned char alpha = textureBuffer[ index * 4 + 3 ];

                if( alpha != 0 )
                {
                    continue;
                }

                size_t count = 0;

                float r = 0;
                float g = 0;
                float b = 0;

                int di [] = {-1, 0, 1, 1, 1, 0, -1, -1};
                int dj [] = {1, 1, 1, 0, -1, -1, -1, 0};

                for( int d = 0; d != 8; ++d )
                {
                    int ni = i + di[d];
                    int nj = j + dj[d];

                    if( ni < 0 )
                    {
                        continue;
                    }

                    if( ni >= height )
                    {
                        continue;
                    }

                    if( nj < 0 )
                    {
                        continue;
                    }

                    if( nj >= width )
                    {
                        continue;
                    }

                    size_t index_kl =  nj + ni * width;
                    unsigned char alpha_kl = textureBuffer[ index_kl * 4 + 3 ];
                    
                    if( alpha_kl != 0 )
                    {
                        r += textureBuffer[ index_kl * 4 + 0 ];
                        g += textureBuffer[ index_kl * 4 + 1 ];
                        b += textureBuffer[ index_kl * 4 + 2 ];

                        ++count;
                    }
                }

                if( count != 0 )
                {
                    textureBuffer[index * 4 + 0] = unsigned char(r / count);
                    textureBuffer[index * 4 + 1] = unsigned char(g / count);
                    textureBuffer[index * 4 + 2] = unsigned char(b / count);
                }
            }
        }

        OutputStreamInterfacePtr output_stream = 
            FILE_SERVICE(serviceProvider)->openOutputFile( ConstString::none(), outputFileName );

        if( output_stream == nullptr )
        {
            delete [] textureBuffer;

            char error[512];
            sprintf( error, "spreadingPngAlpha invalid create PNG file '%s'"
                , outputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return nullptr;
        }

        ImageEncoderInterfacePtr imageEncoder = CODEC_SERVICE(serviceProvider)
            ->createEncoderT<ImageEncoderInterfacePtr>( codec );

        if( imageEncoder == nullptr )
        {
            delete [] textureBuffer;

            char error[512];
            sprintf( error, "spreadingPngAlpha not found encoder for file '%s'"
                , outputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return nullptr;
        }

		if( imageEncoder->initialize( output_stream ) == false )
		{
			delete [] textureBuffer;

			char error[512];
			sprintf( error, "spreadingPngAlpha not found encoder for file '%s'"
				, outputFileName.c_str()
				);

			PyErr_SetString( PyToolException, error );

			return nullptr;
		}

        ImageCodecOptions encode_options;		

        encode_options.pitch = width * 4;
        encode_options.channels = 4;

        imageEncoder->setOptions( &encode_options );

        ImageCodecDataInfo encode_dataInfo;
        //dataInfo.format = _image->getHWPixelFormat();
        encode_dataInfo.width = width;
        encode_dataInfo.height = height;
        encode_dataInfo.channels = 4;
        encode_dataInfo.depth = 1;
        encode_dataInfo.mipmaps = 0;
        encode_dataInfo.size = 0;	// we don't need this

        unsigned int bytesWritten = imageEncoder->encode( textureBuffer, &encode_dataInfo );
		
        if( bytesWritten == 0 )
        {
            delete [] textureBuffer;

            char error[512];
            sprintf( error, "spreadingPngAlpha not found encoder for file '%s'"
                , outputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return nullptr;
        }

        delete [] textureBuffer;
                
        Py_RETURN_NONE;
    } 
}
