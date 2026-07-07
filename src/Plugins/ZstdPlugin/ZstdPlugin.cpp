#include "ZstdPlugin.h"

#include "ArchivatorZstd.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Zstd, Mengine::ZstdPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    ZstdPlugin::ZstdPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ZstdPlugin::~ZstdPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZstdPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nozstd" ) == true )
        {
            return false;
        }

        bool ZstdPlugin_Available = CONFIG_VALUE_BOOLEAN( "ZstdPlugin", "Available", true );

        if( ZstdPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ZstdPlugin::_initializePlugin()
    {
        VOCABULARY_SET( ArchivatorInterface, STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "zstd" ), Helper::makeFactorableUnique<ArchivatorZstd>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ZstdPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "zstd" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ZstdPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "zstd" );
    }
    //////////////////////////////////////////////////////////////////////////
}
