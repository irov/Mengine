#   include "HotspotImageConverter.h"

#   include <Python.h>

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

#   include "WindowsLayer/VistaWindowsLayer.h"

#   include "Logger\Logger.h"

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

        PluginInterface * plugin_hit = PLUGIN_SERVICE(serviceProvider)
            ->loadPlugin( "MengeHotspotImageConverterPlugin.dll" );

        if( FILE_SERVICE(serviceProvider)
            ->mountFileSystem( Helper::stringizeString(serviceProvider, ""), ConstString::none(), Helper::stringizeString(serviceProvider, "dir"), false) == false )
        {
            return false;
        }

        return true;
    }

    static PyObject * convertPngToHit( PyObject * _self, PyObject * _args )
    {
        LOGGER_INFO(serviceProvider)("convertPngToHit\n");

        const wchar_t * pngPath;
        const wchar_t * hitPath;

        if( !PyArg_ParseTuple(_args, "uu", &pngPath, &hitPath) )
        {
            char error[512];
            sprintf( error, "convertPngToHit::decode: error parse args"
                );

            PyErr_SetString( PyToolException, error );

            return NULL;
        }

        ConverterInterface * converter = CONVERTER_SERVICE(serviceProvider)
            ->createConverter(Helper::stringizeString(serviceProvider, "png2hit"));

        if( converter == NULL )
        {
            LOGGER_ERROR(serviceProvider)("Converter png2hit not create!"
                );

            return NULL;
        }

        ConverterOptions options; 
        options.pakName = Helper::stringizeString(serviceProvider, "");
        
        
        String utf8_inputFileName;
        Helper::unicodeToUtf8(serviceProvider, pngPath, utf8_inputFileName);

        String utf8_outputFileName;
        Helper::unicodeToUtf8(serviceProvider, hitPath, utf8_outputFileName);

        options.inputFileName = Helper::stringizeString(serviceProvider, utf8_inputFileName);
        options.outputFileName = Helper::stringizeString(serviceProvider, utf8_outputFileName);
        
        converter->setOptions( &options );

        bool skip;
        if( converter->convert( skip ) == false )
        {
            converter->destroy();
 
            return NULL;
        }

        converter->destroy();
        
        Py_RETURN_NONE;
    } 
}  
/////////////////////////////////////////////////////////////////////////////////////
static PyMethodDef ModuleMethods[] =
{
    {"convertPngToHit", Menge::convertPngToHit, METH_VARARGS, "Greet somebody."},
    {NULL, NULL, 0, NULL}
};
/////////////////////////////////////////////////////////////////////////////////////
struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "ToolsHotspotImageConverterPlugin",
    NULL,
    -1,
    ModuleMethods,
    NULL, NULL, NULL, NULL
};
///////////////////////////////////////////////////////////////////////////////////
PyMODINIT_FUNC PyInit_ToolsHotspotImageConverterPlugin(void)
{
    if( Menge::initialize() == false )
    {
        printf("PyInit_ToolsHotspotImageConverterPlugin Menge::initialize failed\n"
            );

        return NULL;
    }

    PyObject * m =  PyModule_Create( &module_def );

    if( m == NULL )
    {
        return NULL;
    }

    PyToolException = PyErr_NewException("ToolsHotspotImageConverterPlugin.error", NULL, NULL);

    Py_INCREF( PyToolException );
    PyModule_AddObject( m, "error", PyToolException );

    return m;
}