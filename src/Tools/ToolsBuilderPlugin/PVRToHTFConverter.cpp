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
	static bool s_convertPVRToHTF( const WString & _pvrPath, const WString & _htfPath )
	{		
		String utf8_pvrPath;
		Helper::unicodeToUtf8( serviceProvider, _pvrPath, utf8_pvrPath );

		String utf8_htfPath;
		Helper::unicodeToUtf8( serviceProvider, _htfPath, utf8_htfPath );

		ConverterOptions options;
		   
		options.pakName = ConstString::none();
		options.inputFileName = Helper::stringizeString(serviceProvider, utf8_pvrPath);
		options.outputFileName = Helper::stringizeString(serviceProvider, utf8_htfPath);

		ConverterInterfacePtr converter = CONVERTER_SERVICE(serviceProvider)
			->createConverter( Helper::stringizeString(serviceProvider, "pvr2htf") );

		if( converter == nullptr )
		{
			LOGGER_ERROR(serviceProvider)("convertPVRToHTF can't create convert '%s'\nfrom: %s\nto: %s\n"
				, "pvr2htf"
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);

			return false;
		}

		converter->setOptions( &options );

		if( converter->convert() == false )
		{
			LOGGER_ERROR(serviceProvider)( "convertPVRToHTF can't convert '%s'\nfrom: %s\nto: %s\n"
				, "pvr2htf"
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);
			
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * convertPVRToHTF( PyObject* self, PyObject* args )
	{
		(void)self;

		const wchar_t * pvrPath;
		const wchar_t * htfPath;

		if( !PyArg_ParseTuple(args, "uu", &pvrPath, &htfPath ) )
		{
			LOGGER_ERROR(serviceProvider)("convertPVRToHTF: error parse args"
				);

			return NULL;
		}

		LOGGER_WARNING(serviceProvider)("%ls %ls"
			, pvrPath
			, htfPath
			);

		if( s_convertPVRToHTF( pvrPath, htfPath ) == false )
		{
			LOGGER_ERROR(serviceProvider)("convertPVRToHTF: error process"
				);

			return NULL;
		}

		Py_RETURN_NONE;
	}
}

