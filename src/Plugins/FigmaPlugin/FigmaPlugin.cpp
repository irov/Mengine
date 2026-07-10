#include "FigmaPlugin.h"

#include "DataflowFigma.h"
#include "Figma.h"
#include "ResourceFigma.h"
#include "ResourceFigmaValidator.h"

#if defined(MENGINE_PLUGIN_JSON)
#include "JSONLoaderResourceFigma.h"
#endif

#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"
#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "Interface/DataServiceInterface.h"

#include "Kernel/AssertionAllocator.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/VocabularyHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Figma, Mengine::FigmaPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FigmaPlugin::FigmaPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FigmaPlugin::~FigmaPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nofigma" ) == true )
        {
            return false;
        }

        bool FigmaPlugin_Available = CONFIG_VALUE_BOOLEAN( "FigmaPlugin", "Available", true );

        if( FigmaPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaPlugin::_initializePlugin()
    {
        if( Helper::addNodePrototype<Figma, 32>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addResourcePrototype<ResourceFigma, 32>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( DataServiceInterface, [this]()
        {
            DataflowFigmaPtr dataflowFigma = Helper::makeFactorableUnique<DataflowFigma>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( dataflowFigma, "invalid create 'DataflowFigma'" );

            if( dataflowFigma->initialize() == false )
            {
                return false;
            }

            VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "figma" ), dataflowFigma, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( DataServiceInterface, []()
        {
            DataflowInterfacePtr dataflow = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "figma" ) );
            dataflow->finalize();
        } );

        PLUGIN_SERVICE_WAIT( ResourcePrefetcherServiceInterface, [this]()
        {
            ResourcePrefetcherInterfacePtr resourcePrefetcherDataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcherType" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );

            VOCABULARY_SET( ResourcePrefetcherInterface, STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceFigma::getFactorableType(), resourcePrefetcherDataflow, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourcePrefetcherServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), ResourceFigma::getFactorableType() );
        } );

        PLUGIN_SERVICE_WAIT( ResourceValidateServiceInterface, [this]()
        {
            VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), ResourceFigma::getFactorableType(), Helper::makeFactorableUnique<ResourceFigmaValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( ResourceValidateServiceInterface, []()
        {
            VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), ResourceFigma::getFactorableType() );
        } );

#if defined(MENGINE_PLUGIN_JSON)
        VOCABULARY_SET( JSONLoaderInterface, STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceFigma::getFactorableType(), Helper::makeFactorableUnique<JSONLoaderResourceFigma>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaPlugin::_finalizePlugin()
    {
#if defined(MENGINE_PLUGIN_JSON)
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "JSONLoader" ), ResourceFigma::getFactorableType() );
#endif

        Helper::removeResourcePrototype<ResourceFigma>();
        Helper::removeNodePrototype<Figma>();
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "figma" );
    }
    //////////////////////////////////////////////////////////////////////////
}
