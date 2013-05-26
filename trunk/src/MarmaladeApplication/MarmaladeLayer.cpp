#   include "MarmaladeLayer.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SERVICE_FACTORY( MarmaladeLayer, Menge::MarmaladeLayerInterface, Menge::MarmaladeLayer );
    //////////////////////////////////////////////////////////////////////////
    MarmaladeLayer::MarmaladeLayer()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MarmaladeLayer::~MarmaladeLayer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeLayer::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MarmaladeLayer::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeLayer::concatenateFilePath( const FilePath & _folder, const FilePath & _filename, Char * _filePath, size_t _capacity )
    {
        size_t folderSize = _folder.size();
        size_t fileSize = _filename.size();

        if( folderSize + fileSize > _capacity )
        {
            return false;
        }

        strcpy( _filePath, _folder.c_str() );
        strcat( _filePath, _filename.c_str() );

        return true;
    }
}