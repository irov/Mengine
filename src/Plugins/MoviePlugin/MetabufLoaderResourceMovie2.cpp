#include "MetabufLoaderResourceMovie2.h"

#include "ResourceMovie2.h"

#include "Kernel/FilePathHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/VocabularyHelper.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceMovie2::MetabufLoaderResourceMovie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderResourceMovie2::~MetabufLoaderResourceMovie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderResourceMovie2::load( const FactorablePtr & _factorable, const Metabuf::Metadata * _meta )
    {
        ResourceMovie2 * resource = _factorable.getT<ResourceMovie2 *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2 * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2 *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );
        metadata->getm_File_NoExist( content.get(), &ContentInterface::setValidNoExist );

        ConstString dataflowType;
        if( metadata->get_File_Dataflow( &dataflowType ) == false )
        {
            dataflowType = STRINGIZE_FILEPATH_LOCAL( "aezMovie" );
        }

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), dataflowType );

        MENGINE_ASSERTION_MEMORY_PANIC( dataflow, "resource '%s' type '%s' invalid get dataflow '%s'"
            , resource->getName().c_str()
            , Helper::getFactorableType( resource ).c_str()
            , dataflowType.c_str()
        );

        content->setDataflow( dataflow );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::VectorMeta_Composition & includesComposition = metadata->get_Includes_Composition();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition & metaComposition : includesComposition )
        {
            const ConstString & compositionName = metaComposition.get_Name();

            ResourceMovie2::CompositionDesc desc;

            desc.duration = metaComposition.get_Duration() * 1000.f;
            desc.frameDuration = metaComposition.get_FrameDuration() * 1000.f;

            desc.hasBounds = metaComposition.get_Bounds( &desc.bounds );

            desc.master = metaComposition.getd_Master( true );

            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::VectorMeta_Layer & includesLayer = metaComposition.get_Includes_Layer();

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer & metaLayer : includesLayer )
            {
                ResourceMovie2::CompositionLayer layer;
                layer.index = metaLayer.get_Index();
                layer.name = metaLayer.get_Name();
                layer.type = metaLayer.get_Type();
                metaLayer.getd_Matrix( &layer.matrix, mt::mat4f::identity() );
                metaLayer.getd_Color( &layer.color, Color::identity() );

                mt::box2f dimension;
                if( metaLayer.get_Dimension( &dimension ) == true )
                {
                    layer.dimension = dimension;
                }

                layer.trackMatte = metaLayer.get_TrackMatte();

                metaLayer.get_Options( &layer.options );

                desc.layers.emplace_back( layer );
            }

            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::VectorMeta_SubComposition & includesSubcomposition = metaComposition.get_Includes_SubComposition();

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition & metaSubcomposition : includesSubcomposition )
            {
                uint32_t subcompositionIndex = metaSubcomposition.get_Index();
                const ConstString & subcompositionName = metaSubcomposition.get_Name();
                float subcompositionDuration = metaSubcomposition.get_Duration();
                float subcompositionFrameDuration = metaSubcomposition.get_FrameDuration();

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
