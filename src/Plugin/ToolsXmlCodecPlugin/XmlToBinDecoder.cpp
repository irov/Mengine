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
#   include "Interface/XmlCodecInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#   include "Logger/Logger.h"

#	include <Windows.h>
#   include <Python.h>

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

#   include "Config/Typedef.h"

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

static PyObject * PyToolException;

namespace Menge
{
    static ServiceProviderInterface * serviceProvider = NULL;

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
            void setVerboseLevel( EMessageLevel _level ) override
            {
                (void)_level;
            };

            void setVerboseFlag( size_t _flag ) override
            {
                (void)_flag;
            };

            bool validMessage( EMessageLevel _level, size_t _flag ) const override 
            { 
                (void)_level;
                (void)_flag;

                return true; 
            };

        public:
            void log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count ) override
            {
                (void)_level;
                (void)_flag;
                (void)_count;                

                printf("%s"
                    , _data
                    );
            }

            void flush() override 
            {
            };
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
        
        PluginInterface * plugin_hit = PLUGIN_SERVICE(serviceProvider)
            ->loadPlugin( "MengeXmlCodecPlugin.dll" );

        ConstString dev = Helper::stringizeString(serviceProvider, "dev");

        if( FILE_SERVICE(serviceProvider)
            ->mountFileGroup( dev, ConstString::none(), Helper::stringizeString(serviceProvider, "dir"), false) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_writeBin( const WString & _protocolPath, const WString & _xmlPath, const WString & _binPath )
    {
        String utf8_protocolPath;
        Helper::unicodeToUtf8( serviceProvider, _protocolPath, utf8_protocolPath );

        String utf8_xmlPath;
        Helper::unicodeToUtf8( serviceProvider, _xmlPath, utf8_xmlPath );

        String utf8_binPath;
        Helper::unicodeToUtf8( serviceProvider, _binPath, utf8_binPath );

        XmlDecoderInterfacePtr decoder = CODEC_SERVICE(serviceProvider)
            ->createDecoderT<XmlDecoderInterfacePtr>( Helper::stringizeString(serviceProvider, "xml2bin"), 0 );

        if( decoder == NULL )
        {
            LOGGER_ERROR(serviceProvider)("writeBin invalid create decoder xml2bin for %s"
                , utf8_xmlPath.c_str()
                );

            return false;
        }


        XmlCodecOptions options;
        options.pathProtocol = Helper::stringizeString( serviceProvider, utf8_protocolPath );
        options.pathXml = Helper::stringizeString( serviceProvider, utf8_xmlPath );
        options.pathBin = Helper::stringizeString( serviceProvider, utf8_binPath );

        if( decoder->setOptions( &options ) == false )
        {
            LOGGER_ERROR(serviceProvider)("writeBin invalid setup decoder xml2bin for %s"
                , utf8_xmlPath.c_str()
                );

            return false;
        }

        if( decoder->decode( 0, 0 ) == 0 )
        {
            LOGGER_ERROR(serviceProvider)("writeBin invalid decode %s"
                , utf8_xmlPath.c_str()
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static PyObject * py_writeBin( PyObject* self, PyObject* args )
    {
        (void)self;

        const wchar_t * protocolPath;
        const wchar_t * xmlPath;
        const wchar_t * binPath;

        if (!PyArg_ParseTuple(args, "uuu", &protocolPath, &xmlPath, &binPath ))
        {
            LOGGER_ERROR(serviceProvider)("writeBin: error parse args"
                );

            return NULL;
        }

        if( s_writeBin( protocolPath, xmlPath, binPath ) == false )
        {
            LOGGER_ERROR(serviceProvider)("writeBin: error write bin"
                );

            return NULL;
        }

        Py_RETURN_NONE;
    }
    
    //////////////////////////////////////////////////////////////////////////

}
/////////////////////////////////////////////////////////////////////////////////////
static PyMethodDef ModuleMethods[] =
{
    {"writeBin", Menge::py_writeBin, METH_VARARGS, "Greet somebody."},
    {NULL, NULL, 0, NULL}
};
/////////////////////////////////////////////////////////////////////////////////////
struct PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    "ToolsXmlCodecPlugin",
    NULL,
    -1,
    ModuleMethods,
    NULL, NULL, NULL, NULL
};
///////////////////////////////////////////////////////////////////////////////////
PyMODINIT_FUNC PyInit_ToolsXmlCodecPlugin(void)
{
    if( Menge::initialize() == false )
    {
        printf("PyInit_ToolsXmlCodecPlugin Menge::initialize failed\n"
            );

        return NULL;
    }

    PyObject * m =  PyModule_Create( &module_def );

    if( m == NULL )
    {
        return NULL;
    }

    PyToolException = PyErr_NewException("ToolsXmlCodecPlugin.error", NULL, NULL);

    Py_INCREF(PyToolException);

    PyModule_AddObject(m, "error", PyToolException);

    return m;
}
