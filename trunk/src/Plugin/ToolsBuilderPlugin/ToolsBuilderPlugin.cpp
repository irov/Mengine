#	include "Interface/ServiceInterface.h"

#	include "XmlToBinDecoder.h"
#	include "HotspotImageConverter.h"
#	include "AlphaSpreadingPlugin.h"
#	include "XmlToAekConverter.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/LogSystemInterface.h"
#	include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/PluginInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ImageCodecInterface.h"
#	include "Interface/LoaderInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#	include "Codec/ConverterFactory.h"
#	include "Menge/MovieKeyConverter.h"


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

SERVICE_EXTERN(LoaderService, Menge::LoaderServiceInterface);

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
			void setVerboseLevel( EMessageLevel _level ) override 
			{
				(void)_level;
			};

			void setVerboseFlag( size_t _flag ) override 
			{
				(void)_flag;
			};

		public:
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
			}
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
		
		LoaderServiceInterface * loaderService;
		if( SERVICE_CREATE( LoaderService, &loaderService ) == false )
		{
			return false;
		}

		SERVICE_REGISTRY( serviceProvider, loaderService );

		PluginInterface * plugin_hit = PLUGIN_SERVICE(serviceProvider)
			->loadPlugin( L"MengeDevelopmentConverterPlugin.dll" );

		PluginInterface * plugin_xml = PLUGIN_SERVICE(serviceProvider)
			->loadPlugin( L"MengeXmlCodecPlugin.dll" );

		if( FILE_SERVICE(serviceProvider)
			->mountFileGroup( ConstString::none(), ConstString::none(), ConstString::none(), Helper::stringizeString(serviceProvider, "dir"), false) == false )
		{
			return false;
		}

		ConstString dev = Helper::stringizeString(serviceProvider, "dev");

		if( FILE_SERVICE(serviceProvider)
			->mountFileGroup( dev, ConstString::none(), ConstString::none(), Helper::stringizeString(serviceProvider, "dir"), false) == false )
		{
			return false;
		}

		return true;
	}
}
/////////////////////////////////////////////////////////////////////////////////////
static PyMethodDef ModuleMethods[] =
{
	{"spreadingPngAlpha", Menge::spreadingPngAlpha, METH_VARARGS, "spreading png alpha"},
	{"writeBin", Menge::writeBin, METH_VARARGS, "write binary"},
	{"writeAek", Menge::writeAek, METH_VARARGS, "write aek"},	
	{"convertPngToHit", Menge::convertPngToHit, METH_VARARGS, "convert png to hit"},
	{NULL, NULL, 0, NULL}
};
/////////////////////////////////////////////////////////////////////////////////////
struct PyModuleDef module_def = {
	PyModuleDef_HEAD_INIT,
	"ToolsBuilderPlugin",
	NULL,
	-1,
	ModuleMethods,
	NULL, NULL, NULL, NULL
};
///////////////////////////////////////////////////////////////////////////////////
PyMODINIT_FUNC PyInit_ToolsBuilderPlugin(void)
{
	if( Menge::initialize() == false )
	{
		printf("PyInit_ToolsBuilderPlugin Menge::initialize failed\n"
			);

		return NULL;
	}

	PyObject * m =  PyModule_Create( &module_def );

	if( m == NULL )
	{
		return NULL;
	}

	PyToolException = PyErr_NewException("PyInit_ToolsBuilderPlugin.error", NULL, NULL);

	Py_INCREF( PyToolException );

	PyModule_AddObject( m, "error", PyToolException );

	return m;
}