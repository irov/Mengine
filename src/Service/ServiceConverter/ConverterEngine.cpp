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
        : m_serviceProvider(nullptr)
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
	void ConverterEngine::registerConverter( const ConstString & _type, ConverterFactoryInterface * _factory )
	{
        LOGGER_INFO(m_serviceProvider)("ConverterEngine::registerConverter add converter %s"
            , _type.c_str()
            );

		m_mapConverterSystem.insert( _type, _factory );
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
	ConverterInterfacePtr ConverterEngine::createConverter( const ConstString & _type )
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

			return nullptr;
		}

		ConverterFactoryInterface * factory = m_mapConverterSystem.get_value( it_find );

		ConverterInterfacePtr converter = factory->createConverter();

		if( converter == nullptr )
		{
            LOGGER_INFO(m_serviceProvider)("ConverterEngine::createConverter invalid create converter %s"
                , _type.c_str()
                );

			return nullptr;
		}

		if( converter->initialize() == false )
		{
            LOGGER_INFO(m_serviceProvider)("ConverterEngine::createConverter invalid initialize converter %s"
                , _type.c_str()
                );

			return nullptr;
		}

		return converter;
	}
	//////////////////////////////////////////////////////////////////////////
    bool ConverterEngine::convert( const ConstString & _converter, const ConstString & _category, const ConstString & _in, ConstString & _out )
    {
        ConverterInterfacePtr converter = this->createConverter( _converter );

        if ( converter == nullptr )
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
            ->hasFileGroup( _category, nullptr ) == false )
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

        if( options.inputFileName.empty() == true  )
        {
            LOGGER_ERROR(m_serviceProvider)("ConverterEngine::convert input file is empty"
                );

            return false;
        }

        if( options.outputFileName.empty() == true )
        {
            LOGGER_ERROR(m_serviceProvider)("ConverterEngine::convert output file is empty"
                );
            
            return false;
        }

        if( FILE_SERVICE(m_serviceProvider)
            ->existFile( options.pakName, options.inputFileName, nullptr, 0, nullptr ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ConverterEngine::convert: input file '%s' not found"
                , options.inputFileName.c_str()
                );

            return false;
        }

        if( FILE_SERVICE(m_serviceProvider)
            ->existFile( options.pakName, options.outputFileName, nullptr, 0, nullptr ) == true )
        {			
            InputStreamInterfacePtr oldFile = FILE_SERVICE(m_serviceProvider)
                ->openInputFile( options.pakName, options.inputFileName );

            if( oldFile == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)( "ConverterEngine::convert '%s' can't open input file '%s' (time)"
                    , _converter.c_str()
                    , options.inputFileName.c_str()
                    );

                return false;
            }

            uint64_t fileTimeInput;
            oldFile->time( fileTimeInput );

            oldFile = nullptr;

            InputStreamInterfacePtr newFile = FILE_SERVICE(m_serviceProvider)
                ->openInputFile( options.pakName, options.outputFileName );

            if( newFile == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)( "ConverterEngine::convert '%s' can't open output file '%s' (time)"
                    , _converter.c_str()
                    , options.outputFileName.c_str()
                    );

                return false;
            }

            uint64_t fileTimeOutput;
            newFile->time( fileTimeOutput );

            if( fileTimeInput <= fileTimeOutput )
            {
                _out = options.outputFileName;

				if( converter->validateVersion( newFile ) == true )
				{
	                return true;
				}

				LOGGER_WARNING(m_serviceProvider)( "ConverterEngine::convert invalid version '%s'"
					, options.outputFileName.c_str()
					);
            }
        }

		LOGGER_WARNING(m_serviceProvider)( "ConverterEngine::convert '%s'\nfrom: %s\nto: %s\n"
			, _converter.c_str()
			, options.inputFileName.c_str()
			, options.outputFileName.c_str()
			);

        if( converter->convert() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "ConverterEngine::convert can't convert '%s'\nfrom: %s\nto: %s\n"
                , _converter.c_str()
                , options.inputFileName.c_str()
                , options.outputFileName.c_str()
                );

            return false;
        }
        
        _out = options.outputFileName;

        return true;
    }
}
