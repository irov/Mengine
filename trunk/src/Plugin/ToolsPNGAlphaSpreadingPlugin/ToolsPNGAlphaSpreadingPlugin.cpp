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

SERVICE_EXTERN(ServiceProvider, Menge::ServiceProviderInterface);

SERVICE_EXTERN(ArchiveService, Menge::ArchiveServiceInterface);

SERVICE_EXTERN(UnicodeSystem, Menge::UnicodeSystemInterface);
SERVICE_EXTERN(UnicodeService, Menge::UnicodeServiceInterface);

SERVICE_EXTERN(StringizeService, Menge::StringizeServiceInterface);
SERVICE_EXTERN(LogService, Menge::LogServiceInterface);
SERVICE_EXTERN(CodecService, Menge::CodecServiceInterface);
SERVICE_EXTERN(ConverterService, Menge::ConverterServiceInterface);
SERVICE_EXTERN(PluginService, Menge::PluginServiceInterface);
 
SERVICE_EXTERN(FileSystem, Menge::FileSystemInterface);
SERVICE_EXTERN(FileService, Menge::FileServiceInterface);

//////////////////////////////////////////////////////////////////////////
extern "C" // only required if using g++
{
    extern bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin );
}

PyObject * PyToolException;

namespace Menge
{
    ServiceProviderInterface * serviceProvider = NULL;

