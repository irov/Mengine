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
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/SecureServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/LoggerServiceInterface.h"


#include "Plugins/XmlToBinPlugin/XmlToBinInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/LoggerBase.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/PathHelper.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/Ravingcode.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/FileStreamHelper.h"

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
//////////////////////////////////////////////////////////////////////////
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
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( VocabularyService );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( SecureService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( Platform );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    static bool initializeEngine()
    {
        ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_CREATE( DocumentService, nullptr );
        SERVICE_CREATE( OptionsService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( FactoryService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( UnicodeSystem, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( StringizeService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ArchiveService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( NotificationService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( LoggerService, MENGINE_DOCUMENT_FUNCTION );

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
            void log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _count ) override
            {
                MENGINE_UNUSED( _level );
                MENGINE_UNUSED( _flag );
                MENGINE_UNUSED( _color );
                MENGINE_UNUSED( _count );

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
            ->registerLogger( Helper::makeFactorableUnique<MyLogger>( MENGINE_DOCUMENT_FUNCTION ) );

        SERVICE_CREATE( CodecService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( DataService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( ConfigService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( VocabularyService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( ThreadSystem, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( ThreadService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( MemoryService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( PluginService, MENGINE_DOCUMENT_FUNCTION );

        SERVICE_CREATE( FileService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( SecureService, MENGINE_DOCUMENT_FUNCTION );
        SERVICE_CREATE( Platform, MENGINE_DOCUMENT_FUNCTION );

        PLUGIN_CREATE( Win32FileGroup, MENGINE_DOCUMENT_FUNCTION );

        PLUGIN_CREATE( Zip, MENGINE_DOCUMENT_FUNCTION );
        PLUGIN_CREATE( LZ4, MENGINE_DOCUMENT_FUNCTION );

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
        
    Mengine::WString in = parse_kwds( lpCmdLine, L"--in", Mengine::WString() );
    Mengine::WString out = parse_kwds( lpCmdLine, L"--out", Mengine::WString() );

    if( in.empty() == true )
    {
        message_error( "not found 'in' param"
        );

        return 0;
    }

    if( out.empty() == true )
    {
        message_error( "not found 'out' param"
        );

        return 0;
    }

    try
    {
        if( Mengine::initializeEngine() == false )
        {
            message_error( "invalid initialize" );

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

    Mengine::FilePath fp_in = Mengine::Helper::unicodeToFilePath( in );
    Mengine::FilePath fp_out = Mengine::Helper::unicodeToFilePath( out );

    const Mengine::FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
        ->getFileGroup( Mengine::ConstString::none() );

    Mengine::MemoryInterfacePtr memory = Mengine::Helper::createMemoryFile( fileGroup, fp_in, false, false, MENGINE_DOCUMENT_FUNCTION );

    void * memoryBuffer = memory->getBuffer();
    size_t memorySize = memory->getSize();

    Mengine::HashType sequreHash = SECURE_SERVICE()
        ->getSequreHash();

    Mengine::Helper::ravingcode( (uint32_t)sequreHash, memoryBuffer, memorySize, memoryBuffer );

    Mengine::OutputStreamInterfacePtr outputStream = Mengine::Helper::openOutputStreamFile( fileGroup, fp_out, MENGINE_DOCUMENT_FUNCTION );

    outputStream->write( "RGCD", 4 );
    outputStream->write( memoryBuffer, memorySize );

    return 0;
}