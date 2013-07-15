#	include "ResourceReference.h"

#   include "Interface/ServiceInterface.h"

#	include "Interface/ScriptSystemInterface.h"

#   include "Interface/CodecInterface.h"

#	include "Interface/NotificationServiceInterace.h"
#	include "Interface/NotificatorInterface.h"

#   include "Logger/Logger.h"
#   include "Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ResourceReference::ResourceReference()
        : m_serviceProvider(nullptr)
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

        NOTIFICATION_SERVICE(m_serviceProvider)
            ->notify( NOTIFICATOR_RESOURCE_RELEASE, this );
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
    const ConstString & ResourceReference::getCodec_( const FilePath & _filename ) const
    {
        const ConstString & codecType = CODEC_SERVICE(m_serviceProvider)->findCodecType( _filename );

        return codecType;
    }
}
