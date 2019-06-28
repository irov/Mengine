#include "LoaderResourceTexturepacker.h"

#include "Interface/ResourceServiceInterface.h"

#include "ResourceTexturepacker.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceTexturepacker::LoaderResourceTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceTexturepacker::~LoaderResourceTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceTexturepacker::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceTexturepacker* resource = stdex::intrusive_get<ResourceTexturepacker*>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceTexturepacker* metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceTexturepacker*>(_meta);

        metadata->getm_File_Path( resource, &ResourceTexturepacker::setFilePath );
        metadata->getm_File_Converter( resource, &ResourceTexturepacker::setConverterType );
        
        metadata->getm_Image_Name( resource, &ResourceTexturepacker::setResourceImageName );

        return true;
    }
}
