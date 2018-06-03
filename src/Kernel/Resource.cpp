#include "Resource.h"

#include "Interface/ServiceInterface.h"

#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"

#include "Interface/NotificationServiceInterface.h"

#include "Logger/Logger.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Resource::Resource()
        : m_cache(false)
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
	void Resource::setCategory( const ConstString & _category )
	{
		m_category = _category;
	}
	//////////////////////////////////////////////////////////////////////////
	void Resource::setGroup( const ConstString & _group )
	{
		m_group = _group;
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
	bool Resource::isValid() const
	{
        bool result = this->_isValid();

		return result;
	}
    //////////////////////////////////////////////////////////////////////////
    bool Resource::_isValid() const
    {
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool Resource::_loader( const Metabuf::Metadata * _parser )
	{
        (void)_parser;
		//Empty

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
	bool Resource::convertDefault2_( const ConstString & _converter, const FilePath & _path, FilePath & _out )
	{
		if( _path.empty() == true )
		{
			return false;
		}

		if( _converter.empty() == false )
		{
			if( CONVERTER_SERVICE()
				->convert( _converter, m_category, _path, _out ) == false )
			{
				LOGGER_ERROR("ResourceReference::convertDefault_: '%s' can't convert '%s':'%s'"
					, this->getName().c_str() 
					, _path.c_str()
					, _converter.c_str()
					);

				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Resource::convertDefault_( const ConstString & _converter, const FilePath & _path, FilePath & _out, ConstString & _codecType )
	{
		if( this->convertDefault2_( _converter, _path, _out ) == false )
		{
			return false;
		}

		const ConstString & codecType = CODEC_SERVICE()
			->findCodecType( _out );

		if( codecType.empty() == true )
		{
			LOGGER_ERROR("ResourceReference::convertDefault_: '%s' you must determine codec for file '%s'"
				, this->getName().c_str()
				, _path.c_str()
				);

			return false;
		}

		_codecType = codecType;

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
			LOGGER_ERROR("ResourceReference::cache: '%s:%s' invalid increment reference"
				, this->getGroup().c_str()
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
