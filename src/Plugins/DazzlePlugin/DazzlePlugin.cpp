#include "DazzlePlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "DazzleInterface.h"

#include "DazzleEffect.h"
#include "ResourceDazzleEffect.h"

#include "Kernel/Logger.h"
#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/AssertionAllocator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Dazzle, Mengine::DazzlePlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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

        float value = MT_sqrtf( _a );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    static float s_dz_cosf( float _a, dz_userdata_t _ud )
    {
        DZ_UNUSED( _ud );

        float value = MT_cosf( _a );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    static float s_dz_sinf( float _a, dz_userdata_t _ud )
    {
        DZ_UNUSED( _ud );

        float value = MT_sinf( _a );

        return value;
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
        LOGGER_INFO( "dazzle", "Initializing Dazzle..." );

        dz_service_providers_t providers;
        providers.f_malloc = &s_dz_malloc;
        providers.f_realloc = &s_dz_realloc;
        providers.f_free = &s_dz_free;
        providers.f_sqrtf = &s_dz_sqrtf;
        providers.f_cosf = &s_dz_cosf;
        providers.f_sinf = &s_dz_sinf;

        dz_service_t * service;
        if( dz_service_create( &service, &providers, DZ_NULLPTR ) == DZ_FAILURE )
        {
            return false;
        }

        m_service = service;

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "DazzleEffect" ), Helper::makeFactorableUnique<NodePrototypeGenerator<DazzleEffect, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceDazzleEffect" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceDazzleEffect, 128>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzlePlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "DazzleEffect" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceDazzleEffect" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DazzlePlugin::_destroyPlugin()
    {
        dz_service_destroy( m_service );
        m_service = nullptr;

        MENGINE_ASSERTION_ALLOCATOR( "dazzle" );
    }
}