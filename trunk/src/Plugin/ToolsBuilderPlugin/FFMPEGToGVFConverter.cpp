#	include "FFMPEGToGVFConverter.h"

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
	static bool s_convertFFMPEGToGVF( const WString & _ffmpegPath, const WString & _gvfPath )
	{		
		String utf8_ffmpegPath;
		Helper::unicodeToUtf8( serviceProvider, _ffmpegPath, utf8_ffmpegPath );

		String utf8_gvfPath;
		Helper::unicodeToUtf8( serviceProvider, _gvfPath, utf8_gvfPath );

		ConverterOptions options;
		   
		options.pakName = ConstString::none();
		options.inputFileName = Helper::stringizeString(serviceProvider, utf8_ffmpegPath);
		options.outputFileName = Helper::stringizeString(serviceProvider, utf8_gvfPath);

		ConverterInterfacePtr converter = CONVERTER_SERVICE(serviceProvider)
			->createConverter( Helper::stringizeString(serviceProvider, "ffmpegToGVF") );

		if( converter == nullptr )
		{
			LOGGER_ERROR(serviceProvider)( "writeAek can't create convert '%s'\nfrom: %s\nto: %s\n"
				, "ffmpegToGVF"
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);

			return false;
		}

		converter->setOptions( &options );

		if( converter->convert() == false )
		{
			LOGGER_ERROR(serviceProvider)( "ConverterEngine::convert can't convert '%s'\nfrom: %s\nto: %s\n"
				, "ffmpegToGVF"
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);
			
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * convertFFMPEGToGVF( PyObject* self, PyObject* args )
	{
		(void)self;

		const wchar_t * ffmpegPath;
		const wchar_t * gvfPath;

		if( !PyArg_ParseTuple(args, "uu", &ffmpegPath, &gvfPath ) )
		{
			LOGGER_ERROR(serviceProvider)("convertFFMPEGToGVF: error parse args"
				);

			return NULL;
		}

		LOGGER_WARNING(serviceProvider)("%ls %ls"
			, ffmpegPath
			, gvfPath
			);

		if( s_convertFFMPEGToGVF( ffmpegPath, gvfPath ) == false )
		{
			LOGGER_ERROR(serviceProvider)("convertFFMPEGToGVF: error process"
				);

			return NULL;
		}

		Py_RETURN_NONE;
	}
}

