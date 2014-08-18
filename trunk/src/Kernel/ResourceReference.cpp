#	include "ResourceReference.h"

#   include "Interface/ServiceInterface.h"

#	include "Interface/ScriptSystemInterface.h"

#   include "Interface/CodecInterface.h"
#   include "Interface/ConverterInterface.h"

#	include "Interface/NotificationServiceInterace.h"
#	include "Interface/NotificatorInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceReference::ResourceReference()
        : m_serviceProvider(nullptr)
		, m_cache(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceReference::~ResourceReference()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ResourceReference::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::setCategory( const ConstString & _category )
	{
		m_category = _category;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::setGroup( const ConstString & _group )
	{
		m_group = _group;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceReference::isValid() const
	{
        bool result = this->_isValid();

		return result;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ResourceReference::_isValid() const
    {
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceReference::_loader( const Metabuf::Metadata * _parser )
	{
        (void)_parser;
		//Empty

        return true;
	}
    //////////////////////////////////////////////////////////////////////////        
    bool ResourceReference::convert()
    {
        bool result = this->_convert(); 

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceReference::_convert()
    {
        //Empty
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceReference::convertDefault2_( const ConstString & _converter, const ConstString & _path, ConstString & _out )
	{
		if( _path.empty() == true )
		{
			return false;
		}

		if( _converter.empty() == false )
		{
			if( CONVERTER_SERVICE(m_serviceProvider)
				->convert( _converter, m_category, _path, _out ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceReference::convertDefault_: '%s' can't convert '%s':'%s'"
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
	bool ResourceReference::convertDefault_( const ConstString & _converter, const ConstString & _path, ConstString & _out, ConstString & _codecType )
	{
		if( this->convertDefault2_( _converter, _path, _out ) == false )
		{
			return false;
		}

		if( _codecType.empty() == true )
		{
			_codecType = CODEC_SERVICE(m_serviceProvider)
				->findCodecType( _path );
		}

		if( _codecType.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceReference::convertDefault_: '%s' you must determine codec for file '%s'"
				, this->getName().c_str()
				, _path.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceReference::_incrementZero()
	{
		bool result = this->compile();

        //LOGGER_WARNING(m_serviceProvider)("Resource compile %s %s %s"
        //    , this->getName().c_str()
        //    , this->getType().c_str()
        //    , this->getCategory().c_str()
        //    );

#   ifndef MENGE_MASTER_RELEASE
        NOTIFICATION_SERVICE(m_serviceProvider)
            ->notify( NOTIFICATOR_RESOURCE_COMPILE, this );
#   endif

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::_decrementZero()
	{
		this->release();

#   ifndef MENGE_MASTER_RELEASE
        NOTIFICATION_SERVICE(m_serviceProvider)
            ->notify( NOTIFICATOR_RESOURCE_RELEASE, this );
#	endif
	}
    //////////////////////////////////////////////////////////////////////////
    PyObject * ResourceReference::_embedded()
    { 
        const ConstString & type = this->getType();

        PyObject * embedding = SCRIPT_SERVICE(m_serviceProvider)
            ->wrap( type, this );

        return embedding;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ResourceReference::cache()
	{
		if( this->incrementReference() == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceReference::cache: '%s:%s' invalid increment reference"
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
	void ResourceReference::uncache()
	{
		m_cache = false;

		this->_uncache();

		this->decrementReference();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::_cache()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceReference::_uncache()
	{
		//Empty
	}
}
