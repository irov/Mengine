#include "ResourceBank.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionMainThreadGuard.h"
#include "Kernel/ThreadMutexScope.h"

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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceBank::finalize()
    {
        for( const HashtableResources::value_type & value : m_resources )
        {
            const ResourcePtrView & resource = value.element;

            bool precompile = resource->isPrecompile();

            if( precompile == true )
            {
                resource->release();
            }
        }

#ifdef MENGINE_DEBUG
        for( const HashtableResources::value_type & value : m_resources )
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

        for( const HashtableResources::value_type & value : m_resources )
        {
            const ResourcePtrView & resource = value.element;
       
            resource->setResourceBank( nullptr );
            resource->finalize();

            bool keep = resource->isKeep();

            if( keep == true )
            {
                IntrusivePtrBase::intrusive_ptr_dec_ref( resource.get() );
            }
        }

        m_resources.clear();

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceBank::finalizeKeepResource()
    {
        for( const HashtableResources::value_type & value : m_resources )
        {
            const ResourcePtrView & resource = value.element;

            bool precompile = resource->isPrecompile();

            if( precompile == true )
            {
                resource->release();
            }

            resource->setPrecompile( false );

            bool keep = resource->isKeep();

            if( keep == true )
            {
                resource->setResourceBank( nullptr );
                resource->finalize();

                IntrusivePtrBase::intrusive_ptr_dec_ref( resource.get() );

                m_resources.erase( value );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ResourcePointer ResourceBank::createResource( const ConstString & _locale, const ConstString & _groupName, const ConstString & _name, const ConstString & _type, bool _keep, Resource ** const _override, const DocumentPtr & _doc )
    {
        LOGGER_INFO( "resource", "create resource '%s'"
            , _type.c_str()
        );

        ResourcePtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), _type, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "not registered resource type '%s'"
            , _type.c_str()
        );

        if( resource->initialize() == false )
        {
            LOGGER_ERROR( "resource '%s' invalid initialize (doc: %s)"
                , _type.c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

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

        if( _name.empty() == false )
        {
            resource->setMapping( true );

            ResourcePtrView prev_resource = m_resources.change( ConstString::none(), _name, resource );

            if( prev_resource != nullptr )
            {
                prev_resource->setResourceBank( nullptr );
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

        if( _keep == true )
        {
            IntrusivePtrBase::intrusive_ptr_add_ref( resource.get() );
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
        MENGINE_ASSERTION_FATAL( _resource->isMapping() == false || m_resources.exist( ConstString::none(), _resource->getName() ) == true, "resource '%s' type '%s' not found (maybe already remove)"
            , _resource->getName().c_str()
            , _resource->getType().c_str()
        );

        _resource->setResourceBank( nullptr );

        _resource->finalize();

        const ConstString & name = _resource->getName();

        if( _resource->isMapping() == true )
        {
            m_resources.erase( ConstString::none(), name );
        }

        bool keep = _resource->isKeep();

        if( keep == true )
        {
            IntrusivePtrBase::intrusive_ptr_dec_ref( _resource );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & ResourceBank::getResource( const ConstString & _name ) const
    {
        MENGINE_ASSERTION_MAIN_THREAD_GUARD();

        const ResourcePtr & resource = m_resources.find( ConstString::none(), _name );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "resource '%s' does not exist"
            , _name.c_str()
        );

        if( resource->compile() == false )
        {
            LOGGER_ERROR( "resource '%s' '%s' is not compile!"
                , _name.c_str()
                , resource->getType().c_str()
            );

            return ResourcePtr::none();
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & ResourceBank::getResourceReference( const ConstString & _name ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        const ResourcePtr & resource = m_resources.find( ConstString::none(), _name );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "resource '%s' does not exist"
            , _name.c_str()
        );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceBank::hasResource( const ConstString & _name, ResourcePtr * const _resource ) const
    {
        const ResourcePtr & resource = m_resources.find( ConstString::none(), _name );

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
    bool ResourceBank::hasResourceWithType( const ConstString & _name, const ConstString & _type, ResourcePtr * const _resource ) const
    {
        const ResourcePtr & resource = m_resources.find( ConstString::none(), _name );

        if( resource == nullptr )
        {
            return false;
        }

        const ConstString & resourceType = resource->getType();

        if( resourceType != _type )
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