#include "TiledMapReader.h"

#include "Kernel/FileStreamHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        bool loadTiledMapJSONDocument( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ETiledMapDocumentType _documentType, jpp::object * const _document )
        {
            if( _fileGroup->existFile( _filePath, true ) == false )
            {
                LOGGER_ERROR( "can't find Tiled JSON '%s'"
                    , _filePath.c_str()
                );

                return false;
            }

            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, MENGINE_DOCUMENT_FUNCTION );

            if( stream == nullptr )
            {
                LOGGER_ERROR( "can't open Tiled JSON '%s'"
                    , _filePath.c_str()
                );

                return false;
            }

            jpp::object document = Helper::loadJSONStream( stream, MENGINE_DOCUMENT_FUNCTION );

            Helper::closeInputStreamFile( _fileGroup, stream );

            if( document.invalid() == true || document.is_type_object() == false )
            {
                LOGGER_ERROR( "invalid Tiled JSON root '%s'"
                    , _filePath.c_str()
                );

                return false;
            }

            const Char * expectedType = _documentType == ETMDT_MAP ? "map" : "tileset";
            const Char * documentType = document.get( "type", "" );

            if( documentType[0] != '\0' && StdString::strcmp( documentType, expectedType ) != 0 )
            {
                LOGGER_ERROR( "expected Tiled JSON type '%s' in '%s' but found '%s'"
                    , expectedType
                    , _filePath.c_str()
                    , documentType
                );

                return false;
            }

            const Char * extension = Helper::getFilePathExt( _filePath ).c_str();
            String lowercaseExtension;

            for( const Char * ch = extension; *ch != '\0'; ++ch )
            {
                lowercaseExtension += *ch >= 'A' && *ch <= 'Z' ? *ch - 'A' + 'a' : *ch;
            }

            if( lowercaseExtension == "json" && documentType[0] == '\0' )
            {
                LOGGER_ERROR( "generic JSON '%s' has no Tiled document type"
                    , _filePath.c_str()
                );

                return false;
            }

            *_document = std::move( document );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
