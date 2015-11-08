#   pragma once

#   include "Interface/MarmaladeLayerInterface.h"

namespace Menge
{
    class MarmaladeLayer
        : public ServiceBase<MarmaladeLayerInterface>
    {
    public:
        MarmaladeLayer();
        ~MarmaladeLayer();

    public:
        bool concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, Char * _filePath, size_t _capacity ) override;
    };
}