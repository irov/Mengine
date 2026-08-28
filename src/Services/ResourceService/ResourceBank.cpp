#include "ResourceBank.h"

#include "Kernel/Resource.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionResourceUID.h"
#include "Kernel/AssertionMainThreadGuard.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdIntTypes.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceBank::ResourceBank()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceBank::~ResourceBank()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceBank::initialize( const ThreadMutexInterfacePtr & _mutex, uint32_t _reserved )
    {
        m_mutex = _mutex;

        m_resources.reserve( _reserved );
        m_resourcesGroup.reserve( _reserved );
        m_resourceUIDs.reserve( _reserved );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceBank::finalize()
    {
        for( const HashtableGroupResources::value_type & value : m_resourcesGroup )
        {
            const ResourcePtrView & resource = value.element;

            bool precompile = resource->isPrecompile();

            if( precompile == true )
            {
                resource->release();
            }
        }

#if defined(MENGINE_DEBUG)
        for( const HashtableGroupResources::value_type & value : m_resourcesGroup )
        {
            const ResourcePtr & resource = value.element;

            uint32_t refcount = resource->getCompileReferenceCount();

            if( refcount != 0 )
            {
                LOGGER_WARNING( "resource '%s' type '%s' group '%s' refcount [%u]"
                    , resource->getName().c_str()
                    , Helper::getFactorableType( resource ).c_str()
                    , resource->getGroupName().c_str()
                    , refcount
                );
            }
        }
#endif

        for( const HashtableGroupResources::value_type & value : m_resourcesGroup )
        {
            ResourcePtrView resource = value.element;

            resource->finalize();

            bool keep = resource->isKeep();

            if( keep == true )
            {
                IntrusivePtrBase::intrusive_ptr_dec_ref( resource.get() );
            }
        }

        m_resources.clear();
        m_resourcesGroup.clear();
        m_resourceUIDs.clear();

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceBank::finalizeKeepResource()
    {
        for( const HashtableGroupResources::value_type & value : m_resourcesGroup )
        {
            Resource * resource = value.element.get();

            bool precompile = resource->isPrecompile();

            if( precompile == true )
            {
                resource->release();
            }

            resource->setPrecompile( false );

            bool keep = resource->isKeep();

            if( keep == true )
            {
                m_resourcesGroup.erase( value );
                m_resources.erase( value.key2 );

                const ResourceUID & uid = resource->getResourceUID();

                if( uid != INVALID_RESOURCE_UID )
                {
                    m_resourceUIDs.erase( uid );
                }

                resource->finalize();

                IntrusivePtrBase::intrusive_ptr_dec_ref( resource );
            }
        }

        for( const HashtableResources::value_type & value : m_resources )
        {
            Resource * resource = value.element.get();

            bool precompile = resource->isPrecompile();

            if( precompile == true )
            {
                resource->release();
            }

            resource->setPrecompile( false );

            bool keep = resource->isKeep();

            if( keep == true )
            {
                m_resources.erase( value.key );

                const ResourceUID & uid = resource->getResourceUID();

                if( uid != INVALID_RESOURCE_UID )
                {
                    m_resourceUIDs.erase( uid );
                }

                resource->finalize();

                IntrusivePtrBase::intrusive_ptr_dec_ref( resource );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePointer ResourceBank::createResource( const ResourceCook & _cook, Resource ** const _override, const DocumentInterfacePtr & _doc )
    {
        MENGINE_THREAD_GUARD_SCOPE( ResourceBank, this );

        //ToDo

        MENGINE_ASSERTION_FATAL( _cook.uid == INVALID_RESOURCE_UID || _cook.name != ConstString::none(), "resource UID '%08" MENGINE_PRIx64 "-%04" MENGINE_PRIx64 "-%04" MENGINE_PRIx64 "-%04" MENGINE_PRIx64 "-%012" MENGINE_PRIx64 "' requires a resource name"
            , _cook.uid.high >> 32
            , (_cook.uid.high >> 16) & MENGINE_UINT64_C(0xFFFF)
            , _cook.uid.high & MENGINE_UINT64_C(0xFFFF)
            , _cook.uid.low >> 48
            , _cook.uid.low & MENGINE_UINT64_C(0xFFFFFFFFFFFF)
        );

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
        if( _cook.uid != INVALID_RESOURCE_UID )
        {
            MENGINE_ASSERTION_RESOURCE_UID( _cook.uid, _cook.name, _cook.groupName );
        }
#endif

        LOGGER_INFO( "resource", "create resource '%s' group '%s' type '%s' keep [%s]"
            , _cook.name.c_str()
            , _cook.groupName.c_str()
            , _cook.type.c_str()
            //, _locale.c_str()
            , (_cook.keep == true ? "true" : "false")
        );

        const ConstString & categoryType = Resource::getFactorableType();
        const ConstString & factorableType = _cook.type;

        ResourcePtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( categoryType, factorableType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC_SAFE( resource, nullptr, "invalid generate resource name '%s' type '%s' group '%s' doc '%s'"
            , _cook.name.c_str()
            , _cook.type.c_str()
            //, _locales.c_str()
            , _cook.groupName.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        resource->setResourceBank( this );
        resource->setResourceUID( _cook.uid );
        resource->setLocales( _cook.locales );
        resource->setGroupName( _cook.groupName );
        resource->setName( _cook.name );

        resource->setKeep( _cook.keep );

        if( _cook.keep == true )
        {
            IntrusivePtrBase::intrusive_ptr_add_ref( resource.get() );
        }

        if( _cook.name == ConstString::none() )
        {
            return resource;
        }

        resource->setMapping( true );

        ResourcePtrView prev_resource = m_resources.change( _cook.name, resource );

        if( _cook.groupName != ConstString::none() )
        {
            prev_resource = m_resourcesGroup.change( _cook.groupName, _cook.name, resource );
        }

        if( prev_resource != nullptr )
        {
            this->overrideResource( prev_resource, _override );
        }

        if( _cook.uid != INVALID_RESOURCE_UID )
        {
            m_resourceUIDs.emplace( _cook.uid, resource );
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceBank::overrideResource( const ResourcePtrView & _resource, Resource ** const _override )
    {
        const ResourceUID & uid = _resource->getResourceUID();

        if( uid != INVALID_RESOURCE_UID )
        {
            m_resourceUIDs.erase( uid );
        }

        _resource->finalize();
        _resource->setMapping( false );

        bool prev_keep = _resource->isKeep();

        if( prev_keep == true )
        {
            if( _override != nullptr && IntrusivePtrBase::intrusive_ptr_get_ref( _resource.get() ) != 1 )
            {
                *_override = _resource.get();
            }

            IntrusivePtrBase::intrusive_ptr_dec_ref( _resource.get() );
        }
        else
        {
            if( _override != nullptr )
            {
                *_override = _resource.get();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceBank::removeResource( const ResourcePtr & _resource )
    {
        MENGINE_THREAD_GUARD_SCOPE( ResourceBank, this );

        MENGINE_ASSERTION_FATAL( _resource->getCompileReferenceCount() == 0, "remove resource '%s' type '%s' has reference count: %u"
            , _resource->getName().c_str()
            , Helper::getFactorableType( _resource ).c_str()
            , _resource->getCompileReferenceCount()
        );

        const ConstString & name = _resource->getName();
        const ResourceUID & uid = _resource->getResourceUID();

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
        bool nameMapping = name != ConstString::none() && m_resources.exist( name ) == true;
        bool uidMapping = uid != INVALID_RESOURCE_UID && m_resourceUIDs.exist( uid ) == true;

        MENGINE_ASSERTION_FATAL( _resource->isMapping() == false || nameMapping == true || uidMapping == true, "resource '%s' type '%s' not found (maybe already remove)"
            , _resource->getName().c_str()
            , Helper::getFactorableType( _resource ).c_str()
        );
#endif

        //ToDo

        LOGGER_INFO( "resource", "remove resource '%s' group '%s' type '%s'"
            , _resource->getName().c_str()
            , _resource->getGroupName().c_str()
            , Helper::getFactorableType( _resource ).c_str()
            //, _resource->getLocale().c_str()
        );

        _resource->finalize();

        const ConstString & groupName = _resource->getGroupName();
        bool keep = _resource->isKeep();

        if( _resource->isMapping() == true )
        {
            if( name != ConstString::none() )
            {
                if( groupName != ConstString::none() )
                {
                    m_resourcesGroup.erase( groupName, name );
                }

                m_resources.erase( name );
            }

            if( uid != INVALID_RESOURCE_UID )
            {
                m_resourceUIDs.erase( uid );
            }
        }

        if( keep == true )
        {
            IntrusivePtrBase::intrusive_ptr_dec_ref( _resource.get() );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & ResourceBank::getResource( const ConstString & _groupName, const ConstString & _name ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        if( _groupName != ConstString::none() )
        {
            const ResourcePtr & group_resource = m_resourcesGroup.find( _groupName, _name );

            if( group_resource != nullptr )
            {
                return group_resource;
            }
        }

        const ResourcePtr & global_resource = m_resources.find( _name );

        MENGINE_ASSERTION_MEMORY_PANIC( global_resource, "resource '%s' does not exist"
            , _name.c_str()
        );

        return global_resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & ResourceBank::getResource( const ResourceUID & _uid ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        const ResourcePtr & resource = m_resourceUIDs.find( _uid );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "resource UID '%08" MENGINE_PRIx64 "-%04" MENGINE_PRIx64 "-%04" MENGINE_PRIx64 "-%04" MENGINE_PRIx64 "-%012" MENGINE_PRIx64 "' does not exist"
            , _uid.high >> 32
            , (_uid.high >> 16) & MENGINE_UINT64_C(0xFFFF)
            , _uid.high & MENGINE_UINT64_C(0xFFFF)
            , _uid.low >> 48
            , _uid.low & MENGINE_UINT64_C(0xFFFFFFFFFFFF)
        );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceBank::hasResource( const ConstString & _groupName, const ConstString & _name, bool _onlyGroup, ResourcePtr * const _resource ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        if( _groupName != ConstString::none() )
        {
            const ResourcePtr & group_resource = m_resourcesGroup.find( _groupName, _name );

            if( group_resource != nullptr )
            {
                if( _resource != nullptr )
                {
                    *_resource = group_resource;
                }

                return true;
            }
        }

        if( _onlyGroup == true )
        {
            return false;
        }

        const ResourcePtr & globa_resource = m_resources.find( _name );

        if( globa_resource == nullptr )
        {
            return false;
        }

        if( _resource != nullptr )
        {
            *_resource = globa_resource;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceBank::hasResource( const ResourceUID & _uid, ResourcePtr * const _resource ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        const ResourcePtr & resource = m_resourceUIDs.find( _uid );

        if( resource == nullptr )
        {
            return false;
        }

        if( _resource != nullptr )
        {
            *_resource = resource;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceBank::foreachResources( const LambdaResourceView & _lambda ) const
    {
        MENGINE_THREAD_GUARD_SCOPE( ResourceBank, this );

        for( const HashtableResources::value_type & value : m_resources )
        {
            const ResourcePtrView & resource = value.element;

            _lambda( resource );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceBank::HashtableResources & ResourceBank::getResources() const
    {
        return m_resources;
    }
    //////////////////////////////////////////////////////////////////////////
}
