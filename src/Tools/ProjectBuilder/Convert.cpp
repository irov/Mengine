#include "Convert.h"

#include "Interface/ConverterServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    bool convert( const WChar * _fromPath, const WChar * _toPath, const WChar * _convertType, const Params & _params )
    {
        MENGINE_UNUSED( _params );

        String utf8_fromPath;
        Helper::unicodeToUtf8( _fromPath, &utf8_fromPath );

        String utf8_toPath;
        Helper::unicodeToUtf8( _toPath, &utf8_toPath );

        String utf8_convertType;
        Helper::unicodeToUtf8( _convertType, &utf8_convertType );

        ConverterOptions options;

        FileGroupInterfacePtr globalFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( globalFileGroup, "invalid get file group 'dev'" );

        ContentInterfacePtr inputContent = Helper::makeFileContent( globalFileGroup, Helper::stringizeFilePath( utf8_fromPath ), MENGINE_DOCUMENT_FUNCTION );
        ContentInterfacePtr outputContent = Helper::makeFileContent( globalFileGroup, Helper::stringizeFilePath( utf8_toPath ), MENGINE_DOCUMENT_FUNCTION );

        options.inputContent = inputContent;
        options.outputContent = outputContent;
        options.params = _params;

        ConverterInterfacePtr converter = CONVERTER_SERVICE()
            ->createConverter( Helper::stringizeString( utf8_convertType ), MENGINE_DOCUMENT_FUNCTION );

        if( converter == nullptr )
        {
            LOGGER_ERROR( "can't create convert '%s'\nfrom: %s\nto: %s\n"
                , utf8_convertType.c_str()
                , Helper::getContentFullPath( options.inputContent ).c_str()
                , Helper::getContentFullPath( options.outputContent ).c_str()
            );

            return false;
        }

        converter->setOptions( options );

        if( converter->convert() == false )
        {
            LOGGER_ERROR( "can't convert '%s'\nfrom: %s\nto: %s\n"
                , utf8_convertType.c_str()
                , Helper::getContentFullPath( options.inputContent ).c_str()
                , Helper::getContentFullPath( options.outputContent ).c_str()
            );

            return false;
        }

        return true;
    }
}