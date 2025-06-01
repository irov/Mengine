#include "Interface/PluginInterface.h"

#include "Interface/ServiceProviderInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/LoggerBase.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ParamsHelper.h"
#include "Kernel/ContentHelper.h"

static void parse_arg( const std::wstring & _str, Mengine::WString & _value )
{
    _value = Mengine::WString( _str.begin(), _str.end() );
}

#include "ToolUtils/ToolUtils.h"
#include "ToolUtils/ToolLogger.h"

#include "Metacode/Metacode.h"

#include <string>
#include <vector>

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorSystem );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( TimeSystem );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( DateTimeSystem );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( VocabularyService );
SERVICE_EXTERN( PlatformSystem );
SERVICE_EXTERN( PlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    static bool initializeEngine()
    {
        ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( AllocatorSystem, nullptr );
        SERVICE_CREATE( StringizeService, nullptr );
        SERVICE_CREATE( DocumentService, nullptr );

        SERVICE_CREATE( PlatformSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( TimeSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ThreadSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( UnicodeSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( DateTimeSystem, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( EnumeratorService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( NotificationService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( OptionsService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( FactoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ArchiveService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( VocabularyService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( LoggerService, MENGINE_DOCUMENT_FUNCTION );

        LOGGER_SERVICE()
            ->setVerboseLevel( LM_MESSAGE );

        LOGGER_SERVICE()
            ->registerLogger( Helper::makeFactorableUnique<ToolLogger>( MENGINE_DOCUMENT_FUNCTION ) );

        SERVICE_CREATE( CodecService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( DataService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ThreadService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( MemoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PluginService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( PlatformService, MENGINE_DOCUMENT_FUNCTION );
        
        SERVICE_CREATE( FileService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ConfigService, MENGINE_DOCUMENT_FUNCTION );

        return true;
    }
}
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

    try
    {
        if( Mengine::initializeEngine() == false )
        {
            message_error( "ImageTrimmer invalid initialize" );

            return EXIT_FAILURE;
        }
    }
    catch( const std::exception & se )
    {
        message_error( "Mengine exception %s"
            , se.what()
        );

        return EXIT_FAILURE;
    }

    Mengine::WString protocol = parse_kwds( L"--protocol", Mengine::WString() );
    Mengine::WString in = parse_kwds( L"--in", Mengine::WString() );
    Mengine::WString out = parse_kwds( L"--out", Mengine::WString() );

    if( in.empty() == true )
    {
        message_error( "not found 'in' param" );

        return EXIT_FAILURE;
    }

    Mengine::FilePath fp_protocol = Mengine::Helper::unicodeToFilePath( protocol );
    Mengine::FilePath fp_in = Mengine::Helper::unicodeToFilePath( in );
    Mengine::FilePath fp_out = Mengine::Helper::unicodeToFilePath( out );

    if( PLUGIN_SERVICE()
        ->loadPlugin( "XmlToBinPlugin.dll", MENGINE_DOCUMENT_FUNCTION ) == false )
    {
        return EXIT_FAILURE;
    }

    using namespace Mengine::Literals;

    Mengine::ConverterInterfacePtr converter = CONVERTER_SERVICE()
        ->createConverter( STRINGIZE_STRING_LOCAL( "xml2bin" ), MENGINE_DOCUMENT_FUNCTION );

    if( converter == nullptr )
    {
        LOGGER_ERROR( "invalid create converter xml2bin for %s"
            , fp_in.c_str()
        );

        return EXIT_FAILURE;
    }

    Mengine::FileGroupInterfacePtr fileGroup = FILE_SERVICE()
        ->getFileGroup( Mengine::ConstString::none() );

    if( fileGroup == nullptr )
    {
        LOGGER_ERROR( "invalid get file group" );

        return EXIT_FAILURE;
    }

    Mengine::ConverterOptions options;
    options.inputContent = Mengine::Helper::makeFileContent( fileGroup, fp_in, MENGINE_DOCUMENT_FUNCTION );
    options.outputContent = Mengine::Helper::makeFileContent( fileGroup, fp_out, MENGINE_DOCUMENT_FUNCTION );

    Mengine::ContentInterfacePtr protocolContent = Mengine::Helper::makeFileContent( fileGroup, fp_protocol, MENGINE_DOCUMENT_FUNCTION );

    uint32_t useProtocolVersion = Metacode::get_metacode_protocol_version();
    uint32_t useProtocolCrc32 = Metacode::get_metacode_protocol_crc32();

    Mengine::Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "protocolContent" ), protocolContent );
    Mengine::Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "useProtocolVersion" ), (Mengine::ParamInteger)useProtocolVersion );
    Mengine::Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "useProtocolCrc32" ), (Mengine::ParamInteger)useProtocolCrc32 );

    converter->setOptions( options );

    if( converter->convert() == false )
    {
        LOGGER_ERROR( "invalid convert %s"
            , fp_in.c_str()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}