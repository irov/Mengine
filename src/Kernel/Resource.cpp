#include "Resource.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ThreadGuardScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Resource::Resource()
        : m_resourceBank( nullptr )
        , m_compileReferenceCount( 0 )
        , m_prefetchReferenceCount( 0 )
        , m_cache( false )
        , m_groupCache( false )
        , m_keep( false )
        , m_mapping( false )
        , m_precompile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Resource::~Resource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::setResourceBank( ResourceBankInterface * _bank )
    {
        m_resourceBank = _bank;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::setLocale( const ConstString & _locale )
    {
        m_locale = _locale;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::setGroupName( const ConstString & _groupName )
    {
        m_groupName = _groupName;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::setGroupCache( bool _groupCache )
    {
        m_groupCache = _groupCache;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::setKeep( bool _keep )
    {
        m_keep = _keep;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::setMapping( bool _mapping )
    {
        m_mapping = _mapping;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::setPrecompile( bool _precompile )
    {
        m_precompile = _precompile;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::setTags( const Tags & _tags )
    {
        m_tags = _tags;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & Resource::getTags() const
    {
        return m_tags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::initialize()
    {
        bool successful = this->_initialize();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::_initialize()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::finalize()
    {
        this->_finalize();

        ContentInterface * content = this->getContent();

        if( content != nullptr )
        {
            content->setFileGroup( nullptr );
            content->setDataflow( nullptr );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::_finalize()
    {
        //Empty 
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::compile()
    {
        MENGINE_THREAD_GUARD_SCOPE( Resource, this, "Resource::compile" );

        if( ++m_compileReferenceCount == 1 )
        {
            LOGGER_INFO( "resource", "compile '%s:%s'"
                , this->getType().c_str()
                , this->getName().c_str()
            );

            if( Compilable::compile() == false )
            {
                return false;
            }

#ifndef MENGINE_MASTER_RELEASE
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, this );
#endif
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::release()
    {
        MENGINE_THREAD_GUARD_SCOPE( Resource, this, "Resource::release" );

        MENGINE_ASSERTION_FATAL( m_compileReferenceCount != 0, "'%s:%s' release compile ref count == 0"
            , this->getType().c_str()
            , this->getName().c_str()
        );

        if( --m_compileReferenceCount == 0 )
        {
            LOGGER_INFO( "resource", "release '%s:%s'"
                , this->getType().c_str()
                , this->getName().c_str()
            );

            Compilable::release();

#ifndef MENGINE_MASTER_RELEASE
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, this );
#endif
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::prefetch( const LambdaPrefetch & _lambda )
    {
        if( m_prefetchReferenceCount == 0 )
        {
            if( _lambda() == false )
            {
                return false;
            }
        }

        ++m_prefetchReferenceCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::unfetch( const LambdaUnfetch & _lambda )
    {
        if( m_prefetchReferenceCount == 0 )
        {
            return false;
        }

        if( --m_prefetchReferenceCount == 0 )
        {
            if( _lambda() == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::cache()
    {
        if( this->compile() == false )
        {
            LOGGER_ERROR( "resource '%s:%s' invalid increment reference"
                , this->getGroupName().c_str()
                , this->getName().c_str()
            );

            return false;
        }

        m_cache = true;

        this->_cache();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::uncache()
    {
        m_cache = false;

        this->_uncache();

        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::_cache()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::_uncache()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::_destroy()
    {
        if( m_resourceBank != nullptr )
        {
            m_resourceBank->destroyResource( this );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
