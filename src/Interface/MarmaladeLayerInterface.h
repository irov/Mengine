#   pragma once

#   include "Interface/ServiceInterface.h"

#	include "Config/String.h"
#   include "Core/FilePath.h"

#   ifndef MAX_PATH
#   define MAX_PATH 260
#   endif

namespace Menge
{
    class MarmaladeLayerInterface
        : public ServiceInterface
    {
    public:
        SERVICE_DECLARE("MarmaladeLayerService")

    public:
        virtual bool concatenateFilePath( const FilePath & _folder, const FilePath & _dir, const char * _filename, size_t _filenamelen, Char * _filePath, size_t _capacity ) = 0;
    };

#   define MARMALADELAYER_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::MarmaladeLayerInterface>(serviceProvider))
}