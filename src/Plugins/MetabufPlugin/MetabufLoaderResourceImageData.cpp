#include "MetabufLoaderResourceImageData.h"

#include "Interface/CodecServiceInterface.h"

#include "Kernel/ResourceImageData.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceImageData::MetabufLoaderResourceImageData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceImageData::~MetabufLoaderResourceImageData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceImageData::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceImageData * resource = _factorable.getT<ResourceImageData *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageData * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageData *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );

        ConstString codecType;
        if( metadata->get_File_Codec( &codecType ) == false )
        {
            const FilePath & filePath = content->getFilePath();

            codecType = CODEC_SERVICE()
                ->findCodecType( filePath );
        }

        content->setCodecType( codecType );

        metadata->getm_File_NoExist( content.get(), &ContentInterface::setValidNoExist );

        const mt::vec2f & maxSize = metadata->get_File_MaxSize();

        uint32_t width = (uint32_t)(maxSize.x + 0.5f);
        uint32_t height = (uint32_t)(maxSize.y + 0.5f);

        resource->setImageMaxSize( maxSize );
        resource->setImageWidth( width );
        resource->setImageHeight( height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
