#	include "XmlToBinDecoder.h"

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

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

#   include "Config/Typedef.h"

namespace Menge
{
    extern ServiceProviderInterface * serviceProvider;
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
            ->createDecoderT<XmlDecoderInterfacePtr>( Helper::stringizeString(serviceProvider, "xml2bin") );

        if( decoder == nullptr )
        {
            LOGGER_ERROR(serviceProvider)("writeBin invalid create decoder xml2bin for %s"
                , utf8_xmlPath.c_str()
                );

            return false;
        }

		if( decoder->prepareData( nullptr ) == false )
		{
			LOGGER_ERROR(serviceProvider)("writeBin invalid initialize decoder xml2bin for %s"
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
    PyObject * writeBin( const wchar_t * protocolPath, const wchar_t * xmlPath, const wchar_t * binPath )
    {
		if( s_writeBin( protocolPath, xmlPath, binPath ) == false )
        {
            LOGGER_ERROR(serviceProvider)("writeBin: error write bin"
                );

            return nullptr;
        }

        return pybind::ret_none();
    }
}

