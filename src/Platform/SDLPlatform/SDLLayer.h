#pragma once

#include "Interface/SDLLayerInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SDLLayer
        : public ServiceBase<SDLLayerInterface>
    {
    public:
        SDLLayer();
        ~SDLLayer();

    public:
        bool concatenateFilePath( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, Char * _concatenatePath, size_t _capacity ) override;
    };
}
