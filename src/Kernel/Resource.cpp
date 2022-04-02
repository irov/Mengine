#include "Resource.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Resource::Resource()
        : m_resourceBank( nullptr )
        , m_initialize( false )
        , m_groupCache( false )
        , m_keep( false )
        , m_mapping( false )
        , m_precompile( false )
        , m_ignored( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Resource::~Resource()
    {
        MENGINE_ASSERTION_FATAL( m_initialize == false );
        MENGINE_ASSERTION_FATAL( m_resourceBank == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::setResourceBank( ResourceBankInterface * _bank )
    {
        m_resourceBank = _bank;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::setContent( const ContentInterfacePtr & _content )
    {
        m_content = _content;
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
    void Resource::setIgnored( bool _ignored )
    {
        m_ignored = _ignored;
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
        MENGINE_ASSERTION_FATAL( m_initialize == false );

        if( this->_initialize() == false )
        {
            return false;
        }

#ifndef MENGINE_MASTER_RELEASE
        if( m_ignored == false )
        {
            if( NOTIFICATION_NOTIFY( NOTIFICATOR_DEVELOPMENT_RESOURCE_INITIALIZE, this ) == false )
            {
                LOGGER_ERROR( "resource '%s' type [%s] invalid convert"
                    , this->getName().c_str()
                    , this->getType().c_str()
                );

                return false;
            }
        }
#endif

        m_initialize = true;

        return true;
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
        MENGINE_ASSERTION_FATAL( m_initialize == true );

        MENGINE_ASSERTION_FATAL( this->isCompile() == false, "resource '%s' type '%s' not release before finalize"
            , this->getName().c_str()
            , this->getType().c_str()
        );

        this->_finalize();

        m_initialize = false;

        m_content = nullptr;

        m_resourceBank = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::_finalize()
    {
        //Empty 
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::compile()
    {
        MENGINE_ASSERTION_FATAL( m_initialize == true, "resource '%s' type '%s' compile not initialize"
            , this->getName().c_str()
            , this->getType().c_str()
        );

        if( m_compileReferenceCount.incref() == false )
        {
            return true;
        }

        LOGGER_INFO( "resource", "compile type '%s' name '%s' group '%s'"
            , this->getType().c_str()
            , this->getName().c_str()
            , this->getGroupName().c_str()
        );

        if( Compilable::compile() == false )
        {
            m_compileReferenceCount.reset();

            return false;
        }

#ifndef MENGINE_MASTER_RELEASE
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, this );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::release()
    {
        MENGINE_ASSERTION_FATAL( m_initialize == true, "resource '%s' type '%s' release not initialize"
            , this->getName().c_str()
            , this->getType().c_str()
        );

        if( m_compileReferenceCount.decref() == false )
        {
            return;
        }

        LOGGER_INFO( "resource", "release type '%s' name '%s' group '%s'"
            , this->getType().c_str()
            , this->getName().c_str()
            , this->getGroupName().c_str()
        );

        Compilable::release();

#ifndef MENGINE_MASTER_RELEASE
        NOTIFICATION_NOTIFY( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, this );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::prefetch( const LambdaPrefetch & _lambda )
    {
        if( m_prefetchReferenceCount.incref() == false )
        {
            return true;
        }
        
        if( _lambda() == false )
        {
            m_prefetchReferenceCount.reset();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::unfetch( const LambdaUnfetch & _lambda )
    {
        if( m_prefetchReferenceCount.decref() == false )
        {
            return true;
        }
         
        if( _lambda() == false )
        {
            return false;
        }
    
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::cache()
    {
        if( m_cacheReferenceCount.incref() == false )
        {
            return true;
        }

        if( this->compile() == false )
        {
            m_cacheReferenceCount.reset();

            LOGGER_ERROR( "cache resource '%s' group '%s' invalid compile"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        this->_cache();
    
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::uncache()
    {
        if( m_cacheReferenceCount.decref() == false )
        {
            return;
        }

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
}