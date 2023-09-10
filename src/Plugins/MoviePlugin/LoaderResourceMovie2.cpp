#include "LoaderResourceMovie2.h"

#include "Interface/VocabularyServiceInterface.h"

#include "ResourceMovie2.h"

#include "Kernel/FilePathHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceMovie2::LoaderResourceMovie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceMovie2::~LoaderResourceMovie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceMovie2::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceMovie2 * resource = _factorable.getT<ResourceMovie2 *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2 * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2 *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );

        ConstString dataflowType;
        if( metadata->get_File_Dataflow( &dataflowType ) == false )
        {
            dataflowType = STRINGIZE_FILEPATH_LOCAL( "aezMovie" );
        }

        const DataflowInterfacePtr & dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), dataflowType );

        MENGINE_ASSERTION_MEMORY_PANIC( dataflow );

        content->setDataflow( dataflow );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::VectorMeta_Composition & includes_composition = metadata->get_Includes_Composition();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition & meta_composition : includes_composition )
        {
            const ConstString & compositionName = meta_composition.get_Name();

            ResourceMovie2::CompositionDesc desc;

            //ToDo convert to milliseconds
            desc.duration = meta_composition.get_Duration() * 1000.f;
            desc.frameDuration = meta_composition.get_FrameDuration() * 1000.f;

            desc.has_bounds = meta_composition.get_Bounds( &desc.bounds );

            desc.master = meta_composition.getd_Master( true );

            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::VectorMeta_Layer & includes_layer = meta_composition.get_Includes_Layer();

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer & meta_layer : includes_layer )
            {
                ResourceMovie2::CompositionLayer layer;
                layer.index = meta_layer.get_Index();
                layer.name = meta_layer.get_Name();
                layer.type = meta_layer.get_Type();
                meta_layer.getd_Matrix( &layer.matrix, mt::mat4f::identity() );
                meta_layer.getd_Color( &layer.color, Color::identity() );

                mt::box2f dimension;
                if( meta_layer.get_Dimension( &dimension ) == true )
                {
                    layer.dimension = dimension;
                }

                meta_layer.get_Options( &layer.options );

                desc.layers.emplace_back( layer );
            }

            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::VectorMeta_SubComposition & includes_subcomposition = meta_composition.get_Includes_SubComposition();

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition & meta_subcomposition : includes_subcomposition )
            {
                uint32_t subcompositionIndex = meta_subcomposition.get_Index();
                const ConstString & subcompositionName = meta_subcomposition.get_Name();
                float subcompositionDuration = meta_subcomposition.get_Duration();
                float subcompositionFrameDuration = meta_subcomposition.get_FrameDuration();

                ResourceMovie2::CompositionSubComposition subcomposition;
                subcomposition.index = subcompositionIndex;
                subcomposition.name = subcompositionName;
                subcomposition.duration = subcompositionDuration;
                subcomposition.frameDuration = subcompositionFrameDuration;

                desc.subcompositions.emplace_back( subcomposition );
            }

            resource->setCompositionDesc( compositionName, desc );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}