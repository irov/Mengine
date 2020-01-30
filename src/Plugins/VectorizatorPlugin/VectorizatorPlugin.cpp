#include "VectorizatorPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Vectorizator.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Vectorizator, Mengine::VectorizatorPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VectorizatorPlugin::VectorizatorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VectorizatorPlugin::~VectorizatorPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VectorizatorPlugin::_initializePlugin()
    {
        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Vectorizator" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Vectorizator, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VectorizatorPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Vectorizator" ) );
    }
}