#include "LoaderResourceJSON.h"

#include "ResourceJSON.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceJSON::LoaderResourceJSON()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceJSON::~LoaderResourceJSON()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceJSON::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceJSON * resource = stdex::intrusive_get<ResourceJSON *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceJSON * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceJSON *>(_meta);

        ContentInterface * content = resource->getContent();

        metadata->getm_File_Path( content, &ContentInterface::setFilePath );
        metadata->getm_File_Converter( content, &ContentInterface::setConverterType );

        return true;
    }
}
