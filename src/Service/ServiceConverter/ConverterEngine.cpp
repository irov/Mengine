#	include "ConverterEngine.h"

#   include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ConverterService, Menge::ConverterEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ConverterEngine::ConverterEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ConverterEngine::~ConverterEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ConverterEngine::registerConverter( const ConstString & _type, ConverterFactoryInterface * _factory )
	{
        LOGGER_INFO(m_serviceProvider)("ConverterEngine::registerConverter add converter %s"
            , _type.c_str()
            );

		m_mapConverterSystem.insert( std::make_pair(_type, _factory) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ConverterEngine::unregisterConverter( const ConstString& _type )
	{
        LOGGER_INFO(m_serviceProvider)("ConverterEngine::unregisterConverter remove converter %s"
            , _type.c_str()
            );

		TMapConverterSystem::iterator it_found = m_mapConverterSystem.find( _type );

		if( it_found == m_mapConverterSystem.end() )
		{
			return false;
		}

		m_mapConverterSystem.erase( it_found );

		return true;
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

		ConverterFactoryInterface * factory = it_find->second;

		if( factory == nullptr )
		{
			LOGGER_INFO( m_serviceProvider )("ConverterEngine::createConverter invalid factory %s is nullptr"
				, _type.c_str()
				);

			return nullptr;
		}

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
            LOGGER_ERROR(m_serviceProvider)("ConverterEngine::convert: can't create converter '%s'"
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
            LOGGER_ERROR(m_serviceProvider)("ConverterEngine::convert: can't get file system '%s' for converter '%s'"
                , _category.c_str()
                , _converter.c_str()
                );

            return false;
        }

        PathString cache_path;
		
		cache_path += _in;
				
		cache_path.cut_before_last_of( '.' );
		
		const String & ext = converter->getConvertExt();
		cache_path += ext;
		
        options.outputFileName = Helper::stringizeString( m_serviceProvider, cache_path );

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
            ->existFile( options.pakName, options.inputFileName, nullptr ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ConverterEngine::convert: input file '%s:%s' not found"
				, options.pakName.c_str()
                , options.inputFileName.c_str()
                );

            return false;
        }

        if( FILE_SERVICE(m_serviceProvider)
            ->existFile( options.pakName, options.outputFileName, nullptr ) == true )
        {			
            InputStreamInterfacePtr oldFile = FILE_SERVICE(m_serviceProvider)
                ->openInputFile( options.pakName, options.inputFileName, false );

            if( oldFile == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("ConverterEngine::convert '%s' can't open input file '%s:%s' (time)"
                    , _converter.c_str()
					, options.pakName.c_str()
                    , options.inputFileName.c_str()
                    );

                return false;
            }

            uint64_t fileTimeInput;
            oldFile->time( fileTimeInput );

            oldFile = nullptr;

            InputStreamInterfacePtr newFile = FILE_SERVICE(m_serviceProvider)
                ->openInputFile( options.pakName, options.outputFileName, false );

            if( newFile == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("ConverterEngine::convert '%s' can't open output file '%s:%s' (time)"
                    , _converter.c_str()
					, options.pakName.c_str()
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

				LOGGER_WARNING(m_serviceProvider)("ConverterEngine::convert invalid version '%s:%s'"
					, options.pakName.c_str()
					, options.outputFileName.c_str()
					);
            }
        }

		LOGGER_WARNING(m_serviceProvider)("ConverterEngine::convert '%s:%s'\nfrom: %s\nto: '%s:%s'\n"
			, _converter.c_str()
			, options.pakName.c_str()
			, options.inputFileName.c_str()
			, options.pakName.c_str()
			, options.outputFileName.c_str()
			);

        if( converter->convert() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ConverterEngine::convert can't convert '%s:%s'\nfrom: %s\nto: '%s:%s'\n"
                , _converter.c_str()
				, options.pakName.c_str()
                , options.inputFileName.c_str()
				, options.pakName.c_str()
                , options.outputFileName.c_str()
                );

            return false;
        }
        
        _out = options.outputFileName;

        return true;
    }
}
