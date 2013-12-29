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
    bool MarmaladeLayer::concatenateFilePath( const FilePath & _folder, const FilePath & _dir, const char * _filename, size_t _filenamelen, Char * _filePath, size_t _capacity )
    {
        size_t folderSize = _folder.size();
		size_t dirSize = _dir.size();

        if( folderSize + dirSize + _filenamelen > _capacity )
        {
            return false;
        }

        strcpy( _filePath, _folder.c_str() );
        strcat( _filePath, _dir.c_str() );

		if( _filenamelen > 0 )
		{
			strcat( _filePath, _filename );
		}

        return true;
    }
}