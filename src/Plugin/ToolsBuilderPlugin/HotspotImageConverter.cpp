#   include "HotspotImageConverter.h"

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

#   include <Python.h>

#   include "Logger\Logger.h"

extern PyObject * PyToolException;

namespace Menge
{
    extern ServiceProviderInterface * serviceProvider;
	 
    PyObject * convertPngToHit( PyObject * _self, PyObject * _args )
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

            return nullptr;
        }

        ConverterInterfacePtr converter = CONVERTER_SERVICE(serviceProvider)
            ->createConverter(Helper::stringizeString(serviceProvider, "png2hit"));

        if( converter == nullptr )
        {
            LOGGER_ERROR(serviceProvider)("Converter png2hit not create!"
                );

            return nullptr;
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

        if( converter->convert() == false )
        {
            return nullptr;
        }
		        
        Py_RETURN_NONE;
    } 
}  
