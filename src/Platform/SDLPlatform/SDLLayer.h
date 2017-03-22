#   pragma once

#   include "Interface/SDLLayerInterface.h"

#   include "Core/ServantBase.h"

namespace Menge
{
    class SDLLayer
        : public ServiceBase<SDLLayerInterface>
    {
    public:
        SDLLayer();
        ~SDLLayer();

    public:
        bool concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, Char * _filePath, size_t _capacity ) override;
    };
}
