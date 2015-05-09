#   pragma once

#   include "Interface/ServiceInterface.h"

#	include "Config/String.h"
#   include "Core/FilePath.h"

namespace Menge
{
    class SDLLayerInterface
        : public ServiceInterface
    {
    public:
        SERVICE_DECLARE("SDLLayerService")

    public:
        virtual bool concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, Char * _filePath, size_t _capacity ) = 0;
    };

#   define SDLLAYER_SERVICE( serviceProvider )\
    ((Menge::SDLLayerInterface *)SERVICE_GET(serviceProvider, Menge::SDLLayerInterface))
}