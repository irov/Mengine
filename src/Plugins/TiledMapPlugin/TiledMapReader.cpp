#include "TiledMapReader.h"

#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        bool loadTiledMapDocument( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ETiledMapDocumentType _documentType, jpp::object * const _document )
        {
            const Char * extension = Helper::getFilePathExt( _filePath ).c_str();
            String lowercaseExtension;

            for( const Char * ch = extension; *ch != '\0'; ++ch )
            {
                lowercaseExtension += *ch >= 'A' && *ch <= 'Z' ? *ch - 'A' + 'a' : *ch;
            }

            if( lowercaseExtension == "tmx" )
            {
                if( _documentType != ETMDT_MAP )
                {
                    LOGGER_ERROR( "expected a tileset but references Tiled TMX '%s'"
                        , _filePath.c_str()
                    );

                    return false;
                }

                return Detail::loadTiledMapXMLDocument( _fileGroup, _filePath, _documentType, _document );
            }

            if( lowercaseExtension == "tsx" )
            {
                if( _documentType != ETMDT_TILESET )
                {
                    LOGGER_ERROR( "expected a map but references Tiled TSX '%s'"
                        , _filePath.c_str()
                    );

                    return false;
                }

                return Detail::loadTiledMapXMLDocument( _fileGroup, _filePath, _documentType, _document );
            }

            if( lowercaseExtension == "tmj" )
            {
                if( _documentType != ETMDT_MAP )
                {
                    LOGGER_ERROR( "expected a tileset but references Tiled TMJ '%s'"
                        , _filePath.c_str()
                    );

                    return false;
                }

                return Detail::loadTiledMapJSONDocument( _fileGroup, _filePath, _documentType, _document );
            }

            if( lowercaseExtension == "tsj" )
            {
                if( _documentType != ETMDT_TILESET )
                {
                    LOGGER_ERROR( "expected a map but references Tiled TSJ '%s'"
                        , _filePath.c_str()
                    );

                    return false;
                }

                return Detail::loadTiledMapJSONDocument( _fileGroup, _filePath, _documentType, _document );
            }

            if( lowercaseExtension == "json" )
            {
                return Detail::loadTiledMapJSONDocument( _fileGroup, _filePath, _documentType, _document );
            }

            LOGGER_ERROR( "unsupported Tiled document '%s'"
                , _filePath.c_str()
            );

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
