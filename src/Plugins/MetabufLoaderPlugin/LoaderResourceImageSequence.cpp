#include "LoaderResourceImageSequence.h"

#include "Interface/ResourceBankInterface.h"

#include "Kernel/ResourceImageSequence.h"
#include "Kernel/AssertionResourceType.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageSequence::LoaderResourceImageSequence()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceImageSequence::~LoaderResourceImageSequence()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceImageSequence::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceImageSequence * resource = stdex::intrusive_get<ResourceImageSequence *>( _loadable );

        const ConstString & groupName = resource->getGroupName();

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence *>(_meta);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence::VectorMeta_Sequence & includes_sequence = metadata->get_Includes_Sequence();

        float duration = 0.f;

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence & meta_sequence : includes_sequence )
        {
            const ConstString & resourceName = meta_sequence.get_ResourceImageName();
            float delay = meta_sequence.get_Delay();

            duration += delay;

            MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( resourceName, ResourceImagePtr, false, "resource '%s' type does not match 'ResourceImage'"
                , resourceName.c_str()
            );

            const ResourceImagePtr & resourceImage = resourceBank->getResource( groupName, resourceName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "'%s' group '%s' invalid get sequence image resource '%s'"
                , resource->getName().c_str()
                , resource->getGroupName().c_str()
                , resourceName.c_str()
            );

            resource->addFrame( resourceImage, delay );
        }

        resource->setSequenceDuration( duration );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
