#include "Resource.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Resource::Resource()
        : m_compileReferenceCount( 0 )
        , m_prefetchReferenceCount( 0 )
        , m_cache( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Resource::~Resource()
    {
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
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::compile()
    {
        if( ++m_compileReferenceCount == 1 )
        {
            LOGGER_INFO( "compile '%s:%s'"
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
        MENGINE_ASSERTION_FATAL( m_compileReferenceCount != 0, "'%s:%s' release compile ref count == 0"
            , this->getType().c_str()
            , this->getName().c_str()
        );

        if( --m_compileReferenceCount == 0 )
        {
            LOGGER_INFO( "release '%s:%s'"
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
}
