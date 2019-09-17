#include "XmlToAekConverter.h"

#include "Interface/ServiceInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/LoaderInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"

#include "Plugins/XmlToBinPlugin/XmlToBinInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/UnicodeHelper.h"

#include "Xml2Metabuf.hpp"
#include "Xml2Metacode.hpp"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static bool s_writeAek( const WString & _protocolPath, const WString & _xmlPath, const WString & _aekPath )
    {
        String utf8_protocolPath;
        Helper::unicodeToUtf8( _protocolPath, utf8_protocolPath );

        String utf8_xmlPath;
        Helper::unicodeToUtf8( _xmlPath, utf8_xmlPath );

        String utf8_aekPath;
        Helper::unicodeToUtf8( _aekPath, utf8_aekPath );

        FilePath fp_protocolPath = Helper::stringizeFilePath( utf8_protocolPath );

        LOADER_SERVICE()
            ->setProtocolPath( fp_protocolPath );

        String framePackPath( utf8_xmlPath.c_str(), utf8_xmlPath.size() );

        String::size_type size = framePackPath.size();
        framePackPath[size - 3] = L'x';
        framePackPath[size - 2] = L'm';
        framePackPath[size - 1] = L'l';

        FilePath path_xml = Helper::stringizeFilePath( framePackPath );

        ConverterOptions options;

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        options.fileGroup = fileGroup;
        options.inputFilePath = path_xml;
        options.outputFilePath = Helper::stringizeFilePath( utf8_aekPath );

        ConverterInterfacePtr converter = CONVERTER_SERVICE()
            ->createConverter( STRINGIZE_STRING_LOCAL( "xmlToAekMovie" ), MENGINE_DOCUMENT_FUNCTION );

        if( converter == nullptr )
        {
            LOGGER_ERROR( "writeAek can't create convert '%s'\nfrom: %s\nto: %s\n"
                , "xmlToAekMovie"
                , options.inputFilePath.c_str()
                , options.outputFilePath.c_str()
            );

            return false;
        }

        converter->setOptions( &options );

        if( converter->convert() == false )
        {
            LOGGER_ERROR( "can't convert '%s'\nfrom: %s\nto: %s\n"
                , "xmlToAekMovie"
                , options.inputFilePath.c_str()
                , options.outputFilePath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * writeAek( pybind::kernel_interface * _kernel, const wchar_t * protocolPath, const wchar_t * xmlPath, const wchar_t * aekPath )
    {
        if( s_writeAek( protocolPath, xmlPath, aekPath ) == false )
        {
            LOGGER_ERROR( "writeAek: error write bin"
            );

            return nullptr;
        }

        return _kernel->ret_none();
    }
}

