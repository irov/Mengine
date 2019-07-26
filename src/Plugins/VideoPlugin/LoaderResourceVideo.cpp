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
        ResourceVideo * resource = stdex::intrusive_get<ResourceVideo *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceVideo * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceVideo *>(_meta);

        metadata->getm_File_Path( resource, &ResourceVideo::setFilePath );
        metadata->getm_File_Codec( resource, &ResourceVideo::setCodecType );
        metadata->getm_File_Converter( resource, &ResourceVideo::setConverterType );
        metadata->getm_File_Alpha( resource, &ResourceVideo::setAlpha );
        metadata->getm_File_NoSeek( resource, &ResourceVideo::setNoSeek );
        metadata->getm_File_FrameRate( resource, &ResourceVideo::setFrameRate );
        metadata->getm_File_Duration( resource, &ResourceVideo::setDuration );

        return true;
    }
}
