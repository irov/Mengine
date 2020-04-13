#include "OzzAnimationPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/AllocatorServiceInterface.h"

#include "ResourceOzzMesh.h"
#include "ResourceOzzSkeleton.h"
#include "ResourceOzzAnimation.h"

#include "NodeOzzAnimation.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "OzzScriptEmbedding.h"
#endif

#include "Kernel/ConstStringHelper.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ScriptablePrototypeGenerator.h"
#include "Kernel/Document.h"

#include "Config/StdString.h"

#include "ozz/base/maths/math_ex.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( OzzAnimation, Mengine::OzzAnimationPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OzzAllocator
        : public ozz::memory::Allocator
    {
    public:
        OzzAllocator()
        {
        }

        ~OzzAllocator() override
        {
        }

    protected:
        struct Header
        {
            void * unaligned;
            size_t size;
        };

    protected:
        void * Allocate( size_t _size, size_t _alignment ) override
        {
            // Allocates enough memory to store the header + required alignment space.
            const size_t to_allocate = _size + sizeof( Header ) + _alignment - 1;
            char * unaligned = reinterpret_cast<char *>(ALLOCATOR_SERVICE()
                ->malloc( to_allocate, "ozz" ));
            if( !unaligned )
            {
                return NULL;
            }
            char * aligned = ozz::math::Align( unaligned + sizeof( Header ), _alignment );
            assert( aligned + _size <= unaligned + to_allocate );  // Don't overrun.
            // Set the header
            Header * header = reinterpret_cast<Header *>(aligned - sizeof( Header ));
            assert( reinterpret_cast<char *>(header) >= unaligned );
            header->unaligned = unaligned;
            header->size = _size;
            return aligned;
        }

        void Deallocate( void * _block ) override
        {
            if( _block )
            {
                Header * header = reinterpret_cast<Header *>(
                    reinterpret_cast<char *>(_block) - sizeof( Header ));
                ALLOCATOR_SERVICE()
                    ->free( header->unaligned, "ozz" );
            }
        }

        void * Reallocate( void * _block, size_t _size, size_t _alignment ) override
        {
            void * new_block = this->Allocate( _size, _alignment );
            // Copies and deallocate the old memory block.
            if( _block )
            {
                Header * old_header = reinterpret_cast<Header *>(
                    reinterpret_cast<char *>(_block) - sizeof( Header ));
                MENGINE_MEMCPY( new_block, _block, old_header->size );
                ALLOCATOR_SERVICE()
                    ->free( old_header->unaligned, "ozz" );
            }
            return new_block;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    OzzAnimationPlugin::OzzAnimationPlugin()
        : m_allocatorOld( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OzzAnimationPlugin::~OzzAnimationPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OzzAnimationPlugin::_initializePlugin()
    {
        ozz::memory::Allocator * allocator = ozz::memory::default_allocator();

        m_allocatorOld = allocator;

        OzzAllocator * new_allocator = Helper::newT<OzzAllocator>();

        ozz::memory::SetDefaulAllocator( new_allocator );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, [this]()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "OzzScriptEmbedding" ), Helper::makeFactorableUnique<OzzScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        } );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "OzzScriptEmbedding" ) );
        } );
#endif

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzSkeleton" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceOzzSkeleton, 16>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzMesh" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceOzzMesh, 16>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzAnimation" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceOzzAnimation, 16>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Sampler" ), STRINGIZE_STRING_LOCAL( "SamplerOzzAnimation" ), Helper::makeFactorableUnique<ScriptablePrototypeGenerator<SamplerOzzAnimation, 16>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeOzzAnimation" ), Helper::makeFactorableUnique<NodePrototypeGenerator<NodeOzzAnimation, 16>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzAnimationPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzSkeleton" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzMesh" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzAnimation" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Sampler" ), STRINGIZE_STRING_LOCAL( "SamplerOzzAnimation" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeOzzAnimation" ) );

        OzzAllocator * allocator = static_cast<OzzAllocator *>(ozz::memory::default_allocator());

        Helper::deleteT( allocator );

        ozz::memory::SetDefaulAllocator( m_allocatorOld );
        m_allocatorOld = nullptr;
    }
}