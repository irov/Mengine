#include "JSONLoaderResourceMovie2.h"

#include "ResourceMovie2.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/JSONDataHelper.h"
#include "Kernel/VocabularyHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceMovie2::JSONLoaderResourceMovie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceMovie2::~JSONLoaderResourceMovie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceMovie2::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceMovie2 * resource = _factorable.getT<ResourceMovie2 *>();

        const ContentInterfacePtr & content = resource->getContent();

        FilePath filePath;
        if( Helper::getJSONMemberFilePath( _json, "File", "Path", &filePath ) == true )
        {
            content->setFilePath( filePath );
        }

        bool noExist = false;
        if( Helper::getJSONMemberBool( _json, "File", "NoExist", &noExist ) == true )
        {
            content->setValidNoExist( noExist );
        }

        ConstString dataflowType;
        if( Helper::getJSONMemberConstString( _json, "File", "Dataflow", &dataflowType ) == false )
        {
            dataflowType = STRINGIZE_STRING_LOCAL( "aezMovie" );
        }

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), dataflowType );

        MENGINE_ASSERTION_MEMORY_PANIC( dataflow, "resource '%s' type '%s' invalid get dataflow '%s'"
            , resource->getName().c_str()
            , resource->getType().c_str()
            , dataflowType.c_str()
        );

        content->setDataflow( dataflow );

        jpp::object j_compositions;
        if( _json.exist( "Composition", &j_compositions ) == false )
        {
            return true;
        }

        if( j_compositions.is_type_array() == false )
        {
            return false;
        }

        for( const jpp::object & meta_composition : jpp::array( j_compositions ) )
        {
            ConstString compositionName;
            float duration;
            float frameDuration;

            if( Helper::getJSONConstString( meta_composition, "Name", &compositionName ) == false ||
                Helper::getJSONFloat( meta_composition, "Duration", &duration ) == false ||
                Helper::getJSONFloat( meta_composition, "FrameDuration", &frameDuration ) == false )
            {
                return false;
            }

            ResourceMovie2::CompositionDesc desc;

            desc.duration = duration * 1000.f;
            desc.frameDuration = frameDuration * 1000.f;
            desc.has_bounds = Helper::getJSONBox2f( meta_composition, "Bounds", &desc.bounds );
            desc.master = meta_composition.get( "Master", true );

            jpp::object j_layers;
            if( meta_composition.exist( "Layer", &j_layers ) == true )
            {
                if( j_layers.is_type_array() == false )
                {
                    return false;
                }

                for( const jpp::object & meta_layer : jpp::array( j_layers ) )
                {
                    ResourceMovie2::CompositionLayer layer;

                    if( Helper::getJSONUInt32( meta_layer, "Index", &layer.index ) == false ||
                        Helper::getJSONConstString( meta_layer, "Name", &layer.name ) == false ||
                        Helper::getJSONConstString( meta_layer, "Type", &layer.type ) == false )
                    {
                        return false;
                    }

                    if( Helper::getJSONMat4f( meta_layer, "Matrix", &layer.matrix ) == false )
                    {
                        layer.matrix = mt::mat4f::identity();
                    }

                    if( Helper::getJSONColor( meta_layer, "Color", &layer.color ) == false )
                    {
                        layer.color = Color::identity();
                    }

                    mt::box2f dimension;
                    if( Helper::getJSONBox2f( meta_layer, "Dimension", &dimension ) == true )
                    {
                        layer.dimension = dimension;
                    }

                    layer.trackMatte = meta_layer.get( "TrackMatte", false );

                    Helper::getJSONUInt32s( meta_layer, "Options", &layer.options );

                    desc.layers.emplace_back( layer );
                }
            }

            jpp::object j_subcompositions;
            if( meta_composition.exist( "SubComposition", &j_subcompositions ) == true )
            {
                if( j_subcompositions.is_type_array() == false )
                {
                    return false;
                }

                for( const jpp::object & meta_subcomposition : jpp::array( j_subcompositions ) )
                {
                    ResourceMovie2::CompositionSubComposition subcomposition;

                    if( Helper::getJSONUInt32( meta_subcomposition, "Index", &subcomposition.index ) == false ||
                        Helper::getJSONConstString( meta_subcomposition, "Name", &subcomposition.name ) == false ||
                        Helper::getJSONFloat( meta_subcomposition, "Duration", &subcomposition.duration ) == false ||
                        Helper::getJSONFloat( meta_subcomposition, "FrameDuration", &subcomposition.frameDuration ) == false )
                    {
                        return false;
                    }

                    desc.subcompositions.emplace_back( subcomposition );
                }
            }

            resource->setCompositionDesc( compositionName, desc );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
