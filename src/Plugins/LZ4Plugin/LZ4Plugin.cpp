#include "LZ4Plugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"

#include "ArchivatorLZ4.h"

#include "Kernel/FactorableUnique.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( LZ4, Mengine::LZ4Plugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LZ4Plugin::LZ4Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LZ4Plugin::~LZ4Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LZ4Plugin::_initializePlugin()
    {
        ARCHIVE_SERVICE()
            ->registerArchivator( STRINGIZE_STRING_LOCAL( "lz4" ), Helper::makeFactorableUnique<ArchivatorLZ4>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LZ4Plugin::_finalizePlugin()
    {
        ARCHIVE_SERVICE()
            ->unregisterArchivator( STRINGIZE_STRING_LOCAL( "lz4" ) );
    }
}