#   pragma once

#   include "Interface/SDLLayerInterface.h"

namespace Menge
{
    class SDLLayer
        : public ServiceBase<SDLLayerInterface>
    {
    public:
        SDLLayer();
        ~SDLLayer();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, Char * _filePath, size_t _capacity ) override;
        
    protected:
        ServiceProviderInterface * m_serviceProvider;
    };
}
