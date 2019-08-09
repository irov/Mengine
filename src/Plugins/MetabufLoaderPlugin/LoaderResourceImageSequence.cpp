#include "LoaderResourceImageSequence.h"

#include "Engine/ResourceImageSequence.h"

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

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence *>(_meta);

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence::VectorMeta_Sequence & includes_sequence = metadata->get_Includes_Sequence();

        float duration = 0.f;

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceImageSequence::Meta_Sequence & meta_sequence : includes_sequence )
        {
            const ConstString & resourceName = meta_sequence.get_ResourceImageName();
            float delay = meta_sequence.get_Delay();

            duration += delay;

            resource->addFrame( resourceName, delay );
        }

        resource->setSequenceDuration( duration );

        return true;
    }
}
