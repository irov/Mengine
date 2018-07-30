#	include "XmlToBinDecoder.h"

#   include "Interface/ServiceInterface.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"
#   include "Interface/LoggerInterface.h"
#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/PluginInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/XmlCodecInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#   include "Kernel/Logger.h"

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

#   include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static bool s_writeBin( const WString & _protocolPath, const WString & _xmlPath, const WString & _binPath )
    {
        String utf8_protocolPath;
        Helper::unicodeToUtf8( _protocolPath, utf8_protocolPath );

        String utf8_xmlPath;
        Helper::unicodeToUtf8( _xmlPath, utf8_xmlPath );

        String utf8_binPath;
        Helper::unicodeToUtf8( _binPath, utf8_binPath );

        XmlDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<XmlDecoderInterfacePtr>( Helper::stringizeString("xml2bin") );

        if( decoder == nullptr )
        {
            LOGGER_ERROR("writeBin invalid create decoder xml2bin for %s"
                , utf8_xmlPath.c_str()
                );

            return false;
        }

		if( decoder->prepareData( nullptr ) == false )
		{
			LOGGER_ERROR("writeBin invalid initialize decoder xml2bin for %s"
				, utf8_xmlPath.c_str()
				);

			return false;
		}

        XmlCodecOptions options;
        options.pathProtocol = Helper::stringizeFilePath( utf8_protocolPath );
        options.pathXml = Helper::stringizeFilePath( utf8_xmlPath );
        options.pathBin = Helper::stringizeFilePath( utf8_binPath );

        if( decoder->setOptions( &options ) == false )
        {
            LOGGER_ERROR("writeBin invalid setup decoder xml2bin for %s"
                , utf8_xmlPath.c_str()
                );

            return false;
        }

        if( decoder->decode( 0, 0 ) == 0 )
        {
            LOGGER_ERROR("writeBin invalid decode %s"
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
            LOGGER_ERROR("writeBin: error write bin"
                );

            return nullptr;
        }

        return pybind::ret_none();
    }
}

