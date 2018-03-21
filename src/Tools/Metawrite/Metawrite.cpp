#	include <Windows.h>

#	include <shellapi.h>

#	include <stdio.h>

#	include <string>
#	include <vector>

#	include "Interface/PluginInterface.h"

#	include "Interface/StringizeInterface.h"
#	include "Interface/LoggerInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/DataInterface.h"
#	include "Interface/MemoryInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/WindowsLayerInterface.h"
#   include "Interface/ImageCodecInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/ConfigInterface.h"
#	include "Interface/XmlCodecInterface.h"

#   include "WindowsLayer/VistaWindowsLayer.h"

#	include "Logger/Logger.h"
#	include "ToolUtils/ToolUtils.h"

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
SERVICE_EXTERN( WindowsLayer );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( PluginSystem );
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
            : public LoggerInterface
        {
        public:
            MyLogger()
                : m_verboseLevel( LM_WARNING )
                , m_verboseFlag( 0xFFFFFFFF )
            {
            }

        public:
            bool initialize() override
            {
                return true;
            }

            void finalize() override
            {
            };

        public:
            void setVerboseLevel( EMessageLevel _level ) override
            {
                m_verboseLevel = _level;
            };

            void setVerboseFlag( size_t _flag ) override
            {
                m_verboseFlag = _flag;
            };

        public:
            bool validMessage( EMessageLevel _level, size_t _flag ) const override
            {
                if( m_verboseLevel < _level )
                {
                    return false;
                }

                if( _flag == 0 )
                {
                    return true;
                }

                if( (m_verboseFlag & _flag) == 0 )
                {
                    return false;
                }

                return true;
            };

        public:
            void log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count ) override
            {
                (void)_level;
                (void)_flag;
                (void)_count;

                message_error( "%s"
                    , _data
                );
            }

            void flush() override
            {
            }

        protected:
            EMessageLevel m_verboseLevel;
            uint32_t m_verboseFlag;
        };

        LOGGER_SERVICE()
            ->setVerboseLevel( LM_WARNING );

        LOGGER_SERVICE()
            ->registerLogger( new MyLogger );

        SERVICE_CREATE( CodecService );
        SERVICE_CREATE( DataService );
        SERVICE_CREATE( ConfigService );

        SERVICE_CREATE( ThreadSystem );

        SERVICE_CREATE( ThreadService );
        SERVICE_CREATE( MemoryService );
        SERVICE_CREATE( PluginSystem );
        SERVICE_CREATE( PluginService );

        SERVICE_CREATE( WindowsLayer );
        SERVICE_CREATE( FileService );

        PLUGIN_CREATE( Win32FileGroup );

        PLUGIN_CREATE( Zip );
        PLUGIN_CREATE( LZ4 );

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), ConstString::none(), Helper::emptyPath(), Helper::stringizeString( "dir" ) ) == false )
        {
            return false;
        }

        ConstString dev = Helper::stringizeString( "dev" );

        if( FILE_SERVICE()
            ->mountFileGroup( dev, ConstString::none(), Helper::emptyPath(), Helper::stringizeString( "dir" ) ) == false )
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
        ->loadPlugin( L"XmlCodecPlugin.dll" ) == false )
    {
        return 0;
    }

    Mengine::XmlDecoderInterfacePtr decoder = CODEC_SERVICE()
        ->createDecoderT<Mengine::XmlDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "xml2bin" ) );

    if( decoder == nullptr )
    {
        LOGGER_ERROR( "LoaderEngine::makeBin_ invalid create decoder xml2bin for %s"
            , fp_in.c_str()
            );

        return 0;
    }

    if( decoder->prepareData( nullptr ) == false )
    {
        LOGGER_ERROR( "LoaderEngine::makeBin_ invalid initialize decoder xml2bin for %s"
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