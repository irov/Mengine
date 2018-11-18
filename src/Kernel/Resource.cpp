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
    bool Resource::convert()
    {
        bool result = this->_convert();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::_convert()
    {
        //Empty
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::convertDefault2_( const ConstString & _converterType, const FilePath & _filePath, FilePath & _outFilePath )
    {
        if( _filePath.empty() == true )
        {
            return false;
        }

        if( _converterType.empty() == false )
        {
            if( CONVERTER_SERVICE()
                ->convert( _converterType, m_fileGroup, _filePath, _outFilePath ) == false )
            {
                LOGGER_ERROR( "resource '%s' can't convert '%s':'%s'"
                    , this->getName().c_str()
                    , _filePath.c_str()
                    , _converterType.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Resource::convertDefault_( const ConstString & _converterType, const FilePath & _filePath, FilePath & _outFilePath, const ConstString & _codecType, ConstString & _outCodecType )
    {
        if( this->convertDefault2_( _converterType, _filePath, _outFilePath ) == false )
        {
            return false;
        }

        if( _outFilePath.empty() == true )
        {
            _outFilePath = _filePath;
            _outCodecType = _codecType;

            return true;
        }

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( _outFilePath );

        if( codecType.empty() == true )
        {
            LOGGER_ERROR( "resource '%s' you must determine codec for file '%s'"
                , this->getName().c_str()
                , _filePath.c_str()
            );

            return false;
        }

        _outCodecType = codecType;

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
        NOTIFICATION_SERVICE()
            ->notify( NOTIFICATOR_RESOURCE_COMPILE, this );
#endif

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void Resource::_decrementZero()
    {
        this->release();

#ifndef MENGINE_MASTER_RELEASE
        NOTIFICATION_SERVICE()
            ->notify( NOTIFICATOR_RESOURCE_RELEASE, this );
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
