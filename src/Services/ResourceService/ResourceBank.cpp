#include "ResourceBank.h"

#include "Kernel/Resource.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionMainThreadGuard.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadGuardScope.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/Algorithm.h"

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
                    , resource->getType().c_str()
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

        LOGGER_INFO( "resource", "create resource '%s' group '%s' type '%s' keep [%s]"
            , _cook.name.c_str()
            , _cook.groupName.c_str()
            , _cook.type.c_str()
            //, _locale.c_str()
            , (_cook.keep == true ? "true" : "false")
        );

        const ConstString & factorableType = Resource::getFactorableType();

        ResourcePtr resource = Helper::generatePrototype( factorableType, _cook.type, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC_SAFE( resource, nullptr, "invalid generate resource name '%s' type '%s' group '%s' doc '%s'"
            , _cook.name.c_str()
            , _cook.type.c_str()
            //, _locales.c_str()
            , _cook.groupName.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        resource->setResourceBank( this );
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

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceBank::overrideResource( const ResourcePtrView & _resource, Resource ** const _override )
    {
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
            , _resource->getType().c_str()
            , _resource->getCompileReferenceCount()
        );

        MENGINE_ASSERTION_FATAL( _resource->isMapping() == false || m_resources.exist( _resource->getName() ) == true, "resource '%s' type '%s' not found (maybe already remove)"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
        );

        //ToDo

        LOGGER_INFO( "resource", "remove resource '%s' group '%s' type '%s'"
            , _resource->getName().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getType().c_str()
            //, _resource->getLocale().c_str()
        );

        _resource->finalize();

        const ConstString & groupName = _resource->getGroupName();
        const ConstString & name = _resource->getName();
        bool keep = _resource->isKeep();

        if( _resource->isMapping() == true )
        {
            if( groupName != ConstString::none() )
            {
                m_resourcesGroup.erase( groupName, name );
            }

            m_resources.erase( name );
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