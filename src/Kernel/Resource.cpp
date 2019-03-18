#include "Resource.h"

#include "Interface/CodecServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Resource::Resource()
        : m_cache( false )
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
    void Resource::setFileGroup( const FileGroupInterfacePtr & _fileGroup )
    {
        m_fileGroup = _fileGroup;
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
    bool Resource::_incrementZero()
    {
        bool result = this->compile();

#ifndef MENGINE_MASTER_RELEASE
        NOTIFICATION_NOTIFY( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, this );
#endif

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::_decrementZero()
    {
        this->release();

#ifndef MENGINE_MASTER_RELEASE
        NOTIFICATION_NOTIFY( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, this );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::cache()
    {
        if( this->incrementReference() == false )
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

        this->decrementReference();
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
