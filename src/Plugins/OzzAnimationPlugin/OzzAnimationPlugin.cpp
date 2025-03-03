#include "OzzAnimationPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "ResourceOzzMesh.h"
#include "ResourceOzzSkeleton.h"
#include "ResourceOzzAnimation.h"

#include "NodeOzzAnimation.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#include "OzzScriptEmbedding.h"
#endif

#include "Kernel/ConstStringHelper.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ObjectPrototypeGenerator.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/PrototypeHelper.h"

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
            char * unaligned = reinterpret_cast<char *>(Helper::allocateMemory( to_allocate, "ozz" ));
            if( unaligned == nullptr )
            {
                return nullptr;
            }
            char * aligned = ozz::Align( unaligned + sizeof( Header ), _alignment );
            assert( aligned + _size <= unaligned + to_allocate );  // Don't overrun.
            // Set the header
            Header * header = reinterpret_cast<Header *>(aligned - sizeof( Header ));
            assert( reinterpret_cast<char *>(header) >= unaligned );
            header->unaligned = unaligned;
            header->size = _size;
            // Allocation's succeeded.
            return aligned;
        }

        void Deallocate( void * _block ) override
        {
            if( _block != nullptr )
            {
                Header * header = reinterpret_cast<Header *>(
                    reinterpret_cast<char *>(_block) - sizeof( Header ));
                Helper::deallocateMemory( header->unaligned, "ozz" );
                // Deallocation completed.
            }
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

#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( OzzScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<OzzScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EJECTING, this, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( OzzScriptEmbedding::getFactorableType() );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        if( Helper::addResourcePrototype<ResourceOzzSkeleton, 16>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addResourcePrototype<ResourceOzzMesh, 16>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( Helper::addResourcePrototype<ResourceOzzAnimation, 16>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Sampler" ), SamplerOzzAnimation::getFactorableType(), Helper::makeFactorableUnique<ObjectPrototypeGenerator<SamplerOzzAnimation, 16>>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( Helper::addNodePrototype<NodeOzzAnimation, 16>( MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzAnimationPlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        Helper::removeResourcePrototype<ResourceOzzSkeleton>();
        Helper::removeResourcePrototype<ResourceOzzMesh>();
        Helper::removeResourcePrototype<ResourceOzzAnimation>();
        Helper::removePrototype<SamplerOzzAnimation>( STRINGIZE_STRING_LOCAL( "Sampler" ) );
        Helper::removeNodePrototype<NodeOzzAnimation>();

        OzzAllocator * allocator = static_cast<OzzAllocator *>(ozz::memory::default_allocator());

        Helper::deleteT( allocator );

        ozz::memory::SetDefaulAllocator( m_allocatorOld );
        m_allocatorOld = nullptr;

        MENGINE_ASSERTION_ALLOCATOR( "ozz" );
    }
    //////////////////////////////////////////////////////////////////////////
}