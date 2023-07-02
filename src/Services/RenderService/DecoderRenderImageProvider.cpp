#include "DecoderRenderImageProvider.h"

#include "Interface/RenderTextureServiceInterface.h"

#include "DecoderRenderImageLoader.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DecoderRenderImageProvider::DecoderRenderImageProvider()
        : m_codecFlags( 0 )
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
    RenderImageLoaderInterfacePtr DecoderRenderImageProvider::getLoader( const DocumentInterfacePtr & _doc ) const
    {
        RenderImageLoaderInterfacePtr loader = RENDERTEXTURE_SERVICE()
            ->createDecoderRenderImageLoader( m_fileGroup, m_filePath, m_codecType, m_codecFlags, _doc );

        return loader;
    }
    //////////////////////////////////////////////////////////////////////////
}
