#include "LoaderResourceSpineSkeleton.h"

#include "Interface/ResourceBankInterface.h"

#include "ResourceSpineSkeleton.h"

#include "Kernel/AssertionMemoryPanic.h"

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
        ResourceSpineSkeleton * resource = _loadable.getT<ResourceSpineSkeleton *>();

        const ConstString & groupName = resource->getGroupName();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineSkeleton * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineSkeleton *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );
        metadata->getm_File_Converter( content.get(), &ContentInterface::setConverterType );

        ConstString resourceSpineAtlasName;
        metadata->get_Atlas_Name( &resourceSpineAtlasName );

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        const ResourcePtr & resourceSpineAtlas = resourceBank->getResource( groupName, resourceSpineAtlasName );

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
