#include "ZipPlugin.h"

#include "Interface/FileServiceInterface.h"

#include "FileGroupZip.h"
#include "ArchivatorZip.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Zip, Mengine::ZipPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ZipPlugin::ZipPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ZipPlugin::~ZipPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nozip" ) == true )
        {
            return false;
        }

        bool ZipPlugin_Available = CONFIG_VALUE_BOOLEAN( "ZipPlugin", "Available", true );

        if( ZipPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZipPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "zip" ), Helper::makeFactoryDefault<FileGroupZip>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        ArchivatorInterfacePtr archivator = Helper::makeFactorableUnique<ArchivatorZip>( MENGINE_DOCUMENT_FACTORABLE );

        VOCABULARY_SET( ArchivatorInterface, STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "zip" ), archivator, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ZipPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "zip" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "zip" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ZipPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "zip" );
    }
    //////////////////////////////////////////////////////////////////////////
}