#	include "XmlToAekConverter.h"

#   include "Interface/ServiceInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/LogSystemInterface.h"
#   include "Interface/LoaderInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/PluginInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/XmlCodecInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#   include "Logger/Logger.h"

#   include <Python.h>

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

#	include "Menge/Consts.h"

#   include "Config/Typedef.h"

extern PyObject * PyToolException;

namespace Menge
{
	extern ServiceProviderInterface * serviceProvider;
	//////////////////////////////////////////////////////////////////////////
	static bool s_writeAek( const WString & _protocolPath, const WString & _xmlPath, const WString & _aekPath )
	{
		String utf8_protocolPath;
		Helper::unicodeToUtf8( serviceProvider, _protocolPath, utf8_protocolPath );

		String utf8_xmlPath;
		Helper::unicodeToUtf8( serviceProvider, _xmlPath, utf8_xmlPath );

		String utf8_aekPath;
		Helper::unicodeToUtf8( serviceProvider, _aekPath, utf8_aekPath );

		LOADER_SERVICE(serviceProvider)
			->setProtocolPath( Helper::stringizeString(serviceProvider, utf8_protocolPath) );

		ConverterOptions options;

		options.pakName = ConstString::none();
		options.inputFileName = Helper::stringizeString(serviceProvider, utf8_xmlPath);
		options.outputFileName = Helper::stringizeString(serviceProvider, utf8_aekPath);
		
		ConverterInterface * converter = CONVERTER_SERVICE(serviceProvider)
			->createConverter( Helper::stringizeString(serviceProvider, "binToAekMovie") );

		converter->setOptions( &options );
		
		if( converter->convert() == false )
		{
			LOGGER_ERROR(serviceProvider)( "ConverterEngine::convert can't convert '%s'\nfrom: %s\nto: %s\n"
				, Helper::stringizeString(serviceProvider, "binToAekMovie").c_str()
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);

			converter->destroy();

			return false;
		}

		converter->destroy();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * writeAek( PyObject* self, PyObject* args )
	{
		(void)self;

		const wchar_t * protocolPath;
		const wchar_t * xmlPath;
		const wchar_t * aekPath;

		if( !PyArg_ParseTuple(args, "uuu", &protocolPath, &xmlPath, &aekPath ) )
		{
			LOGGER_ERROR(serviceProvider)("writeAek: error parse args"
				);

			return NULL;
		}

		if( s_writeAek( protocolPath, xmlPath, aekPath ) == false )
		{
			LOGGER_ERROR(serviceProvider)("writeAek: error write bin"
				);

			return NULL;
		}

		Py_RETURN_NONE;
	}
}

