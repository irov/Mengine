#include "Interface/ArchiveServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/Data.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "Metacode/Metacode.h"
#include "Metacode/Metaprotocol.h"

#include "metabuf/Metaconvert.hpp"

#include "ToolEngine/ToolEngine.h"
#include "ToolUtils/ToolUtils.h"

#include <cstdlib>
#include <string>

//////////////////////////////////////////////////////////////////////////
static bool writeBin( const std::wstring & _inputFormat, const std::wstring & _outputFormat, const std::wstring & _meta, const std::wstring & _node, const std::wstring & _input, const std::wstring & _output )
{
    if( _outputFormat != L"bin" )
    {
        LOGGER_ERROR( "Metawrite unsupported output format" );

        return false;
    }

    const Mengine::FilePath inputPath = Mengine::Helper::unicodeToFilePath( Mengine::WString( _input.begin(), _input.end() ) );
    const Mengine::FilePath outputPath = Mengine::Helper::unicodeToFilePath( Mengine::WString( _output.begin(), _output.end() ) );

    Mengine::FileGroupInterfacePtr fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

    if( fileGroup == nullptr )
    {
        LOGGER_ERROR( "Metawrite cannot get tool file group" );

        return false;
    }

    Mengine::ContentInterfacePtr inputContent = Mengine::Helper::makeFileContent( fileGroup, inputPath, MENGINE_DOCUMENT_FUNCTION );
    Mengine::ContentInterfacePtr outputContent = Mengine::Helper::makeFileContent( fileGroup, outputPath, MENGINE_DOCUMENT_FUNCTION );

    Mengine::InputStreamInterfacePtr inputStream = inputContent->openInputStreamFile( false, false, MENGINE_DOCUMENT_FUNCTION );

    if( inputStream == nullptr )
    {
        LOGGER_ERROR( "Metawrite cannot open input" );

        return false;
    }

    Mengine::MemoryInterfacePtr inputMemory = Mengine::Helper::createMemoryStream( inputStream, MENGINE_DOCUMENT_FUNCTION );

    inputContent->closeInputStreamFile( inputStream );

    const Metacode::MetaprotocolGenerator generator;
    Metabuf::MetaconvertInterface * metaconvert;

    if( _inputFormat == L"json" )
    {
        metaconvert = Metabuf::createJsonMetaconvert( &generator );
    }
    else if( _inputFormat == L"xml" )
    {
        metaconvert = Metabuf::createXmlMetaconvert( &generator );
    }
    else
    {
        LOGGER_ERROR( "Metawrite unsupported input format" );

        return false;
    }

    if( metaconvert == nullptr )
    {
        LOGGER_ERROR( "Metawrite cannot create Metaconvert" );

        return false;
    }

    std::string error;

    const std::string metaName( _meta.begin(), _meta.end() );
    const std::string nodeName( _node.begin(), _node.end() );
    const Metabuf::MetaInterface * meta = metaconvert->getMeta( metaName );

    if( meta == nullptr )
    {
        LOGGER_ERROR( "Metawrite cannot find meta '%s'", metaName.c_str() );

        Metabuf::destroyMetaconvert( metaconvert );

        return false;
    }

    const Metabuf::NodeInterface * node = meta->getNode( nodeName );

    if( node == nullptr )
    {
        LOGGER_ERROR( "Metawrite cannot find node '%s' in meta '%s'", nodeName.c_str(), metaName.c_str() );

        Metabuf::destroyMetaconvert( metaconvert );

        return false;
    }

    Mengine::Data raw;

    if( metaconvert->convert( inputMemory->getBuffer(), inputMemory->getSize(), meta, node, raw, error ) == false )
    {
        LOGGER_ERROR( "Metawrite cannot convert '%s': %s", inputPath.c_str(), error.c_str() );

        Metabuf::destroyMetaconvert( metaconvert );

        return false;
    }

    Mengine::Data header;

    if( metaconvert->makeHeader( meta, header, error ) == false )
    {
        LOGGER_ERROR( "Metawrite cannot make header: %s", error.c_str() );

        Metabuf::destroyMetaconvert( metaconvert );

        return false;
    }

    Mengine::ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );
    Mengine::MemoryInputInterfacePtr compressed = ARCHIVE_SERVICE()
        ->compressBuffer( archivator, raw.data(), raw.size(), Mengine::EAC_BEST );

    MENGINE_ASSERTION_MEMORY_PANIC( compressed, "Metawrite cannot compress output" );

    Mengine::OutputStreamInterfacePtr outputStream = outputContent->openOutputStreamFile( true, MENGINE_DOCUMENT_FUNCTION );

    if( outputStream == nullptr )
    {
        Metabuf::destroyMetaconvert( metaconvert );

        return false;
    }

    const uint32_t rawSize = static_cast<uint32_t>(raw.size());
    const uint32_t compressedSize = static_cast<uint32_t>(compressed->getSize());

    bool successful = outputStream->write( header.data(), header.size() ) == header.size() &&
        outputStream->write( &rawSize, sizeof( rawSize ) ) == sizeof( rawSize ) &&
        outputStream->write( &compressedSize, sizeof( compressedSize ) ) == sizeof( compressedSize ) &&
        outputStream->write( compressed->getBuffer(), compressedSize ) == compressedSize &&
        outputStream->flush();

    successful &= outputContent->closeOutputStreamFile( outputStream );

    Metabuf::destroyMetaconvert( metaconvert );

    return successful;
}
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    (void)argc;
    (void)argv;

    if( has_args( L"--help" ) == true || has_args( L"-h" ) == true )
    {
        message_error( "usage: Metawrite --input-format <json|xml> --output-format bin --meta <meta> --node <node> --in <source> --out <output.bin>" );

        return EXIT_SUCCESS;
    }

    if( has_args( L"--protocol" ) == true )
    {
        message_error( "Metawrite no longer accepts --protocol; the protocol is embedded in the tool" );

        return EXIT_FAILURE;
    }

    const std::wstring inputFormat = parse_kwds( L"--input-format", std::wstring() );
    const std::wstring outputFormat = parse_kwds( L"--output-format", std::wstring() );
    const std::wstring meta = parse_kwds( L"--meta", std::wstring() );
    const std::wstring node = parse_kwds( L"--node", std::wstring() );
    const std::wstring input = parse_kwds( L"--in", std::wstring() );
    const std::wstring output = parse_kwds( L"--out", std::wstring() );

    if( inputFormat.empty() == true )
    {
        message_error( "Metawrite missing required argument '--input-format'. Expected 'json' or 'xml'." );

        return EXIT_FAILURE;
    }

    if( outputFormat.empty() == true )
    {
        message_error( "Metawrite missing required argument '--output-format'. Expected 'bin'." );

        return EXIT_FAILURE;
    }

    if( meta.empty() == true )
    {
        message_error( "Metawrite missing required argument '--meta'. Specify the embedded protocol meta name, for example 'Data'." );

        return EXIT_FAILURE;
    }

    if( node.empty() == true )
    {
        message_error( "Metawrite missing required argument '--node'. Specify the embedded protocol node name, for example 'Pak'." );

        return EXIT_FAILURE;
    }

    if( input.empty() == true )
    {
        message_error( "Metawrite missing required argument '--in'. Specify the input JSON or XML file path." );

        return EXIT_FAILURE;
    }

    if( output.empty() == true )
    {
        message_error( "Metawrite missing required argument '--out'. Specify the output BIN file path." );

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

    const bool successful = writeBin( inputFormat, outputFormat, meta, node, input, output );
    Mengine::ToolEngineFinalize();

    return successful == true ? EXIT_SUCCESS : EXIT_FAILURE;
}
//////////////////////////////////////////////////////////////////////////
