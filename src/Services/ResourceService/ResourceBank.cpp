#include "ResourceBank.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionMainThreadGuard.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ThreadGuardScope.h"

#include <algorithm>

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

#ifdef MENGINE_DEBUG
        for( const HashtableGroupResources::value_type & value : m_resourcesGroup )
        {
            const ResourcePtr & resource = value.element;

            uint32_t refcount = resource->getCompileReferenceCount();

            if( refcount != 0 )
            {
                LOGGER_WARNING( "resource '%s' type '%s' group '%s' refcount %d"
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
    ResourcePointer ResourceBank::createResource( const ConstString & _locale, const ConstString & _groupName, const ConstString & _name, const ConstString & _type, bool _keep, Resource ** const _override, const DocumentPtr & _doc )
    {
        MENGINE_THREAD_GUARD_SCOPE( ResourceBank, this, "ResourceBank::foreachResources" );

        LOGGER_INFO( "resource", "create resource '%s' group '%s' type '%s'"
            , _name.c_str()
            , _groupName.c_str()
            , _type.c_str()
        );

        ResourcePtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), _type, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "invalid generate resource locale '%s' group '%s' name '%s' type '%s' doc '%s'"
            , _locale.c_str()
            , _groupName.c_str()
            , _name.c_str()
            , _type.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        resource->setResourceBank( this );
        resource->setLocale( _locale );
        resource->setGroupName( _groupName );
        resource->setName( _name );

        resource->setKeep( _keep );

        if( _keep == true )
        {
            IntrusivePtrBase::intrusive_ptr_add_ref( resource.get() );
        }

        if( _name == ConstString::none() )
        {
            return resource;
        }

        resource->setMapping( true );

        m_resources.change( _name, resource );

        if( _groupName != ConstString::none() )
        {
            ResourcePtrView prev_resource = m_resourcesGroup.change( _groupName, _name, resource );

            if( prev_resource != nullptr )
            {
                prev_resource->finalize();
                prev_resource->setMapping( false );

                bool prev_keep = prev_resource->isKeep();

                if( prev_keep == true )
                {
                    IntrusivePtrBase::intrusive_ptr_dec_ref( prev_resource.get() );
                }
            }

            if( _override != nullptr )
            {
                *_override = prev_resource.get();
            }
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceBank::removeResource( const ResourcePtr & _resource )
    {
        Resource * resource = _resource.get();

        this->destroyResource( resource );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceBank::destroyResource( Resource * _resource )
    {
        MENGINE_THREAD_GUARD_SCOPE( ResourceBank, this, "ResourceBank::foreachResources" );

        MENGINE_ASSERTION_FATAL( _resource->getCompileReferenceCount() == 0 );

        MENGINE_ASSERTION_FATAL( _resource->isMapping() == false || m_resources.exist( _resource->getName() ) == true, "resource '%s' type '%s' not found (maybe already remove)"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
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
            IntrusivePtrBase::intrusive_ptr_dec_ref( _resource );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & ResourceBank::getResourceReference( const ConstString & _groupName, const ConstString & _name ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        const ResourcePtr & group_resource = m_resourcesGroup.find( _groupName, _name );

        if( group_resource != nullptr )
        {
            return group_resource;
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

        const ResourcePtr & group_resource = m_resourcesGroup.find( _groupName, _name );

        if( group_resource != nullptr )
        {
            if( _resource != nullptr )
            {
                *_resource = group_resource;
            }

            return true;
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
        MENGINE_THREAD_GUARD_SCOPE( ResourceBank, this, "ResourceBank::foreachResources" );

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