#include "Resource.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionReferenceCount.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/BuildMode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Resource::Resource()
        : m_resourceBank( nullptr )
        , m_prefetchReferenceSuccessful( false )
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
        MENGINE_ASSERTION_FATAL( m_initialize == false, "destroy without finalize resource '%s' type '%s'"
            , this->getName().c_str()
            , this->getType().c_str()
        );

        MENGINE_ASSERTION_REFERENCE_COUNT( m_compileReferenceCount, "compile reference '%s' type '%s'"
            , this->getName().c_str()
            , this->getType().c_str()
        );

        MENGINE_ASSERTION_REFERENCE_COUNT( m_prefetchReferenceCount, "prefetch reference '%s' type '%s'"
            , this->getName().c_str()
            , this->getType().c_str()
        );

        MENGINE_ASSERTION_REFERENCE_COUNT( m_cacheReferenceCount, "cache reference '%s' type '%s'"
            , this->getName().c_str()
            , this->getType().c_str()
        );
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
    void Resource::setLocales( const VectorConstString & _locales )
    {
        m_locales = _locales;
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
        MENGINE_ASSERTION_FATAL( m_initialize == false, "resource '%s' type '%s' already initialize"
            , this->getName().c_str()
            , this->getType().c_str()
        );

        if( this->_initialize() == false )
        {
            return false;
        }

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        if( m_ignored == false && Helper::isDevelopmentMode() == true )
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
        MENGINE_ASSERTION_FATAL( m_initialize == true, "resource '%s' type '%s' not initialize (doc: %s)"
            , this->getName().c_str()
            , this->getType().c_str()
            , MENGINE_DOCUMENT_STR( this->getDocument() )
        );

        MENGINE_ASSERTION_FATAL( this->isCompile() == false, "resource '%s' type '%s' group '%s' cache '%d' keep '%d' mapping '%d' precompile '%d' ignored '%d' not release before finalize (doc: %s)"
            , this->getName().c_str()
            , this->getType().c_str()
            , this->getGroupName().c_str()
            , this->isGroupCache()
            , this->isKeep()
            , this->isMapping()
            , this->isPrecompile()
            , this->isIgnored()
            , MENGINE_DOCUMENT_STR( this->getDocument() )
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

        uint32_t referenceCount = m_compileReferenceCount.increfReferenceCount();

        if( referenceCount != 0 )
        {
            return true;
        }

        LOGGER_INFO( "resource", "compile [%s] name '%s' group '%s' file '%s'"
            , this->getType().c_str()
            , this->getName().c_str()
            , this->getGroupName().c_str()
            , Helper::getContentFullPath( this->getContent() ).c_str()
        );

        if( Compilable::compile() == false )
        {
            m_compileReferenceCount.resetReferenceCount();

            return false;
        }

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        if( Helper::isDevelopmentMode() == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, this );
        }
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

        uint32_t referenceCount = m_compileReferenceCount.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return;
        }

        LOGGER_INFO( "resource", "release [%s] name '%s' group '%s' file '%s'"
            , this->getType().c_str()
            , this->getName().c_str()
            , this->getGroupName().c_str()
            , Helper::getContentFullPath( this->getContent() ).c_str()
        );

        Compilable::release();

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        if( Helper::isDevelopmentMode() == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, this );
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::prefetch( const LambdaPrefetch & _lambda )
    {
        uint32_t referenceCount = m_prefetchReferenceCount.increfReferenceCount();

        if( referenceCount != 0 )
        {
            return true;
        }
        
        if( _lambda() == false )
        {
            m_prefetchReferenceCount.decrefReferenceCount();

            return false;
        }

        m_prefetchReferenceSuccessful = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::unfetch( const LambdaUnfetch & _lambda )
    {
        if( m_prefetchReferenceSuccessful == false )
        {
            return false;
        }

        uint32_t referenceCount = m_prefetchReferenceCount.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return true;
        }
        
        m_prefetchReferenceSuccessful = false;

        if( _lambda() == false )
        {
            return false;
        }        
    
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::cache()
    {
        uint32_t referenceCount = m_cacheReferenceCount.increfReferenceCount();

        if( referenceCount != 0 )
        {
            return true;
        }

        if( this->compile() == false )
        {
            m_cacheReferenceCount.resetReferenceCount();

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
        uint32_t referenceCount = m_cacheReferenceCount.decrefReferenceCount();

        if( referenceCount != 0 )
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