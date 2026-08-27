#pragma once

#include "Interface/FileGroupInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/JSON.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        enum ETiledMapDocumentType
        {
            ETMDT_MAP,
            ETMDT_TILESET
        };
        //////////////////////////////////////////////////////////////////////////
        bool loadTiledMapDocument( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ETiledMapDocumentType _documentType, jpp::object * const _document );
        bool loadTiledMapJSONDocument( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ETiledMapDocumentType _documentType, jpp::object * const _document );
        bool loadTiledMapXMLDocument( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ETiledMapDocumentType _documentType, jpp::object * const _document );
        //////////////////////////////////////////////////////////////////////////
    }
}
