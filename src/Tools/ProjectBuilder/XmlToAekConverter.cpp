#	include "XmlToAekConverter.h"

#   include "Interface/ServiceInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/LoggerInterface.h"
#   include "Interface/LoaderInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/PluginInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/XmlCodecInterface.h"
#   include "Interface/ConfigInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#   include "Logger/Logger.h"

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

#	include "Menge/Consts.h"

#   include "Config/Typedef.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
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
		
		FilePath fp_protocolPath = Helper::stringizeFilePath( serviceProvider, utf8_protocolPath );

		LOADER_SERVICE( serviceProvider )
			->setProtocolPath( fp_protocolPath );
	
		String framePackPath( utf8_xmlPath.c_str(), utf8_xmlPath.size() );

		String::size_type size = framePackPath.size();
		framePackPath[size-3] = L'x';
		framePackPath[size-2] = L'm';
		framePackPath[size-1] = L'l';

		FilePath path_xml = Helper::stringizeFilePath( serviceProvider, framePackPath );

		ConverterOptions options;
		   
		options.pakName = ConstString::none();
		options.inputFileName = path_xml;
		options.outputFileName = Helper::stringizeFilePath(serviceProvider, utf8_aekPath);
		
		ConverterInterfacePtr converter = CONVERTER_SERVICE(serviceProvider)
			->createConverter( Helper::stringizeString(serviceProvider, "xmlToAekMovie") );

		if( converter == nullptr )
		{
			LOGGER_ERROR(serviceProvider)( "writeAek can't create convert '%s'\nfrom: %s\nto: %s\n"
				, "xmlToAekMovie"
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);

			return false;
		}
 
		converter->setOptions( &options );
		
		if( converter->convert() == false )
		{
			LOGGER_ERROR(serviceProvider)( "ConverterEngine::convert can't convert '%s'\nfrom: %s\nto: %s\n"
				, Helper::stringizeString(serviceProvider, "xmlToAekMovie").c_str()
				, options.inputFileName.c_str()
				, options.outputFileName.c_str()
				);
			
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * writeAek( const wchar_t * protocolPath, const wchar_t * xmlPath, const wchar_t * aekPath )
	{
		if( s_writeAek( protocolPath, xmlPath, aekPath ) == false )
		{
			LOGGER_ERROR(serviceProvider)("writeAek: error write bin"
				);

			return NULL;
		}

		return pybind::ret_none();
	}
}

