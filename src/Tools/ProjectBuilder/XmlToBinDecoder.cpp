#include "XmlToBinDecoder.h"

#include "Interface/ServiceInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Plugins/XmlToBinPlugin/XmlToBinInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/UnicodeHelper.h"

#include "Xml2Metabuf.hpp"
#include "Xml2Metacode.hpp"

#include "Metacode/Metacode.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static bool s_writeBin( const WString & _protocolPath, const WString & _xmlPath, const WString & _binPath )
    {
        String utf8_protocolPath;
        Helper::unicodeToUtf8( _protocolPath, &utf8_protocolPath );

        String utf8_xmlPath;
        Helper::unicodeToUtf8( _xmlPath, &utf8_xmlPath );

        String utf8_binPath;
        Helper::unicodeToUtf8( _binPath, &utf8_binPath );

        XmlDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( STRINGIZE_STRING_LOCAL( "xml2bin" ), MENGINE_DOCUMENT_FUNCTION );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "writeBin invalid create decoder xml2bin for %s"
                , utf8_xmlPath.c_str()
            );

            return false;
        }

        if( decoder->prepareData( nullptr ) == false )
        {
            LOGGER_ERROR( "writeBin invalid initialize decoder xml2bin for %s"
                , utf8_xmlPath.c_str()
            );

            return false;
        }

        XmlDecoderData data;
        data.pathProtocol = Helper::stringizeFilePath( utf8_protocolPath );
        data.pathXml = Helper::stringizeFilePath( utf8_xmlPath );
        data.pathBin = Helper::stringizeFilePath( utf8_binPath );

        data.useProtocolVersion = Metacode::get_metacode_protocol_version();
        data.useProtocolCrc32 = Metacode::get_metacode_protocol_crc32();

        if( decoder->decode( &data ) == 0 )
        {
            LOGGER_ERROR( "writeBin invalid decode %s"
                , utf8_xmlPath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * writeBin( pybind::kernel_interface * _kernel, const wchar_t * protocolPath, const wchar_t * xmlPath, const wchar_t * binPath )
    {
        if( s_writeBin( protocolPath, xmlPath, binPath ) == false )
        {
            LOGGER_ERROR( "writeBin: error write bin"
            );

            return nullptr;
        }

        return _kernel->ret_none();
    }
}

