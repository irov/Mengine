#   include "SDLLayer.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SDLLayer, Menge::SDLLayerInterface, Menge::SDLLayer );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
	SDLLayer::SDLLayer()
        : m_serviceProvider(nullptr)
    {
    }
    //////////////////////////////////////////////////////////////////////////
	SDLLayer::~SDLLayer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLLayer::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * SDLLayer::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLLayer::concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, Char * _filePath, size_t _capacity )
    {
        size_t folderSize = _folder.size();
		size_t fileNameSize = _fileName.size();

        if( folderSize + fileNameSize > _capacity )
        {
            return false;
        }

        strcpy( _filePath, _folder.c_str() );
        strcat( _filePath, _fileName.c_str() );

        return true;
    }
}