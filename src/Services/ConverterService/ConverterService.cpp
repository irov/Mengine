#include "ConverterService.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PathString.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"

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
        LOGGER_INFO( "add converter '%s'"
            , _type.c_str()
        );

        m_mapConverterSystem.emplace( _type, _factory );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConverterService::unregisterConverter( const ConstString & _type )
    {
        LOGGER_INFO( "remove converter '%s'"
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

        MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr, "invalid factory '%s' is nullptr"
            , _type.c_str()
        );

        ConverterInterfacePtr converter = factory->createConverter( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( converter, nullptr, "invalid create converter '%s'"
            , _type.c_str()
        );

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
    bool ConverterService::convert( const ConstString & _converter, const FileGroupInterfacePtr & _fileGroup, const FilePath & _in, FilePath * _out, const Char * _doc )
    {
        if( _converter.empty() == true )
        {
            *_out = _in;

            return true;
        }

        ConverterInterfacePtr converter = this->createConverter( _converter, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( converter, false, "can't create converter '%s'"
            , _converter.c_str()
        );

        ConverterOptions options;
        options.fileGroup = _fileGroup;
        options.inputFilePath = _in;

        PathString cache_path;

        cache_path += _in;

        cache_path.cut_before_last_of( '.' );

        const ConstString & ext = converter->getConvertExt();
        cache_path += ext;

        options.outputFilePath = Helper::stringizeFilePath( cache_path );

        converter->setOptions( &options );

        if( options.inputFilePath.empty() == true )
        {
            LOGGER_ERROR( "input file is empty"
            );

            return false;
        }

        if( options.outputFilePath.empty() == true )
        {
            LOGGER_ERROR( "output file is empty"
            );

            return false;
        }

        if( options.fileGroup->existFile( options.inputFilePath, true ) == false )
        {
            LOGGER_ERROR( "input file '%s:%s' not found"
                , options.fileGroup->getName().c_str()
                , options.inputFilePath.c_str()
            );

            return false;
        }

        if( options.fileGroup->existFile( options.outputFilePath, true ) == true )
        {
            InputStreamInterfacePtr oldFile = Helper::openInputStreamFile( options.fileGroup, options.inputFilePath, false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( oldFile, false, "converter '%s' can't open input file '%s:%s' (time)"
                , _converter.c_str()
                , options.fileGroup->getName().c_str()
                , options.inputFilePath.c_str()
            );

            uint64_t fileTimeInput;
            oldFile->time( &fileTimeInput );

            oldFile = nullptr;

            InputStreamInterfacePtr newFile = Helper::openInputStreamFile( options.fileGroup, options.outputFilePath, false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( newFile, false, "converter '%s' can't open output file '%s:%s' (time)"
                , _converter.c_str()
                , options.fileGroup->getName().c_str()
                , options.outputFilePath.c_str()
            );

            uint64_t fileTimeOutput;
            newFile->time( &fileTimeOutput );

            if( fileTimeInput <= fileTimeOutput )
            {
                *_out = options.outputFilePath;

                if( converter->validateVersion( newFile ) == true )
                {
                    return true;
                }

                LOGGER_WARNING( "invalid version '%s:%s'"
                    , options.fileGroup->getName().c_str()
                    , options.outputFilePath.c_str()
                );
            }
        }

        LOGGER_WARNING( "converter '%s:%s'\nfrom: %s\nto: '%s:%s'\n"
            , _converter.c_str()
            , options.fileGroup->getName().c_str()
            , options.inputFilePath.c_str()
            , options.fileGroup->getName().c_str()
            , options.outputFilePath.c_str()
        );

        if( converter->convert() == false )
        {
            LOGGER_ERROR( "can't convert '%s:%s'\nfrom: %s\nto: '%s:%s'\n"
                , _converter.c_str()
                , options.fileGroup->getName().c_str()
                , options.inputFilePath.c_str()
                , options.fileGroup->getName().c_str()
                , options.outputFilePath.c_str()
            );

            return false;
        }

        *_out = options.outputFilePath;

        return true;
    }
}
