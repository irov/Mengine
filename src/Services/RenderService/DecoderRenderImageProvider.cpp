#include "DecoderRenderImageProvider.h"

#include "DecoderRenderImageLoader.h"

#include "Kernel/FactorableUnique.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageProvider::DecoderRenderImageProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageProvider::~DecoderRenderImageProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////    
    void DecoderRenderImageProvider::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, uint32_t _codecFlags )
    {
        m_fileGroup = _fileGroup;
        m_filePath = _filePath;
        m_codecType = _codecType;
        m_codecFlags = _codecFlags;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLoaderInterfacePtr DecoderRenderImageProvider::getLoader() const
    {
        DecoderRenderImageLoaderPtr loader = Helper::makeFactorableUnique<DecoderRenderImageLoader>();

        if( loader->initialize( m_fileGroup, m_filePath, m_codecType, m_codecFlags ) == false )
        {
            return nullptr;
        }

        return loader;
    }
}
