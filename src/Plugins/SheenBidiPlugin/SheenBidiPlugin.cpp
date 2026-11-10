#include "SheenBidiPlugin.h"

#include "SheenBidiTextAlgorithm.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/VocabularyHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( SheenBidi, Mengine::SheenBidiPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SheenBidiPlugin::SheenBidiPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SheenBidiPlugin::~SheenBidiPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SheenBidiPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nosheenbidi" ) == true )
        {
            return false;
        }

        bool available = CONFIG_VALUE_BOOLEAN( "SheenBidiPlugin", "Available", true );

        if( available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SheenBidiPlugin::_initializePlugin()
    {
        TextAlgorithmInterfacePtr algorithm = Helper::makeFactorableUnique<SheenBidiTextAlgorithm>( MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( TextAlgorithmInterface, STRINGIZE_STRING_LOCAL( "TextAlgorithm" ), STRINGIZE_STRING_LOCAL( "Unicode" ), algorithm, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SheenBidiPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "TextAlgorithm" ), STRINGIZE_STRING_LOCAL( "Unicode" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
