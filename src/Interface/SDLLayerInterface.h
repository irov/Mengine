#   pragma once

#include "Interface/ServiceInterface.h"

#include "Config/String.h"
#include "Core/FilePath.h"

namespace Mengine
{
    class SDLLayerInterface
        : public ServiceInterface
    {
    public:
        SERVICE_DECLARE("SDLLayerService")

    public:
        virtual bool concatenateFilePath( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * _concatenatePath, size_t _capacity ) = 0;
    };

#   define SDLLAYER_SERVICE()\
    ((Mengine::SDLLayerInterface *)SERVICE_GET(Mengine::SDLLayerInterface))
}
