#include "LZ4Plugin.h"

#include "Interface/VocabularyServiceInterface.h"

#include "ArchivatorLZ4.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"

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
        VOCABULARY_SET( ArchivatorInterface, STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ), Helper::makeFactorableUnique<ArchivatorLZ4>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LZ4Plugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );
    }
}