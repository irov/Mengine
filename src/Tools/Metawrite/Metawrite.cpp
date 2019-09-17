#include "Interface/PluginInterface.h"

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
#include "Kernel/Document.h"
#include "Kernel/LoggerBase.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/FactorableUnique.h"

#include "ToolUtils/ToolUtils.h"

#include "Environment/Windows/WindowsIncluder.h"

#include <string>
#include <vector>

#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( Win32FileGroup );
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider )

SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( PluginService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    static bool initializeEngine()
    {
        ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( FactoryService );

        SERVICE_CREATE( UnicodeSystem );

        SERVICE_CREATE( StringizeService );
        SERVICE_CREATE( ArchiveService );

        SERVICE_CREATE( LoggerService );

        class MyLogger
            : public LoggerBase
        {
        public:
            MyLogger()
                : m_verboseLevel( LM_WARNING )
                , m_verboseFlag( 0xFFFFFFFF )
            {
            }

        public:
            void log( ELoggerLevel _level, uint32_t _flag, const Char * _data, uint32_t _count ) override
            {
                (void)_level;
                (void)_flag;
                (void)_count;

                message_error( "%s"
                    , _data
                );
            }

        protected:
            ELoggerLevel m_verboseLevel;
            uint32_t m_verboseFlag;
        };

        LOGGER_SERVICE()
            ->setVerboseLevel( LM_WARNING );

        LOGGER_SERVICE()
            ->registerLogger( Helper::makeFactorableUnique<MyLogger>() );

        SERVICE_CREATE( CodecService );
        SERVICE_CREATE( DataService );
        SERVICE_CREATE( ConfigService );

        SERVICE_CREATE( ThreadSystem );

        SERVICE_CREATE( ThreadService );
        SERVICE_CREATE( MemoryService );
        SERVICE_CREATE( PluginService );

        SERVICE_CREATE( FileService );

        PLUGIN_CREATE( Win32FileGroup );

        PLUGIN_CREATE( Zip );
        PLUGIN_CREATE( LZ4 );

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        ConstString dev = STRINGIZE_STRING_LOCAL( "dev" );

        if( FILE_SERVICE()
            ->mountFileGroup( dev, nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            return false;
        }

        return true;
    }
}
//////////////////////////////////////////////////////////////////////////
static void parse_arg( const std::wstring & _str, Mengine::WString & _value )
{
    _value = Mengine::WString( _str.begin(), _str.end() );
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)nShowCmd;

    stdex_allocator_initialize();

    Mengine::WString protocol = parse_kwds( lpCmdLine, L"--protocol", Mengine::WString() );
    Mengine::WString in = parse_kwds( lpCmdLine, L"--in", Mengine::WString() );
    Mengine::WString out = parse_kwds( lpCmdLine, L"--out", Mengine::WString() );

    if( in.empty() == true )
    {
        message_error( "not found 'in' param"
        );

        return 0;
    }

    try
    {
        if( Mengine::initializeEngine() == false )
        {
            message_error( "ImageTrimmer invalid initialize" );

            return 0;
        }
    }
    catch( const std::exception & se )
    {
        message_error( "Mengine exception %s"
            , se.what()
        );

        return 0;
    }

    Mengine::FilePath fp_protocol = Mengine::Helper::unicodeToFilePath( protocol );
    Mengine::FilePath fp_in = Mengine::Helper::unicodeToFilePath( in );
    Mengine::FilePath fp_out = Mengine::Helper::unicodeToFilePath( out );

    if( PLUGIN_SERVICE()
        ->loadPlugin( "XmlToBinPlugin.dll" ) == false )
    {
        return 0;
    }

    using namespace Mengine::Literals;

    Mengine::XmlDecoderInterfacePtr decoder = CODEC_SERVICE()
        ->createDecoderT<Mengine::XmlDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "xml2bin" ), MENGINE_DOCUMENT_FUNCTION );

    if( decoder == nullptr )
    {
        LOGGER_ERROR( "invalid create decoder xml2bin for %s"
            , fp_in.c_str()
        );

        return 0;
    }

    if( decoder->prepareData( nullptr ) == false )
    {
        LOGGER_ERROR( "invalid initialize decoder xml2bin for %s"
            , fp_in.c_str()
        );

        return 0;
    }

    Mengine::XmlCodecOptions options;
    options.pathProtocol = fp_protocol;

    Mengine::FileGroupInterfacePtr fileGroup = FILE_SERVICE()
        ->getFileGroup( Mengine::ConstString::none() );

    if( fileGroup == nullptr )
    {
        LOGGER_ERROR( "LoaderEngine::makeBin_ invalid get file group"
        );

        return 0;
    }

    const Mengine::FilePath & path = fileGroup->getRelationPath();

    options.pathXml = Mengine::Helper::concatenationFilePath( path, fp_in );
    options.pathBin = Mengine::Helper::concatenationFilePath( path, fp_out );

    if( decoder->setOptions( &options ) == false )
    {
        return 0;
    }

    if( decoder->decode( 0, 0 ) == 0 )
    {
        return 0;
    }

    return 0;
}