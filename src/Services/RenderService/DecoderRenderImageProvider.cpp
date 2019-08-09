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
    void DecoderRenderImageProvider::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName, const ConstString & _codecName )
    {
        m_fileGroup = _fileGroup;
        m_fileName = _fileName;
        m_codecName = _codecName;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLoaderInterfacePtr DecoderRenderImageProvider::getLoader() const
    {
        DecoderRenderImageLoaderPtr loader = Helper::makeFactorableUnique<DecoderRenderImageLoader>();

        if( loader->initialize( m_fileGroup, m_fileName, m_codecName ) == false )
        {
            return nullptr;
        }

        return loader;
    }
}
