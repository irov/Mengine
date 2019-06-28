#include "ConverterService.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ConverterService, Mengine::ConverterService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ConverterService::ConverterService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ConverterService::~ConverterService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConverterService::registerConverter( const ConstString & _type, const ConverterFactoryInterfacePtr & _factory )
    {
        LOGGER_INFO( "ConverterService::registerConverter add converter %s"
            , _type.c_str()
        );

        m_mapConverterSystem.emplace( _type, _factory );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConverterService::unregisterConverter( const ConstString& _type )
    {
        LOGGER_INFO( "ConverterService::unregisterConverter remove converter %s"
            , _type.c_str()
        );

        MapConverterSystem::iterator it_found = m_mapConverterSystem.find( _type );

        if( it_found == m_mapConverterSystem.end() )
        {
            return false;
        }

        m_mapConverterSystem.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ConverterInterfacePtr ConverterService::createConverter( const ConstString & _type, const Char * _doc )
    {
        LOGGER_INFO( "create converter '%s'"
            , _type.c_str()
        );

        MapConverterSystem::iterator it_find = m_mapConverterSystem.find( _type );

        if( it_find == m_mapConverterSystem.end() )
        {
            LOGGER_INFO( "not found converter '%s'"
                , _type.c_str()
            );

            return nullptr;
        }

        const ConverterFactoryInterfacePtr & factory = it_find->second;

        if( factory == nullptr )
        {
            LOGGER_INFO( "invalid factory %s is nullptr"
                , _type.c_str()
            );

            return nullptr;
        }

        ConverterInterfacePtr converter = factory->createConverter( _doc );

        if( converter == nullptr )
        {
            LOGGER_INFO( "invalid create converter '%s'"
                , _type.c_str()
            );

            return nullptr;
        }

        if( converter->initialize() == false )
        {
            LOGGER_INFO( "invalid initialize converter '%s'"
                , _type.c_str()
            );

            return nullptr;
        }

        return converter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConverterService::convert( const ConstString & _converter, const FileGroupInterfacePtr & _category, const FilePath & _in, FilePath * _out, const Char * _doc )
    {
        if( _converter.empty() == true )
        {
            *_out = _in;
        }

        ConverterInterfacePtr converter = this->createConverter( _converter, _doc );

        if( converter == nullptr )
        {
            LOGGER_ERROR( "can't create converter '%s'"
                , _converter.c_str()
            );

            return false;
        }

        ConverterOptions options;
        options.fileGroup = _category;
        options.inputFileName = _in;

        PathString cache_path;

        cache_path += _in;

        cache_path.cut_before_last_of( '.' );

        const String & ext = converter->getConvertExt();
        cache_path += ext;

        options.outputFileName = Helper::stringizeFilePath( cache_path );

        converter->setOptions( &options );

        if( options.inputFileName.empty() == true )
        {
            LOGGER_ERROR( "input file is empty"
            );

            return false;
        }

        if( options.outputFileName.empty() == true )
        {
            LOGGER_ERROR( "output file is empty"
            );

            return false;
        }

        if( options.fileGroup->existFile( options.inputFileName ) == false )
        {
            LOGGER_ERROR( "input file '%s:%s' not found"
                , options.fileGroup->getName().c_str()
                , options.inputFileName.c_str()
            );

            return false;
        }

        if( options.fileGroup->existFile( options.outputFileName ) == true )
        {
            InputStreamInterfacePtr oldFile = FILE_SERVICE()
                ->openInputFile( options.fileGroup, options.inputFileName, false, _doc );

            if( oldFile == nullptr )
            {
                LOGGER_ERROR( "converter '%s' can't open input file '%s:%s' (time)"
                    , _converter.c_str()
                    , options.fileGroup->getName().c_str()
                    , options.inputFileName.c_str()
                );

                return false;
            }

            uint64_t fileTimeInput;
            oldFile->time( fileTimeInput );

            oldFile = nullptr;

            InputStreamInterfacePtr newFile = FILE_SERVICE()
                ->openInputFile( options.fileGroup, options.outputFileName, false, _doc );

            if( newFile == nullptr )
            {
                LOGGER_ERROR( "converter '%s' can't open output file '%s:%s' (time)"
                    , _converter.c_str()
                    , options.fileGroup->getName().c_str()
                    , options.outputFileName.c_str()
                );

                return false;
            }

            uint64_t fileTimeOutput;
            newFile->time( fileTimeOutput );

            if( fileTimeInput <= fileTimeOutput )
            {
                *_out = options.outputFileName;

                if( converter->validateVersion( newFile ) == true )
                {
                    return true;
                }

                LOGGER_WARNING( "invalid version '%s:%s'"
                    , options.fileGroup->getName().c_str()
                    , options.outputFileName.c_str()
                );
            }
        }

        LOGGER_WARNING( "converter '%s:%s'\nfrom: %s\nto: '%s:%s'\n"
            , _converter.c_str()
            , options.fileGroup->getName().c_str()
            , options.inputFileName.c_str()
            , options.fileGroup->getName().c_str()
            , options.outputFileName.c_str()
        );

        if( converter->convert() == false )
        {
            LOGGER_ERROR( "can't convert '%s:%s'\nfrom: %s\nto: '%s:%s'\n"
                , _converter.c_str()
                , options.fileGroup->getName().c_str()
                , options.inputFileName.c_str()
                , options.fileGroup->getName().c_str()
                , options.outputFileName.c_str()
            );

            return false;
        }

        *_out = options.outputFileName;

        return true;
    }
}
