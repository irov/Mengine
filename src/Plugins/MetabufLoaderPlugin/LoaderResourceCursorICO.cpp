#include "LoaderResourceCursorICO.h"

#include "Engine/ResourceCursorICO.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceCursorICO::LoaderResourceCursorICO()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceCursorICO::~LoaderResourceCursorICO()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceCursorICO::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceCursorICO * resource = _loadable.getT<ResourceCursorICO *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorICO * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorICO *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}