    static bool initialize()
    {
        if( createServiceProvider( &serviceProvider ) == false )
        {
            return false;
        }

        UnicodeSystemInterface * unicodeSystem;
        if( createUnicodeSystem( &unicodeSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(serviceProvider, unicodeSystem) == false )
        {
            return false;
        }

        UnicodeServiceInterface * unicodeService;
        if( createUnicodeService( &unicodeService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(serviceProvider, unicodeService) == false )
        {
            return false;
        }

        StringizeServiceInterface * stringizeService;
        if( createStringizeService( &stringizeService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(serviceProvider, stringizeService) == false )
        {
            return false;
        }

        ArchiveServiceInterface * archiveService;
        if( createArchiveService( &archiveService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(serviceProvider, archiveService) == false )
        {
            return false;
        }

        LogServiceInterface * logService;
        if( createLogService( &logService ) == false )
        {
            return false;
        }

        class MyLoggerInterface
            : public LoggerInterface
        {
        public:
            void setVerboseLevel( EMessageLevel _level ) override {};
            bool validVerboseLevel( EMessageLevel _level ) const override { return true; };

        public:
            void log( const char * _data, int _count, EMessageLevel _level ) override
            {
                printf("%s"
                    , _data
                    );
            }

            void flush() override {};
        };

        logService->setVerboseLevel( LM_WARNING );
        logService->registerLogger( new MyLoggerInterface );

        if( SERVICE_REGISTRY(serviceProvider, logService) == false )
        {
            return false;
        }

        ConverterServiceInterface * converterService;
        if( createConverterService( &converterService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(serviceProvider, converterService) == false )
        {
            return false;
        }

        CodecServiceInterface * codecService;
        if( createCodecService( &codecService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(serviceProvider, codecService) == false )
        {
            return false;
        }

        PluginServiceInterface * pluginService;
        if( createPluginService( &pluginService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(serviceProvider, pluginService) == false )
        {
            return false;
        }

        WindowsLayerInterface * windowsLayer = new VistaWindowsLayer();

        if( SERVICE_REGISTRY(serviceProvider, windowsLayer) == false )
        {
            return false;
        }

        FileSystemInterface * fileSystem;
        if( createFileSystem( &fileSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(serviceProvider, fileSystem) == false )
        {
            return false;
        }

        FileServiceInterface * fileService;
        if( createFileService( &fileService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(serviceProvider, fileService) == false )
        {
            return false;
        }

        fileService->initialize();

        PluginInterface * plugin_image_codec;
        initPluginMengeImageCodec( &plugin_image_codec );

        if( plugin_image_codec == NULL )
        {
            return false;
        }

        plugin_image_codec->initialize( serviceProvider );
        
        if( FILE_SERVICE(serviceProvider)
            ->mountFileSystem( ConstString::none(), ConstString::none(), Helper::stringizeString(serviceProvider, "dir"), false) == false )
        {
            return false;
        }

        return true;
    }

    static PyObject * spreadingPngAlpha( PyObject * _self, PyObject * _args )
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

        InputStreamInterface * input_stream = 
            FILE_SERVICE(serviceProvider)->openInputFile( ConstString::none(), inputFileName );
        
        if( input_stream == 0 )
        {
            char error[512];
            sprintf( error, "spreadingPngAlpha invalid PNG file '%s' not found"
                , inputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return NULL;
        }

        ConstString codec = Helper::stringizeString(serviceProvider, "pngImage");

        ImageDecoderInterface * imageDecoder = 
            CODEC_SERVICE(serviceProvider)->createDecoderT<ImageDecoderInterface>( codec, input_stream );

        if( imageDecoder == 0 )
        {
            input_stream->destroy();

            char error[512];
            sprintf( error, "spreadingPngAlpha not found decoder for file '%s'"
                , inputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return NULL;
        }

        const ImageCodecDataInfo* decode_dataInfo = imageDecoder->getCodecDataInfo();

        if( decode_dataInfo->channels != 4 )
        {
            imageDecoder->destroy();
            input_stream->destroy();

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
            imageDecoder->destroy();
            input_stream->destroy();

            char error[512];
            sprintf( error, "spreadingPngAlpha invalid decode file '%s'"
                , inputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return NULL;
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

        OutputStreamInterface * output_stream = 
            FILE_SERVICE(serviceProvider)->openOutputFile( ConstString::none(), outputFileName );

        if( output_stream == 0 )
        {
            delete [] textureBuffer;

            char error[512];
            sprintf( error, "spreadingPngAlpha invalid create PNG file '%s'"
                , outputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return NULL;
        }

        ImageEncoderInterface * imageEncoder = 
            CODEC_SERVICE(serviceProvider)->createEncoderT<ImageEncoderInterface>( codec, output_stream );

        if( imageEncoder == 0 )
        {
            delete [] textureBuffer;
            output_stream->destroy();

            char error[512];
            sprintf( error, "spreadingPngAlpha not found encoder for file '%s'"
                , outputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return NULL;
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
        encode_dataInfo.num_mipmaps = 0;
        encode_dataInfo.flags = 0;
        encode_dataInfo.size = 0;	// we don't need this

        unsigned int bytesWritten = imageEncoder->encode( textureBuffer, &encode_dataInfo );


        if( bytesWritten == 0 )
        {
            delete [] textureBuffer;
            imageEncoder->destroy();

            char error[512];
            sprintf( error, "spreadingPngAlpha not found encoder for file '%s'"
                , outputFileName.c_str()
                );

            PyErr_SetString( PyToolException, error );

            return NULL;
        }

        delete [] textureBuffer;
                
        input_stream->destroy();
        imageDecoder->destroy(); 

        output_stream->destroy();
        imageEncoder->destroy();

        Py_RETURN_NONE;
    } 
}  
/////////////////////////////////////////////////////////////////////////////////////
static PyMethodDef ModuleMethods[] =
{
    {"spreadingPngAlpha", Menge::spreadingPngAlpha, METH_VARARGS, "Greet somebody."},
    {NULL, NULL, 0, NULL}
};
/////////////////////////////////////////////////////////////////////////////////////
struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "ToolsPNGAlphaSpreadingPlugin",
    NULL,
    -1,
    ModuleMethods,
    NULL, NULL, NULL, NULL
};
///////////////////////////////////////////////////////////////////////////////////
PyMODINIT_FUNC PyInit_ToolsPNGAlphaSpreadingPlugin(void)
{
    if( Menge::initialize() == false )
    {
        printf("PyInit_ToolsPNGAlphaSpreadingPlugin Menge::initialize failed\n"
            );

        return NULL;
    }

    PyObject * m =  PyModule_Create( &module_def );

    if( m == NULL )
    {
        return NULL;
    }

    PyToolException = PyErr_NewException("PyInit_ToolsPNGAlphaSpreadingPlugin.error", NULL, NULL);

    Py_INCREF( PyToolException );
    PyModule_AddObject( m, "error", PyToolException );

    return m;
}