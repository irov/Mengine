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

        metadata->getm_File_Path( resource, &ResourceHIT::setFilePath );
        metadata->getm_File_Converter( resource, &ResourceHIT::setConverterType );
        metadata->getm_File_Codec( resource, &ResourceHIT::setCodecType );

        return true;
    }
}
