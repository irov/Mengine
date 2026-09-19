#pragma once

#include "Kernel/Node.h"
#include "Kernel/Shape.h"
#include "Kernel/SurfaceImage.h"
#include "Kernel/SurfaceImageSequence.h"
#include "Engine/TextField.h"
#include "Plugins/MoviePlugin/Movie2Interface.h"
#include "Plugins/SpinePlugin/SpineInterface.h"
#include "NodeDebuggerSerialization.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeDebugInspector
    {
    public:
        void applyNodeProperties( const NodePtr & _node, const pugi::xml_node & _xmlNode );
        void serializeNode( const NodePtr & _node, pugi::xml_node & _xmlParentNode );
        void serializeNodeSingle( const NodePtr & _node, pugi::xml_node & _xmlNode );
        void serializePickerable( PickerInterface * _picker, pugi::xml_node & _xmlParentNode );
        void serializeRenderable( RenderInterface * _render, pugi::xml_node & _xmlParentNode );
        void serializeTransformation( const TransformablePtr & _transformable, pugi::xml_node & _xmlParentNode );
        void serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode );
        void serializeAnimation( const Compilable * _compilable, const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode );
        void serializeTextField( const TextFieldPtr & _textField, pugi::xml_node & _xmlParentNode );
        void serializeMovie2( const Compilable * _compilable, const UnknownMovie2Interface * _unknownMovie2, pugi::xml_node & _xmlParentNode );
        void serializeSpine( const UnknownSpineInterface * _unknownSpine, pugi::xml_node & _xmlParentNode );
        void serializeShape( const ShapePtr & _shape, pugi::xml_node & _xmlParentNode );
        void serializeSurfaceImage( const SurfaceImagePtr & _surfaceImage, pugi::xml_node & _xmlParentNode );
        void serializeSurfaceImageSequence( const SurfaceImageSequencePtr & _surfaceImageSequence, pugi::xml_node & _xmlParentNode );
        void serializeContent( const ContentInterfacePtr & _content, pugi::xml_node & _xmlParentNode );
    };
    //////////////////////////////////////////////////////////////////////////
}
