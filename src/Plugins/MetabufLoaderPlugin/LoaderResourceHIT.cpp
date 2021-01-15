#include "LoaderResourceHIT.h"

#include "Engine/ResourceHIT.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceHIT::LoaderResourceHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceHIT::~LoaderResourceHIT()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceHIT::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceHIT * resource = stdex::intrusive_get<ResourceHIT *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceHIT * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceHIT *>(_meta);

        ContentInterface * content = resource->getContent();

        metadata->getm_File_Path( content, &ContentInterface::setFilePath );
        metadata->getm_File_Converter( content, &ContentInterface::setConverterType );
        metadata->getm_File_Codec( content, &ContentInterface::setCodecType );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
