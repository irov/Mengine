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

        //LOGGER_WARNING("Resource compile %s %s %s"
        //    , this->getName().c_str()
        //    , this->getType().c_str()
        //    , this->getCategory().c_str()
        //    );

#ifndef MENGINE_MASTER_RELEASE
        NOTIFICATION_NOTIFY( NOTIFICATOR_RESOURCE_COMPILE, (this) );
#endif

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::_decrementZero()
    {
        this->release();

#ifndef MENGINE_MASTER_RELEASE
        NOTIFICATION_NOTIFY( NOTIFICATOR_RESOURCE_RELEASE, (this) );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::cache()
    {
        if( this->incrementReference() == false )
        {
            LOGGER_ERROR( "ResourceReference::cache: '%s:%s' invalid increment reference"
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
