#include "DecoderRenderImageProvider.h"

#include "DecoderRenderImageLoader.h"

#include "Factory/FactorableUnique.h"

#include "Logger/Logger.h"

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
    void DecoderRenderImageProvider::initialize( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, const ConstString & _codecName )
    {
        m_fileGroup = _fileGroup;
        m_fileName = _fileName;
        m_codecName = _codecName;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageLoaderInterfacePtr DecoderRenderImageProvider::getLoader() const
    {
        DecoderRenderImageLoaderPtr loader = new FactorableUnique<DecoderRenderImageLoader>();
        
        if( loader->initialize( m_fileGroup, m_fileName, m_codecName ) == false )
        {
            return nullptr;
        }

        return loader;
    }
}	
