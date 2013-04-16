#	include "ConverterEngine.h"

#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ConverterService, Menge::ConverterServiceInterface, Menge::ConverterEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConverterEngine::ConverterEngine()
        : m_serviceProvider(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConverterEngine::~ConverterEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ConverterEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ConverterEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void ConverterEngine::registerConverter( const ConstString& _type, ConverterFactoryInterface * _interface )
	{
        LOGGER_INFO(m_serviceProvider)("ConverterEngine::registerConverter add converter %s"
            , _type.c_str()
            );

		m_mapConverterSystem.insert( std::make_pair(_type, _interface) );
	}
	//////////////////////////////////////////////////////////////////////////
	void ConverterEngine::unregisterConverter( const ConstString& _type )
	{
        LOGGER_INFO(m_serviceProvider)("ConverterEngine::unregisterConverter remove converter %s"
            , _type.c_str()
            );

		m_mapConverterSystem.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	ConverterInterface * ConverterEngine::createConverter( const ConstString & _type )
	{
        LOGGER_INFO(m_serviceProvider)("ConverterEngine::createConverter %s"
            , _type.c_str()
            );

		TMapConverterSystem::iterator it_find = m_mapConverterSystem.find( _type );

		if( it_find == m_mapConverterSystem.end() )
		{
            LOGGER_INFO(m_serviceProvider)("ConverterEngine::createConverter not found converter %s"
                , _type.c_str()
                );

			return 0;
		}

		ConverterInterface * decoder = 
			it_find->second->createConverter();

		if( decoder == 0 )
		{
            LOGGER_INFO(m_serviceProvider)("ConverterEngine::createConverter invalid create converter %s"
                , _type.c_str()
                );

			return 0;
		}

		if( decoder->initialize() == false )
		{
            LOGGER_INFO(m_serviceProvider)("ConverterEngine::createConverter invalid initialize converter %s"
                , _type.c_str()
                );

			decoder->destroy();

			return 0;
		}

		return decoder;
	}
	//////////////////////////////////////////////////////////////////////////
    bool ConverterEngine::convert( const ConstString & _converter, const ConstString & _category, const ConstString & _in, ConstString & _out )
    {
        ConverterInterface * converter = this->createConverter( _converter );

        if ( converter == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "ConverterEngine::convert: can't create converter '%s'"
                , _converter.c_str()
                );

            return false;
        }

        ConverterOptions options; 
        options.pakName = _category;
        options.inputFileName = _in;

        if( FILE_SERVICE(m_serviceProvider)
            ->hasFileSystem( _category ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ConverterEngine::convert: can't get file system '%s' for converter '%s'"
                , _category.c_str()
                , _converter.c_str()
                );

            return false;
        }

        static String cache_path;
        cache_path.assign( _in.c_str(), _in.size() );
                
        static String outputFile;
        String::size_type pointPos = cache_path.find_last_of( '.' );
        outputFile = cache_path.substr( 0 , pointPos );
        outputFile += converter->getConvertExt();

        options.outputFileName = Helper::stringizeString( m_serviceProvider, outputFile );

        converter->setOptions( &options );

        bool skip;
        if ( converter->convert( skip ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ConverterEngine::convert can't convert '%s'\nfrom: %s\nto: %s\n"
                , _converter.c_str()
                , options.inputFileName.c_str()
                , options.outputFileName.c_str()
                );

            converter->destroy();

            return false;
        }

        if( skip == false )
        {
            LOGGER_WARNING(m_serviceProvider)( "ConverterEngine::convert '%s'\nfrom: %s\nto: %s\n"
                , _converter.c_str()
                , options.inputFileName.c_str()
                , options.outputFileName.c_str()
                );
        }

        _out = options.outputFileName;
        
        converter->destroy();

        return true;
    }
}
