#include "XmlToBinDecoder.h"

#include "Interface/ServiceInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/ConverterServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/ParamsHelper.h"

#include "Xml2Metabuf.hpp"
#include "Xml2Metacode.hpp"

#include "Metacode/Metacode.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool writeBin( const wchar_t * _protocolPath, const wchar_t * _xmlPath, const wchar_t * _binPath )
    {
        String utf8_protocolPath;
        Helper::unicodeToUtf8( _protocolPath, &utf8_protocolPath );

        String utf8_xmlPath;
        Helper::unicodeToUtf8( _xmlPath, &utf8_xmlPath );

        String utf8_binPath;
        Helper::unicodeToUtf8( _binPath, &utf8_binPath );

        ConverterInterfacePtr converter = CONVERTER_SERVICE()
            ->createConverter( STRINGIZE_STRING_LOCAL( "xml2bin" ), MENGINE_DOCUMENT_FUNCTION );

        if( converter == nullptr )
        {
            LOGGER_ERROR( "writeBin invalid create converter xml2bin for %s"
                , utf8_xmlPath.c_str()
            );

            return false;
        }

        const FileGroupInterfacePtr & devFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

        ConverterOptions options;
        options.inputContent = Helper::makeFileContent( devFileGroup, Helper::stringizeFilePath( utf8_xmlPath ), MENGINE_DOCUMENT_FUNCTION );
        options.outputContent = Helper::makeFileContent( devFileGroup, Helper::stringizeFilePath( utf8_binPath ), MENGINE_DOCUMENT_FUNCTION );

        ContentInterfacePtr protocolContent = Helper::makeFileContent( devFileGroup, Helper::stringizeFilePath( utf8_protocolPath ), MENGINE_DOCUMENT_FUNCTION );

        uint32_t useProtocolVersion = Metacode::get_metacode_protocol_version();
        uint32_t useProtocolCrc32 = Metacode::get_metacode_protocol_crc32();

        Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "protocolContent" ), protocolContent );
        Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "useProtocolVersion" ), (ParamInteger)useProtocolVersion );
        Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "useProtocolCrc32" ), (ParamInteger)useProtocolCrc32 );

        converter->setOptions( options );

        if( converter->convert() == false )
        {
            LOGGER_ERROR( "writeBin invalid decode %s"
                , utf8_xmlPath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}