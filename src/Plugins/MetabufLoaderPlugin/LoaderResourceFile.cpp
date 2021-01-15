#include "LoaderResourceFile.h"

#include "Engine/ResourceFile.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceFile::LoaderResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceFile::~LoaderResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceFile::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceFile * resource = stdex::intrusive_get<ResourceFile *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceFile * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceFile *>(_meta);

        ContentInterface * content = resource->getContent();

        metadata->getm_File_Path( content, &ContentInterface::setFilePath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
