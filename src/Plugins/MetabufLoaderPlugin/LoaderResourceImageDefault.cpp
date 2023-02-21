#include "LoaderResourceImageDefault.h"

#include "Interface/CodecServiceInterface.h"

#include "Kernel/ResourceImageDefault.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageDefault::LoaderResourceImageDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageDefault::~LoaderResourceImageDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceImageDefault::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceImageDefault * resource = _loadable.getT<ResourceImageDefault *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageDefault * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageDefault *>(_meta);

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

        metadata->getm_File_Converter( content.get(), &ContentInterface::setConverterType );
        metadata->getm_File_NoExist( content.get(), &ContentInterface::setValidNoExist );

        metadata->getm_File_Alpha( resource, &ResourceImage::setAlpha );
        metadata->getm_File_Premultiply( resource, &ResourceImage::setPremultiply );
        metadata->getm_File_TrimAtlas( resource, &ResourceImage::setTrimAtlas );

        const mt::vec2f & maxSize = metadata->get_File_MaxSize();
        resource->setMaxSize( maxSize );

        mt::vec2f size;
        if( metadata->get_File_Size( &size ) == true )
        {
            resource->setSize( size );
        }
        else
        {
            resource->setSize( maxSize );
        }

        metadata->getm_File_Offset( resource, &ResourceImage::setOffset );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
