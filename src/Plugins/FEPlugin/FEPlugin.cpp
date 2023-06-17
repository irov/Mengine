#include "FEPlugin.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/DataServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/PixelFormat.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionAllocator.h"

#include "FEDataflow.h"
#include "FEFontEffectFile.h"
#include "FEFontEffectCustom.h"

#include "fe/fe.h"

//////////////////////////////////////////////////////////////////////////
void * _fe_alloc( size_t _size )
{
    void * p = Mengine::Helper::allocateMemory( _size, "fe" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
void _fe_free( void * _ptr )
{
    Mengine::Helper::deallocateMemory( _ptr, "fe" );
}
//////////////////////////////////////////////////////////////////////////
void _debug_image_created( fe_image * )
{
}
//////////////////////////////////////////////////////////////////////////
void _debug_image_deleted( fe_image * )
{
}
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( FE, Mengine::FEPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FEPlugin::FEPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FEPlugin::~FEPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FEPlugin::_initializePlugin()
    {
        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FEFile" ), Helper::makeFactorableUnique<DefaultPrototypeGenerator<FEFontEffectFile, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FECustom" ), Helper::makeFactorableUnique<DefaultPrototypeGenerator<FEFontEffectCustom, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        PLUGIN_SERVICE_WAIT( DataServiceInterface, [this]()
        {
            FEDataflowPtr dataflowFE = Helper::makeFactorableUnique<FEDataflow>( MENGINE_DOCUMENT_FACTORABLE );

            if( dataflowFE->initialize() == false )
            {
                return false;
            }

            VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ), dataflowFE, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( DataServiceInterface, []()
        {
            DataflowInterfacePtr dataflowFE = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );
            dataflowFE->finalize();
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FEPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FEFile" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FECustom" ), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void FEPlugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "fe" );
    }
    //////////////////////////////////////////////////////////////////////////
}
