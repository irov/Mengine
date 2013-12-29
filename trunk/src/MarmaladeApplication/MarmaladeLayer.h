#   pragma once

#   include "Interface/MarmaladeLayerInterface.h"

namespace Menge
{
    class MarmaladeLayer
        : public MarmaladeLayerInterface
    {
    public:
        MarmaladeLayer();
        ~MarmaladeLayer();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        //bool concatenateFilePath( const FilePath & _folder, const FilePath & _filename, Char * _filePath, size_t _capacity ) override;
		bool concatenateFilePath( const FilePath & _folder, const FilePath & _dir, const char * _filename, size_t _filenamelen, Char * _filePath, size_t _capacity ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;
    };
}