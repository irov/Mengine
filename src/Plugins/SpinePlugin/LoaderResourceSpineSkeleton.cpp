#include "LoaderResourceSpineSkeleton.h"

#include "Interface/ResourceBankInterface.h"

#include "ResourceSpineSkeleton.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSpineSkeleton::LoaderResourceSpineSkeleton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSpineSkeleton::~LoaderResourceSpineSkeleton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceSpineSkeleton::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceSpineSkeleton * resource = stdex::intrusive_get<ResourceSpineSkeleton *>( _loadable );

        const ConstString & groupName = resource->getGroupName();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineSkeleton * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineSkeleton *>(_meta);

        ContentInterface * content = resource->getContent();

        metadata->getm_File_Path( content, &ContentInterface::setFilePath );
        metadata->getm_File_Converter( content, &ContentInterface::setConverterType );

        ConstString resourceSpineAtlasName;
        metadata->get_Atlas_Name( &resourceSpineAtlasName );

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        const ResourcePtr & resourceSpineAtlas = resourceBank->getResourceReference( groupName, resourceSpineAtlasName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceSpineAtlas, "'%s' group '%s' invalid get resource '%s'"
            , resource->getName().c_str()
            , resource->getGroupName().c_str()
            , resourceSpineAtlasName.c_str()
        );

        resource->setResourceSpineAtlas( resourceSpineAtlas );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
