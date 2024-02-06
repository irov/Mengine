#include "DazzlePlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "DazzleInterface.h"

#include "DataflowDZZ.h"
#include "DazzleEffect.h"
#include "ResourceDazzleEffect.h"
#include "DazzleEffectPrototypeGenerator.h"

#if !defined(MENGINE_MASTER_RELEASE)
#   include "DazzleEffectConverterDZBToDZZ.h"
#endif

#include "Kernel/Logger.h"
#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/ConverterFactory.h"

#include "Config/StdMath.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Dazzle, Mengine::DazzlePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * s_dz_malloc( dz_size_t _size, dz_userdata_t _ud )
        {
            DZ_UNUSED( _ud );

            void * p = Helper::allocateMemory( _size, "dazzle" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * s_dz_realloc( void * _ptr, dz_size_t _size, dz_userdata_t _ud )
        {
            DZ_UNUSED( _ud );

            void * p = Helper::reallocateMemory( _ptr, _size, "dazzle" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_dz_free( const void * _ptr, dz_userdata_t _ud )
        {
            DZ_UNUSED( _ud );

            Helper::deallocateMemory( (void *)_ptr, "dazzle" );
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_dz_sqrtf( float _a, dz_userdata_t _ud )
        {
            DZ_UNUSED( _ud );

            float value = MENGINE_SQRTF( _a );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_dz_cosf( float _a, dz_userdata_t _ud )
        {
            DZ_UNUSED( _ud );

            float value = MENGINE_COSF( _a );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_dz_sinf( float _a, dz_userdata_t _ud )
        {
            DZ_UNUSED( _ud );

            float value = MENGINE_SINF( _a );

            return value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    DazzlePlugin::DazzlePlugin()
        : m_service( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DazzlePlugin::~DazzlePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DazzlePlugin::_initializePlugin()
    {
        dz_service_providers_t providers;
        providers.f_malloc = &Detail::s_dz_malloc;
        providers.f_realloc = &Detail::s_dz_realloc;
        providers.f_free = &Detail::s_dz_free;
        providers.f_sqrtf = &Detail::s_dz_sqrtf;
        providers.f_cosf = &Detail::s_dz_cosf;
        providers.f_sinf = &Detail::s_dz_sinf;

        dz_service_t * service;
        if( dz_service_create( &service, &providers, DZ_NULLPTR ) == DZ_FAILURE )
        {
            return false;
        }

        m_service = service;

        DazzleEffectPrototypeGeneratorPtr prototypeGenerator = Helper::makeFactorableUnique<DazzleEffectPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE );

        prototypeGenerator->setDazzleService( m_service );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "DazzleEffect" ), prototypeGenerator ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceDazzleEffect" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceDazzleEffect, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        CODEC_SERVICE()
            ->registerCodecExt( STRINGIZE_STRING_LOCAL( "dzz" ), STRINGIZE_STRING_LOCAL( "dazzle" ) );

#if !defined(MENGINE_MASTER_RELEASE)
        Helper::registerConverter<DazzleEffectConverterDZBToDZZ>( "dzb2dzz", MENGINE_DOCUMENT_FACTORABLE );
#endif

        PLUGIN_SERVICE_WAIT( DataServiceInterface, [this]()
        {
            DataflowDZZPtr dataflowDazzle = Helper::makeFactorableUnique<DataflowDZZ>( MENGINE_DOCUMENT_FACTORABLE );

            ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( archivator );

            dataflowDazzle->setArchivator( archivator );
            dataflowDazzle->setDazzleService( m_service );

            if( dataflowDazzle->initialize() == false )
            {
                return false;
            }

            VOCABULARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "dazzle" ), dataflowDazzle, MENGINE_DOCUMENT_FACTORABLE );

            return true;
        } );

        PLUGIN_SERVICE_LEAVE( DataServiceInterface, []()
        {
            DataflowInterfacePtr dataflowFE = VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "dazzle" ) );
            dataflowFE->finalize();
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzlePlugin::_finalizePlugin()
    {
#if !defined(MENGINE_MASTER_RELEASE)
        Helper::unregisterConverter( "dzb2dzz" );
#endif

        CODEC_SERVICE()
            ->removeCodecExt( STRINGIZE_STRING_LOCAL( "dzz" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "DazzleEffect" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceDazzleEffect" ), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzlePlugin::_destroyPlugin()
    {
        dz_service_destroy( m_service );
        m_service = nullptr;

        MENGINE_ASSERTION_ALLOCATOR( "dazzle" );
    }
    //////////////////////////////////////////////////////////////////////////
}