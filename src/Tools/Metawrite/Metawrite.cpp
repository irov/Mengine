#include "Interface/PluginInterface.h"

#include "Interface/ServiceProviderInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Plugins/XmlToBinPlugin/XmlToBinInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/LoggerBase.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/FactorableUnique.h"

static void parse_arg( const std::wstring & _str, Mengine::WString & _value )
{
    _value = Mengine::WString( _str.begin(), _str.end() );
}

#include "ToolUtils/ToolUtils.h"
#include "ToolUtils/ToolLogger.h"

#include "Environment/Windows/WindowsIncluder.h"

#include <string>
#include <vector>

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorSystem );
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
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( Platform );
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

        SERVICE_CREATE( OptionsService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( FactoryService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( UnicodeSystem, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( ArchiveService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( LoggerService, MENGINE_DOCUMENT_FUNCTION );

        LOGGER_SERVICE()
            ->setVerboseLevel( LM_WARNING );

        LOGGER_SERVICE()
            ->registerLogger( Helper::makeFactorableUnique<ToolLogger>( MENGINE_DOCUMENT_FUNCTION ) );

        SERVICE_CREATE( CodecService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( DataService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ConfigService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( TimeSystem, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ThreadSystem, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( ThreadService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( MemoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PluginService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( Platform, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( FileService, MENGINE_DOCUMENT_FUNCTION );

        PLUGIN_CREATE( Zip, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( LZ4, MENGINE_DOCUMENT_FUNCTION );

        return true;
    }
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    MENGINE_UNUSED( hInstance );
    MENGINE_UNUSED( hPrevInstance );
    MENGINE_UNUSED( lpCmdLine );
    MENGINE_UNUSED( nShowCmd );

    {
        PWSTR pwCmdLine = ::GetCommandLineW();

        Mengine::WString protocol = parse_kwds( pwCmdLine, L"--protocol", Mengine::WString() );
        Mengine::WString in = parse_kwds( pwCmdLine, L"--in", Mengine::WString() );
        Mengine::WString out = parse_kwds( pwCmdLine, L"--out", Mengine::WString() );

        if( in.empty() == true )
        {
            message_error( "not found 'in' param" );

            return EXIT_FAILURE;
        }

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

        Mengine::FilePath fp_protocol = Mengine::Helper::unicodeToFilePath( protocol );
        Mengine::FilePath fp_in = Mengine::Helper::unicodeToFilePath( in );
        Mengine::FilePath fp_out = Mengine::Helper::unicodeToFilePath( out );

        if( PLUGIN_SERVICE()
            ->loadPlugin( "XmlToBinPlugin.dll", MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return EXIT_FAILURE;
        }

        using namespace Mengine::Literals;

        Mengine::XmlDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoder( STRINGIZE_STRING_LOCAL( "xml2bin" ), MENGINE_DOCUMENT_FUNCTION );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "invalid create decoder xml2bin for %s"
                , fp_in.c_str()
            );

            return EXIT_FAILURE;
        }

        if( decoder->prepareData( nullptr ) == false )
        {
            LOGGER_ERROR( "invalid initialize decoder xml2bin for %s"
                , fp_in.c_str()
            );

            return EXIT_FAILURE;
        }

        Mengine::XmlDecoderData data;
        data.pathProtocol = fp_protocol;

        Mengine::FileGroupInterfacePtr fileGroup = FILE_SERVICE()
            ->getFileGroup( Mengine::ConstString::none() );

        if( fileGroup == nullptr )
        {
            LOGGER_ERROR( "LoaderEngine::makeBin_ invalid get file group" );

            return EXIT_FAILURE;
        }

        const Mengine::FilePath & path = fileGroup->getRelationPath();

        data.pathXml = Mengine::Helper::concatenationFilePath( path, fp_in );
        data.pathBin = Mengine::Helper::concatenationFilePath( path, fp_out );

        if( decoder->decode( &data ) == 0 )
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}