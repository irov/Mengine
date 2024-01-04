#include "ConverterService.h"

#include "Interface/ConverterFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PathString.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/BuildMode.h"
#include "Kernel/ContentHelper.h"

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
    bool ConverterService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ConverterService::_finalizeService()
    {
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "ConverterFactory" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    ConverterInterfacePtr ConverterService::createConverter( const ConstString & _type, const DocumentInterfacePtr & _doc )
    {
        LOGGER_INFO( "convert", "create converter '%s'"
            , _type.c_str()
        );

        ConverterFactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "ConverterFactory" ), _type );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, "not found converter factory '%s'"
            , _type.c_str()
        );

        ConverterInterfacePtr converter = factory->createConverter( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( converter, "invalid create converter '%s'"
            , _type.c_str()
        );

        if( converter->initialize() == false )
        {
            LOGGER_INFO( "convert", "invalid initialize converter '%s'"
                , _type.c_str()
            );

            return nullptr;
        }

        return converter;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ConverterService::convert( const ConstString & _converterType, const ContentInterfacePtr & _in, ContentInterfacePtr * const _out, const DocumentInterfacePtr & _doc )
    {
        if( _converterType.empty() == true )
        {
            *_out = _in;

            return true;
        }

        bool developmentMode = Helper::isDevelopmentMode();

        if( developmentMode == false )
        {
            return true;
        }

        ConverterInterfacePtr converter = this->createConverter( _converterType, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( converter, "can't create converter '%s'"
            , _converterType.c_str()
        );

        ConverterOptions options;
        options.inputContent = _in;

        const FileGroupInterfacePtr & inputFileGroup = _in->getFileGroup();
        const FilePath & inputFilePath = _in->getFilePath();

        PathString cache_path;

        cache_path += inputFilePath;

        cache_path.cut_before_last_of( '.' );

        const ConstString & ext = converter->getConvertExt();
        cache_path += ext;

        FilePath cache_filePath = Helper::stringizeFilePath( cache_path );

        const FileGroupInterfacePtr & outputFileGroup = inputFileGroup;
        const FilePath & outputFilePath = cache_filePath;

        ContentInterfacePtr outputContent = Helper::makeFileContent( outputFileGroup, outputFilePath, _doc );

        options.outputContent = outputContent;

        converter->setOptions( &options );

        if( inputFilePath.empty() == true )
        {
            LOGGER_ERROR( "input file is empty" );

            return false;
        }

        if( outputFilePath.empty() == true )
        {
            LOGGER_ERROR( "output file is empty" );

            return false;
        }

        if( options.inputContent->exist( true ) == false )
        {
            LOGGER_ERROR( "input file '%s' not found"
                , Helper::getContentFullPath( _in )
            );

            return false;
        }

        if( options.outputContent->exist( true ) == true )
        {
            InputStreamInterfacePtr oldFile = Helper::openInputStreamFile( inputFileGroup, inputFilePath, false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( oldFile, "converter '%s' can't open input file '%s' (time)"
                , _converterType.c_str()
                , Helper::getContentFullPath( options.inputContent )
            );

            uint64_t fileTimeInput;
            oldFile->time( &fileTimeInput );

            oldFile = nullptr;

            InputStreamInterfacePtr newFile = Helper::openInputStreamFile( outputFileGroup, outputFilePath, false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( newFile, "converter '%s' can't open output file '%s' (time)"
                , _converterType.c_str()
                , Helper::getContentFullPath( options.outputContent )
            );

            uint64_t fileTimeOutput;
            newFile->time( &fileTimeOutput );

            if( fileTimeInput <= fileTimeOutput )
            {
                *_out = options.outputContent;

                if( converter->validateVersion( newFile ) == true )
                {
                    return true;
                }

                LOGGER_WARNING( "file '%s' invalid version"
                    , Helper::getContentFullPath( options.outputContent )
                );
            }
        }

        LOGGER_WARNING( "converter '%s'\nfrom: %s\nto: '%s'\n"
            , _converterType.c_str()
            , Helper::getContentFullPath( options.inputContent )
            , Helper::getContentFullPath( options.outputContent )
        );

        if( converter->convert() == false )
        {
            LOGGER_ERROR( "can't convert '%s'\nfrom: %s\nto: '%s'\n"
                , _converterType.c_str()
                , Helper::getContentFullPath( options.inputContent )
                , Helper::getContentFullPath( options.outputContent )
            );

            return false;
        }

        *_out = options.outputContent;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
