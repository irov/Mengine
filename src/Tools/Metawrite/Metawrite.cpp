#include "Interface/ConverterServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ParamsHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "Metacode/Metacode.h"

#include "ToolEngine/ToolEngine.h"
#include "ToolUtils/ToolUtils.h"

#include <cstdlib>
#include <string>

//////////////////////////////////////////////////////////////////////////
static bool writeBin( const std::wstring & _protocol, const std::wstring & _input, const std::wstring & _output )
{
    const Mengine::FilePath protocolPath = Mengine::Helper::unicodeToFilePath( Mengine::WString( _protocol.begin(), _protocol.end() ) );
    const Mengine::FilePath inputPath = Mengine::Helper::unicodeToFilePath( Mengine::WString( _input.begin(), _input.end() ) );
    const Mengine::FilePath outputPath = Mengine::Helper::unicodeToFilePath( Mengine::WString( _output.begin(), _output.end() ) );

    Mengine::ConverterInterfacePtr converter = CONVERTER_SERVICE()
        ->createConverter( STRINGIZE_STRING_LOCAL( "xml2bin" ), MENGINE_DOCUMENT_FUNCTION );

    if( converter == nullptr )
    {
        LOGGER_ERROR( "Metawrite cannot create xml2bin converter for '%s'", inputPath.c_str() );
        return false;
    }

    Mengine::FileGroupInterfacePtr fileGroup = VOCABULARY_GET(
        STRINGIZE_STRING_LOCAL( "FileGroup" ),
        STRINGIZE_STRING_LOCAL( "dev" ) );

    if( fileGroup == nullptr )
    {
        LOGGER_ERROR( "Metawrite cannot get tool file group" );
        return false;
    }

    Mengine::ConverterOptions options;
    options.inputContent = Mengine::Helper::makeFileContent( fileGroup, inputPath, MENGINE_DOCUMENT_FUNCTION );
    options.outputContent = Mengine::Helper::makeFileContent( fileGroup, outputPath, MENGINE_DOCUMENT_FUNCTION );

    const Mengine::ContentInterfacePtr protocolContent = Mengine::Helper::makeFileContent( fileGroup, protocolPath, MENGINE_DOCUMENT_FUNCTION );

    Mengine::Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "protocolContent" ), protocolContent );
    Mengine::Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "useProtocolVersion" ), static_cast<Mengine::ParamInteger>( Metacode::get_metacode_protocol_version() ) );
    Mengine::Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "useProtocolCrc32" ), static_cast<Mengine::ParamInteger>( Metacode::get_metacode_protocol_crc32() ) );

    converter->setOptions( options );

    if( converter->convert() == false )
    {
        LOGGER_ERROR( "Metawrite cannot convert '%s'", inputPath.c_str() );
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    (void)argc;
    (void)argv;

    if( has_args( L"--help" ) == true || has_args( L"-h" ) == true )
    {
        message_error( "usage: Metawrite --protocol <protocol.xml> --in <source.xml> --out <output.bin>" );

        return EXIT_SUCCESS;
    }

    const std::wstring protocol = parse_kwds( L"--protocol", std::wstring() );
    const std::wstring input = parse_kwds( L"--in", std::wstring() );
    const std::wstring output = parse_kwds( L"--out", std::wstring() );

    if( protocol.empty() == true )
    {
        message_error( "Metawrite missing required argument: --protocol <protocol.xml>" );

        return EXIT_FAILURE;
    }

    if( input.empty() == true )
    {
        message_error( "Metawrite missing required argument: --in <source.xml>" );

        return EXIT_FAILURE;
    }

    if( output.empty() == true )
    {
        message_error( "Metawrite missing required argument: --out <output.bin>" );

        return EXIT_FAILURE;
    }

    const std::wstring outputDirectory = path_parent( output );

    if( outputDirectory.empty() == false && create_directories( outputDirectory ) == false )
    {
        message_error( "Metawrite cannot create output directory: %ls", outputDirectory.c_str() );

        return EXIT_FAILURE;
    }

    if( Mengine::ToolEngineInitialize() == false )
    {
        message_error( "Metawrite failed to initialize Mengine services" );
        return EXIT_FAILURE;
    }

    const bool successful = writeBin( protocol, input, output );
    Mengine::ToolEngineFinalize();

    return successful == true ? EXIT_SUCCESS : EXIT_FAILURE;
}
//////////////////////////////////////////////////////////////////////////
