#include "LoaderResourceVideo.h"

#include "ResourceVideo.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceVideo::LoaderResourceVideo()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceVideo::~LoaderResourceVideo()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceVideo::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceVideo * resource = _loadable.getT<ResourceVideo *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceVideo * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceVideo *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );
        metadata->getm_File_Codec( content.get(), &ContentInterface::setCodecType );
        metadata->getm_File_Converter( content.get(), &ContentInterface::setConverterType );
        metadata->getm_File_NoExist( content.get(), &ContentInterface::setValidNoExist );

        metadata->getm_File_Alpha( resource, &ResourceVideo::setAlpha );
        metadata->getm_File_Premultiply( resource, &ResourceVideo::setPremultiply );
        metadata->getm_File_NoSeek( resource, &ResourceVideo::setNoSeek );
        metadata->getm_File_FrameRate( resource, &ResourceVideo::setFrameRate );
        metadata->getm_File_Duration( resource, &ResourceVideo::setDuration );
        metadata->getm_File_Resize( resource, &ResourceVideo::setResize );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
