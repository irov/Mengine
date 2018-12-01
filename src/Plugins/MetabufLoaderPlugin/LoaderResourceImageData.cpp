#include "LoaderResourceImageData.h"

#include "Interface/CodecServiceInterface.h"

#include "Engine/ResourceImageData.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageData::LoaderResourceImageData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageData::~LoaderResourceImageData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceImageData::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceImageData * resource = stdex::intrusive_get<ResourceImageData *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageData * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageData *>(_meta);

        const FilePath & filePath = metadata->get_File_Path();
        resource->setFilePath( filePath );

        ConstString codecType;
        if( metadata->get_File_Codec( &codecType ) == false )
        {
            codecType = CODEC_SERVICE()
                ->findCodecType( filePath );
        }

        resource->setCodecType( codecType );

        const mt::vec2f & maxSize = metadata->get_File_MaxSize();

        uint32_t width = (uint32_t)(maxSize.x + 0.5f);
        uint32_t height = (uint32_t)(maxSize.y + 0.5f);

        resource->setImageMaxSize( maxSize );
        resource->setImageWidth( width );
        resource->setImageHeight( height );

        metadata->getm_File_NoExist( resource, &ResourceImageData::setValidNoExist );

        return true;
    }
}
