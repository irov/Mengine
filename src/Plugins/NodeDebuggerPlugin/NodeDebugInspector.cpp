#include "NodeDebugInspector.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/AllocatorSystemInterface.h"
#include "Interface/UnknownAllocatorDebugReportInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/FactoryServiceInterface.h"
#include "Interface/SocketSystemInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PickerInterface.h"
#include "Interface/AnimationInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ArrowServiceInterface.h"
#include "Interface/SoundServiceInterface.h"

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/Win32CreateProcess.h"
#endif

#include "NodeDebuggerSerialization.h"

#include "HotSpotPolygonDebuggerBoundingBox.h"
#include "TextFieldDebuggerBoundingBox.h"

#include "Engine/HotSpotPolygon.h"
#include "Engine/HotSpotSurface.h"
#include "Engine/TextField.h"

#include "Kernel/Assertion.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/Scene.h"
#include "Kernel/Surface.h"
#include "Kernel/StringFormat.h"
#include "Kernel/Logger.h"
#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/Data.h"
#include "Kernel/StringHelper.h"
#include "Kernel/RenderCameraHelper.h"
#include "Kernel/ResourceImageSubstract.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/ResolutionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/MixinDebug.h"
#include "Kernel/DebugFileHelper.h"

#include "Config/StdString.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdIterator.h"
#include "Config/DynamicCast.h"
#include "Config/Path.h"

#define NODEDEBUGGERLISTEN_THREAD_NAME "NodeDebuggerListen"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeTransformation( const TransformablePtr & _transformable, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Transformation" );

        const TransformationInterface * transformation = _transformable->getTransformation();

        Detail::serializeNodeProp( transformation->getLocalPosition(), "position", xmlNode );
        Detail::serializeNodeProp( transformation->getLocalOrigin(), "origin", xmlNode );
        Detail::serializeNodeProp( transformation->getLocalSkew(), "skew", xmlNode );
        Detail::serializeNodeProp( transformation->getLocalScale(), "scale", xmlNode );
        Detail::serializeNodeProp( transformation->getLocalOrientation(), "orientation", xmlNode );
        Detail::serializeNodeProp( transformation->getWorldPosition(), "worldPosition", xmlNode );
        Detail::serializeNodeProp( transformation->getWorldScale(), "worldScale", xmlNode );
        Detail::serializeNodeProp( transformation->getWorldOrientation(), "worldOrientation", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlRender = _xmlParentNode.append_child( "Render" );

        Detail::serializeNodeProp( _render->isRenderEnable(), "enable", xmlRender );
        Detail::serializeNodeProp( _render->isHide(), "hide", xmlRender );
        Detail::serializeNodeProp( _render->getLocalColor(), "local_color", xmlRender );
        Detail::serializeNodeProp( _render->getPersonalColor(), "personal_color", xmlRender );
        Detail::serializeNodeProp( _render->getZGroup(), "z_group", xmlRender );
        Detail::serializeNodeProp( _render->getZIndex(), "z_index", xmlRender );
        Detail::serializeNodeProp( Helper::getRenderZGroupInheritance( _render ), "total_z_group", xmlRender );
        Detail::serializeNodeProp( Helper::getRenderZIndexInheritance( _render ), "total_z_index", xmlRender );

        RenderInterface * relationRender = _render->getExtraRelationRender();

        if( relationRender != nullptr )
        {
            Detail::serializeNodeProp( true, "HasExtraRelationRender", xmlRender );

            Renderable * relationRenderable = relationRender->getRenderable();

            Node * relationNode = Helper::dynamicCast<Node *>( relationRenderable );

            Detail::serializeNodeProp( relationNode->getUniqueIdentity(), "BaseRenderNodeUniqueIdentity", xmlRender );
            Detail::serializeNodeProp( relationNode->getName(), "BaseRenderNodeName", xmlRender );
            Detail::serializeNodeProp( Helper::getFactorableType( relationNode ), "BaseRenderNodeType", xmlRender );
        }
        else
        {
            Detail::serializeNodeProp( false, "HasRelationRender", xmlRender );
        }

        const RenderViewportInterfacePtr & viewport = _render->getRenderViewport();

        if( viewport != nullptr )
        {
            pugi::xml_node xmlViewport = xmlRender.append_child( "Viewport" );

            const Viewport & v = viewport->getViewportWM();

            Detail::serializeNodeProp( v.begin, "begin", xmlViewport );
            Detail::serializeNodeProp( v.end, "end", xmlViewport );
        }

        const RenderCameraInterfacePtr & camera = _render->getRenderCamera();

        if( camera != nullptr )
        {
            pugi::xml_node xmlCamera = xmlRender.append_child( "Camera" );

            Detail::serializeNodeProp( MENGINE_MIXIN_DEBUG_NAME( camera.get() ), "Name", xmlCamera );
            Detail::serializeNodeProp( MENGINE_MIXIN_DEBUG_TYPE( camera.get() ), "Type", xmlCamera );
        }

        const RenderCameraInterface * renderCameraInheritance = Helper::getRenderCameraInheritance( _render );

        if( renderCameraInheritance != nullptr )
        {
            pugi::xml_node xmlCamera = xmlRender.append_child( "Camera" );

            Detail::serializeNodeProp( MENGINE_MIXIN_DEBUG_NAME( renderCameraInheritance ), "RelationName", xmlCamera );
            Detail::serializeNodeProp( MENGINE_MIXIN_DEBUG_TYPE( renderCameraInheritance ), "RelationType", xmlCamera );
        }

        const RenderTransformationInterfacePtr & transformation = _render->getRenderTransformation();

        if( transformation != nullptr )
        {
            pugi::xml_node xmlTransformation = xmlRender.append_child( "Transformation" );

            MENGINE_UNUSED( xmlTransformation );
        }

        const RenderScissorInterfacePtr & scissor = _render->getRenderScissor();

        if( scissor != nullptr )
        {
            pugi::xml_node xmlScissor = xmlRender.append_child( "Scissor" );

            const Viewport & v = scissor->getScissorViewportWM();

            Detail::serializeNodeProp( v.begin, "begin", xmlScissor );
            Detail::serializeNodeProp( v.end, "end", xmlScissor );
        }

        const RenderTargetInterfacePtr & target = _render->getRenderTarget();

        if( target != nullptr )
        {
            pugi::xml_node xmlTarget = xmlRender.append_child( "Target" );

            Detail::serializeNodeProp( target->getHWWidth(), "width", xmlTarget );
            Detail::serializeNodeProp( target->getHWHeight(), "height", xmlTarget );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeAnimation( const Compilable * _compilable, const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Animation" );

        Detail::serializeNodeProp( _animation->isLoop(), "loop", xmlNode );
        Detail::serializeNodeProp( _animation->isPlay(), "play", xmlNode );
        Detail::serializeNodeProp( _animation->isPause(), "pause", xmlNode );

        if( _compilable->isCompile() == true )
        {
            Detail::serializeNodeProp( _animation->getTime(), "time", xmlNode );
            Detail::serializeNodeProp( _animation->getDuration(), "duration", xmlNode );
        }
        else
        {
            Detail::serializeNodeProp( 0.f, "time", xmlNode );
            Detail::serializeNodeProp( 0.f, "duration", xmlNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeTextField( const TextFieldPtr & _textField, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:TextField" );

        Detail::serializeNodeProp( _textField->getWrap(), "Wrap", xmlNode );
        Detail::serializeNodeProp( _textField->getAnchorPercent(), "AnchorPercent", xmlNode );
        Detail::serializeNodeProp( _textField->getAnchorVerticalAlign(), "AnchorVerticalAlign", xmlNode );
        Detail::serializeNodeProp( _textField->getAnchorHorizontalAlign(), "AnchorHorizontalAlign", xmlNode );

        const ConstString & textId = _textField->getTextId();
        const ConstString & textAliasEnvironment = _textField->getTextAliasEnvironment();

        Detail::serializeNodeProp( textId, "TextId", xmlNode );
        Detail::serializeNodeProp( textAliasEnvironment, "TextAliasEnvironment", xmlNode );

        if( TEXT_SERVICE()
            ->isTextAlias( textId ) == false )
        {
            Detail::serializeNodeProp( "", "TextAliasId", xmlNode );

            TextEntryInterfacePtr textEntry;
            if( TEXT_SERVICE()
                ->hasTextEntry( textId, &textEntry ) == false )
            {
                const String & text = _textField->getText();

                if( text.empty() == true )
                {
                    Detail::serializeNodeProp( false, "HasText", xmlNode );
                }
                else
                {
                    Detail::serializeNodeProp( true, "HasText", xmlNode );

                    Detail::serializeNodeProp( String(), "Format", xmlNode );
                    Detail::serializeNodeProp( text, "Text", xmlNode );
                }
            }
            else
            {
                Detail::serializeNodeProp( true, "HasText", xmlNode );

                const String & text = textEntry->getValue();

                Detail::serializeNodeProp( text, "Format", xmlNode );

                const VectorTextArguments & textFormatArgs = _textField->getTextArguments();

                String fmt;
                Helper::fillStringFormat( text, textFormatArgs, &fmt );

                Detail::serializeNodeProp( fmt, "Text", xmlNode );
            }
        }
        else if( TEXT_SERVICE()
            ->hasTextAlias( textAliasEnvironment, textId ) == true )
        {
            const ConstString & textAliasId = TEXT_SERVICE()
                ->getTextAlias( textAliasEnvironment, textId );

            Detail::serializeNodeProp( textAliasId, "TextAliasId", xmlNode );

            TextEntryInterfacePtr textEntry;
            if( TEXT_SERVICE()
                ->hasTextEntry( textAliasId, &textEntry ) == false )
            {
                const String & text = _textField->getText();

                if( text.empty() == true )
                {
                    Detail::serializeNodeProp( false, "HasText", xmlNode );
                }
                else
                {
                    Detail::serializeNodeProp( true, "HasText", xmlNode );

                    Detail::serializeNodeProp( String(), "Format", xmlNode );
                    Detail::serializeNodeProp( text, "Text", xmlNode );
                }
            }
            else
            {
                Detail::serializeNodeProp( true, "HasText", xmlNode );

                const String & text = textEntry->getValue();

                Detail::serializeNodeProp( text, "Format", xmlNode );

                VectorTextArguments textFormatArgs;

                if( TEXT_SERVICE()
                    ->getTextAliasArguments( textAliasEnvironment, textId, &textFormatArgs ) == false )
                {
                    textFormatArgs = _textField->getTextArguments();
                }

                String fmt;
                Helper::fillStringFormat( text, textFormatArgs, &fmt );

                Detail::serializeNodeProp( fmt, "Text", xmlNode );
            }
        }
        else
        {
            Detail::serializeNodeProp( "[Not-Found]", "TextAliasId", xmlNode );
        }

        if( _textField->isCompile() == true )
        {
            Detail::serializeNodeProp( _textField->calcFont()->getName(), "TotalFontName", xmlNode );
            Detail::serializeNodeProp( _textField->calcLineOffset(), "TotalLineOffset", xmlNode );
            Detail::serializeNodeProp( _textField->calcCharOffset(), "TotalCharOffset", xmlNode );
            Detail::serializeNodeProp( _textField->calcMaxLength(), "TotalMaxLength", xmlNode );
            Detail::serializeNodeProp( _textField->calcMaxHeight(), "TotalMaxHeight", xmlNode );
            Detail::serializeNodeProp( _textField->calcFontColor(), "TotalFontColor", xmlNode );
            Detail::serializeNodeProp( (uint32_t)_textField->calcHorizontAlign(), "TotalHorizontAlign", xmlNode );
            Detail::serializeNodeProp( (uint32_t)_textField->calcVerticalAlign(), "TotalVerticalAlign", xmlNode );
            Detail::serializeNodeProp( _textField->calcCharScale(), "TotalCharScale", xmlNode );
            Detail::serializeNodeProp( _textField->calcAutoScale(), "TotalAutoScale", xmlNode );
            Detail::serializeNodeProp( _textField->calcJustify(), "TotalJustify", xmlNode );
        }

        const FontInterfacePtr & defaultFont = FONT_SERVICE()
            ->getDefaultFont();

        Detail::serializeNodeProp( _textField->getFont() != nullptr ? _textField->getFont()->getName() : defaultFont->getName(), "FontName", xmlNode );
        Detail::serializeNodeProp( _textField->hasMaxLength(), "HasMaxLength", xmlNode );
        Detail::serializeNodeProp( _textField->getMaxLength(), "MaxLength", xmlNode );
        Detail::serializeNodeProp( _textField->hasMaxHeight(), "HasMaxHeight", xmlNode );
        Detail::serializeNodeProp( _textField->getMaxHeight(), "MaxHeight", xmlNode );
        Detail::serializeNodeProp( _textField->hasAutoScale(), "HasAutoScale", xmlNode );
        Detail::serializeNodeProp( _textField->getAutoScale(), "AutoScale", xmlNode );
        Detail::serializeNodeProp( _textField->hasJustify(), "HasJustify", xmlNode );
        Detail::serializeNodeProp( _textField->getJustify(), "Justify", xmlNode );
        Detail::serializeNodeProp( _textField->hasFontColor(), "HasFontColor", xmlNode );
        Detail::serializeNodeProp( _textField->getFontColor(), "FontColor", xmlNode );
        Detail::serializeNodeProp( _textField->hasLineOffset(), "HasLineOffset", xmlNode );
        Detail::serializeNodeProp( _textField->getLineOffset(), "LineOffset", xmlNode );
        Detail::serializeNodeProp( _textField->hasCharOffset(), "HasCharOffset", xmlNode );
        Detail::serializeNodeProp( _textField->getCharOffset(), "CharOffset", xmlNode );
        Detail::serializeNodeProp( _textField->hasCharScale(), "HasCharScale", xmlNode );
        Detail::serializeNodeProp( _textField->getCharScale(), "CharScale", xmlNode );
        Detail::serializeNodeProp( (uint32_t)_textField->getHorizontAlign(), "HorizontAlign", xmlNode );
        Detail::serializeNodeProp( (uint32_t)_textField->getVerticalAlign(), "VerticalAlign", xmlNode );
        Detail::serializeNodeProp( _textField->getMaxCharCount(), "MaxCharCount", xmlNode );
        Detail::serializeNodeProp( _textField->getPixelsnap(), "Pixelsnap", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeMovie2( const Compilable * _compilable, const UnknownMovie2Interface * _unknownMovie2, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:Movie2" );

        Detail::serializeNodeProp( _unknownMovie2->getCompositionName(), "CompositionName", xmlNode );
        Detail::serializeNodeProp( _unknownMovie2->getTextAliasEnvironment(), "TextAliasEnvironment", xmlNode );

        pugi::xml_node xmlSubCompositions = xmlNode.append_child( "SubCompositions" );

        _unknownMovie2->foreachSubComposition( [this, _compilable, &xmlSubCompositions]( const Movie2SubCompositionInterfacePtr & _subComposition )
        {
            pugi::xml_node xmlElement = xmlSubCompositions.append_child( "Element" );

            Detail::serializeNodeProp( _subComposition->getName(), "SubCompositionName", xmlElement );
            Detail::serializeNodeProp( _subComposition->getSubCompositionEnable(), "SubCompositionEnable", xmlElement );

            AnimationInterface * animation = _subComposition->getAnimation();

            if( animation != nullptr )
            {
                this->serializeAnimation( _compilable, animation, xmlElement );
            }
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeSpine( const UnknownSpineInterface * _unknownSpine, pugi::xml_node & _xmlParentNode )
    {
        const ResourcePtr & resourceSpineSkeleton = _unknownSpine->getResourceSpineSkeleton();

        if( resourceSpineSkeleton == nullptr )
        {
            return;
        }

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:Spine" );

        Detail::serializeNodeProp( resourceSpineSkeleton->getName(), "ResourceName", xmlNode );
        Detail::serializeNodeProp( Helper::getFactorableType( resourceSpineSkeleton ), "ResourceType", xmlNode );

        const ContentInterfacePtr & content = resourceSpineSkeleton->getContent();

        if( content != nullptr && content->getFilePath() != ConstString::none() )
        {
            this->serializeContent( content, xmlNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeShape( const ShapePtr & _shape, pugi::xml_node & _xmlParentNode )
    {
        const SurfacePtr & surface = _shape->getSurface();

        if( surface == nullptr )
        {
            return;
        }

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Component:Surface" );

        Detail::serializeNodeProp( surface->getName(), "Name", xmlNode );
        Detail::serializeNodeProp( Helper::getFactorableType( surface ), "Type", xmlNode );

        if( surface->isCompile() == true )
        {
            Detail::serializeNodeProp( true, "Compile", xmlNode );
            Detail::serializeNodeProp( surface->getMaxSize(), "MaxSize", xmlNode );
            Detail::serializeNodeProp( surface->getSize(), "Size", xmlNode );
            Detail::serializeNodeProp( surface->getOffset(), "Offset", xmlNode );
            Detail::serializeNodeProp( surface->getAnchor(), "Anchor", xmlNode );
        }
        else
        {
            Detail::serializeNodeProp( false, "Compile", xmlNode );
            Detail::serializeNodeProp( mt::vec2f{0.f, 0.f}, "MaxSize", xmlNode );
            Detail::serializeNodeProp( mt::vec2f{0.f, 0.f}, "Size", xmlNode );
            Detail::serializeNodeProp( mt::vec2f{0.f, 0.f}, "Offset", xmlNode );
            Detail::serializeNodeProp( mt::vec2f{0.f, 0.f}, "Anchor", xmlNode );
        }

        const AnimationInterface * animation = surface->getAnimation();

        if( animation != nullptr )
        {
            this->serializeAnimation( surface.get(), animation, xmlNode );
        }

        SurfaceImagePtr surfaceImage = SurfaceImagePtr::dynamic_from( surface );

        if( surfaceImage != nullptr )
        {
            this->serializeSurfaceImage( surfaceImage, xmlNode );
        }

        SurfaceImageSequencePtr surfaceImageSequence = SurfaceImageSequencePtr::dynamic_from( surface );

        if( surfaceImageSequence != nullptr )
        {
            this->serializeSurfaceImageSequence( surfaceImageSequence, xmlNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeSurfaceImage( const SurfaceImagePtr & _surfaceImage, pugi::xml_node & _xmlParentNode )
    {
        const ResourceImagePtr & resourceImage = _surfaceImage->getResourceImage();

        if( resourceImage == nullptr )
        {
            return;
        }

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:SurfaceImage" );

        Detail::serializeNodeProp( resourceImage->getName(), "ResourceName", xmlNode );
        Detail::serializeNodeProp( Helper::getFactorableType( resourceImage ), "ResourceType", xmlNode );
        Detail::serializeNodeProp( resourceImage->getUV( 0 ), "UVImage", xmlNode );

        const ContentInterfacePtr & content = resourceImage->getContent();

        if( content != nullptr && content->getFilePath() != ConstString::none() )
        {
            this->serializeContent( content, xmlNode );
        }

        ResourceImageSubstractPtr resourceImageSubstract = ResourceImageSubstractPtr::dynamic_from( resourceImage );

        if( resourceImageSubstract != nullptr )
        {
            pugi::xml_node xmlNodeAtlas = xmlNode.append_child( "Atlas" );

            const ResourceImagePtr & resourceImageAtlas = resourceImageSubstract->getResourceImage();

            Detail::serializeNodeProp( resourceImageAtlas->getName(), "ResourceName", xmlNodeAtlas );
            Detail::serializeNodeProp( Helper::getFactorableType( resourceImageAtlas ), "ResourceType", xmlNodeAtlas );

            const ContentInterfacePtr & atlasContent = resourceImageAtlas->getContent();

            if( atlasContent != nullptr && atlasContent->getFilePath() != ConstString::none() )
            {
                this->serializeContent( atlasContent, xmlNodeAtlas );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeSurfaceImageSequence( const SurfaceImageSequencePtr & _surfaceImageSequence, pugi::xml_node & _xmlParentNode )
    {
        const ResourceImageSequencePtr & resourceImageSequence = _surfaceImageSequence->getResourceImageSequence();

        if( resourceImageSequence == nullptr )
        {
            return;
        }

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Type:SurfaceImageSequence" );

        Detail::serializeNodeProp( resourceImageSequence->getName(), "ResourceName", xmlNode );
        Detail::serializeNodeProp( Helper::getFactorableType( resourceImageSequence ), "ResourceType", xmlNode );

        Detail::serializeNodeProp( resourceImageSequence->getSequenceCount(), "ResourceSequenceCount", xmlNode );
        Detail::serializeNodeProp( resourceImageSequence->getSequenceDuration(), "ResourceSequenceDuration", xmlNode );

        Detail::serializeNodeProp( resourceImageSequence->getSequenceCount(), "ResourceSequenceCount", xmlNode );
        Detail::serializeNodeProp( resourceImageSequence->getSequenceDuration(), "ResourceSequenceDuration", xmlNode );

        Detail::serializeNodeProp( _surfaceImageSequence->getCurrentFrame(), "CurrentFrame", xmlNode );

        const ContentInterfacePtr & content = resourceImageSequence->getContent();

        if( content != nullptr )
        {
            this->serializeContent( content, xmlNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeContent( const ContentInterfacePtr & _content, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Content" );

        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();

        Detail::serializeNodeProp( fileGroup->getName(), "FileGroup", xmlNode );

        const FilePath & filePath = _content->getFilePath();

        Path fullPath = {'\0'};
        fileGroup->getFullPath( filePath, fullPath );

        Detail::serializeNodeProp( fullPath, "FilePath", xmlNode );
        Detail::serializeNodeProp( _content->getCodecType(), "CodecType", xmlNode );
        Detail::serializeNodeProp( _content->getConverterType(), "ConverterType", xmlNode );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeNode( const NodePtr & _node, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Node" );

        this->serializeNodeSingle( _node, xmlNode );

        if( _node->isEmptyChildren() == false )
        {
            pugi::xml_node xmlChildrenContainer = xmlNode.append_child( "Children" );

            _node->foreachChildren( [this, &xmlChildrenContainer]( const NodePtr & _child )
            {
                this->serializeNode( _child, xmlChildrenContainer );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeNodeSingle( const NodePtr & _node, pugi::xml_node & _xmlNode )
    {
        Detail::serializeNodeProp( _node->getHierarchyHash(), "hhash", _xmlNode );
        Detail::serializeNodeProp( _node->getUniqueIdentity(), "uid", _xmlNode );
        Detail::serializeNodeProp( _node->getName(), "name", _xmlNode );
        Detail::serializeNodeProp( Helper::getFactorableType( _node ), "type", _xmlNode );
        Detail::serializeNodeProp( _node->isEnable(), "enable", _xmlNode );

        this->serializeTransformation( _node, _xmlNode );

        RenderInterface * render = _node->getRender();

        if( render != nullptr )
        {
            this->serializeRender( render, _xmlNode );
        }

        AnimationInterface * animation = _node->getAnimation();

        if( animation != nullptr )
        {
            this->serializeAnimation( _node.get(), animation, _xmlNode );
        }

        ShapePtr shape = ShapePtr::dynamic_from( _node );

        if( shape != nullptr )
        {
            this->serializeShape( shape, _xmlNode );
        }

        TextFieldPtr textField = TextFieldPtr::dynamic_from( _node );

        if( textField != nullptr )
        {
            this->serializeTextField( textField, _xmlNode );
        }

        const UnknownMovie2Interface * unknownMovie2 = _node->getDynamicUnknown();

        if( unknownMovie2 != nullptr )
        {
            this->serializeMovie2( _node.get(), unknownMovie2, _xmlNode );
        }

        const UnknownSpineInterface * unknownSpine = _node->getDynamicUnknown();

        if( unknownSpine != nullptr )
        {
            this->serializeSpine( unknownSpine, _xmlNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializePickerable( PickerInterface * _picker, pugi::xml_node & _xmlParentNode )
    {
        Pickerable * pickerable = _picker->getPickerable();

        Node * nodeChild = Helper::dynamicCast<Node *>( pickerable );

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Node" );

        if( nodeChild != nullptr )
        {
            this->serializeNodeSingle( NodePtr::from( nodeChild ), xmlNode );
        }

        if( _picker->isEmptyPickerChildren() == false )
        {
            pugi::xml_node xmlChildrenContainer = xmlNode.append_child( "Children" );

            _picker->foreachPickerChildren( [this, &xmlChildrenContainer]( PickerInterface * _child )
            {
                this->serializePickerable( _child, xmlChildrenContainer );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::serializeRenderable( RenderInterface * _render, pugi::xml_node & _xmlParentNode )
    {
        Renderable * renderable = _render->getRenderable();

        Node * nodeChild = Helper::dynamicCast<Node *>( renderable );

        pugi::xml_node xmlNode = _xmlParentNode.append_child( "Node" );

        if( nodeChild != nullptr )
        {
            this->serializeNodeSingle( NodePtr::from( nodeChild ), xmlNode );
        }

        if( _render->isEmptyRenderChildren() == false )
        {
            pugi::xml_node xmlChildrenContainer = xmlNode.append_child( "Children" );

            _render->foreachRenderChildren( [this, &xmlChildrenContainer]( RenderInterface * _child )
            {
                this->serializeRenderable( _child, xmlChildrenContainer );
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebugInspector::applyNodeProperties( const NodePtr & _node, const pugi::xml_node & _xmlNode )
    {
        if( _node != nullptr )
        {
            Detail::deserializeNodeProp<bool>( "enable", _xmlNode, [_node]( bool _value )
            {
                if( _value == _node->isEnable() )
                {
                    return;
                }

                if( _value == true )
                {
                    _node->enable();
                }
                else
                {
                    _node->disable();
                }
            } );

            pugi::xml_node transformationNode = _xmlNode.child( "Transformation" );

            TransformationInterface * transformation = _node->getTransformation();

            Detail::deserializeNodeProp<mt::vec3f>( "position", transformationNode, [transformation]( const mt::vec3f & _value )
            {
                transformation->setLocalPosition( _value );
            } );

            Detail::deserializeNodeProp<mt::vec3f>( "origin", transformationNode, [transformation]( const mt::vec3f & _value )
            {
                transformation->setLocalOrigin( _value );
            } );

            Detail::deserializeNodeProp<mt::vec2f>( "skew", transformationNode, [transformation]( const mt::vec2f & _value )
            {
                transformation->setLocalSkew( _value );
            } );

            Detail::deserializeNodeProp<mt::vec3f>( "scale", transformationNode, [transformation]( const mt::vec3f & _value )
            {
                transformation->setLocalScale( _value );
            } );

            Detail::deserializeNodeProp<mt::vec3f>( "orientation", transformationNode, [transformation]( const mt::vec3f & _value )
            {
                transformation->setLocalOrientation( _value );
            } );

            pugi::xml_node renderNode = _xmlNode.child( "Render" );

            if( renderNode.empty() == false )
            {
                RenderInterface * render = _node->getRender();

                Detail::deserializeNodeProp<bool>( "hide", renderNode, [render]( bool _value )
                {
                    render->setHide( _value );
                } );

                Detail::deserializeNodeProp<Color>( "local_color", renderNode, [render]( const Color & _value )
                {
                    render->setLocalColor( _value );
                } );

                Detail::deserializeNodeProp<Color>( "personal_color", renderNode, [render]( const Color & _value )
                {
                    render->setPersonalColor( _value );
                } );

                Detail::deserializeNodeProp<ZGroupType>( "z_group", renderNode, [render]( ZGroupType _value )
                {
                    render->setZGroup( _value );
                } );

                Detail::deserializeNodeProp<ZIndexType>( "z_index", renderNode, [render]( ZIndexType _value )
                {
                    render->setZIndex( _value );
                } );
            }

            pugi::xml_node animationNode = _xmlNode.child( "Animation" );

            if( animationNode.empty() == false )
            {
                AnimationInterface * animation = _node->getAnimation();

                Detail::deserializeNodeProp<bool>( "loop", animationNode, [animation]( bool _value )
                {
                    animation->setLoop( _value );
                } );

                Detail::deserializeNodeProp<float>( "time", animationNode, [animation]( float _value )
                {
                    if( _value < 0.f )
                    {
                        _value = 0.f;
                    }
                    else if( _value > animation->getDuration() )
                    {
                        _value = animation->getDuration();
                    }

                    animation->setTime( _value );
                } );
            }

            pugi::xml_node typeNodeTextField = _xmlNode.child( "Type:TextField" );

            if( typeNodeTextField.empty() == false )
            {
                TextFieldPtr textField = TextFieldPtr::from( _node );

                Detail::deserializeNodeProp<float>( "MaxLength", typeNodeTextField, [textField]( float _value )
                {
                    if( _value > 0.f )
                    {
                        textField->setMaxLength( _value );
                    }
                    else
                    {
                        textField->removeMaxLength();
                    }
                } );

                Detail::deserializeNodeProp<bool>( "Wrap", typeNodeTextField, [textField]( bool _value )
                {
                    textField->setWrap( _value );
                } );

                Detail::deserializeNodeProp<bool>( "AutoScale", typeNodeTextField, [textField]( bool _value )
                {
                    textField->setAutoScale( _value );
                } );

                Detail::deserializeNodeProp<bool>( "Justify", typeNodeTextField, [textField]( bool _value )
                {
                    textField->setJustify( _value );
                } );

                Detail::deserializeNodeProp<mt::vec2f>( "AnchorPercent", typeNodeTextField, [textField]( const mt::vec2f & _value )
                {
                    textField->setAnchorPercent( _value );
                } );

                Detail::deserializeNodeProp<bool>( "AnchorHorizontalAlign", typeNodeTextField, [textField]( bool _value )
                {
                    textField->setAnchorHorizontalAlign( _value );
                } );

                Detail::deserializeNodeProp<bool>( "AnchorVerticalAlign", typeNodeTextField, [textField]( bool _value )
                {
                    textField->setAnchorVerticalAlign( _value );
                } );

                Detail::deserializeNodeProp<ConstString>( "TextId", typeNodeTextField, [textField]( const ConstString & _value )
                {
                    textField->setTextId( _value );
                } );

                Detail::deserializeNodeProp<ConstString>( "TextAliasEnvironment", typeNodeTextField, [textField]( const ConstString & _value )
                {
                    textField->setTextAliasEnvironment( _value );
                } );

                Detail::deserializeNodeProp<ConstString>( "FontName", typeNodeTextField, [textField]( const ConstString & _value )
                {
                    FontInterfacePtr font;

                    if( FONT_SERVICE()
                        ->existFont( _value, &font ) == false )
                    {
                        return;
                    }

                    textField->setFont( font );
                } );

                bool HasFontColor = false;
                Detail::deserializeNodeProp<bool>( "HasFontColor", typeNodeTextField, [&HasFontColor]( bool _value )
                {
                    HasFontColor = _value;
                } );

                if( typeNodeTextField.child( "HasFontColor" ) )
                {
                    if( HasFontColor == true )
                    {
                        Detail::deserializeNodeProp<Color>( "FontColor", typeNodeTextField, [textField]( const Color & _value )
                        {
                            textField->setFontColor( _value );
                        } );
                    }
                    else
                    {
                        textField->removeFontColor();
                    }
                }

                bool HasLineOffset = false;
                Detail::deserializeNodeProp<bool>( "HasLineOffset", typeNodeTextField, [&HasLineOffset]( bool _value )
                {
                    HasLineOffset = _value;
                } );

                if( typeNodeTextField.child( "HasLineOffset" ) )
                {
                    if( HasLineOffset == true )
                    {
                        Detail::deserializeNodeProp<float>( "LineOffset", typeNodeTextField, [textField]( float _value )
                        {
                            textField->setLineOffset( _value );
                        } );
                    }
                    else
                    {
                        textField->removeLineOffset();
                    }
                }

                bool HasCharOffset = false;
                Detail::deserializeNodeProp<bool>( "HasCharOffset", typeNodeTextField, [&HasCharOffset]( bool _value )
                {
                    HasCharOffset = _value;
                } );

                if( typeNodeTextField.child( "HasCharOffset" ) )
                {
                    if( HasCharOffset == true )
                    {
                        Detail::deserializeNodeProp<float>( "CharOffset", typeNodeTextField, [textField]( float _value )
                        {
                            textField->setCharOffset( _value );
                        } );
                    }
                    else
                    {
                        textField->removeCharOffset();
                    }
                }

                bool HasCharScale = false;
                Detail::deserializeNodeProp<bool>( "HasCharScale", typeNodeTextField, [&HasCharScale]( bool _value )
                {
                    HasCharScale = _value;
                } );

                if( typeNodeTextField.child( "HasCharScale" ) )
                {
                    if( HasCharScale == true )
                    {
                        Detail::deserializeNodeProp<float>( "CharScale", typeNodeTextField, [textField]( float _value )
                        {
                            textField->setCharScale( _value );
                        } );
                    }
                    else
                    {
                        textField->removeCharScale();
                    }
                }

                Detail::deserializeNodeProp<uint32_t>( "HorizontAlign", typeNodeTextField, [textField]( uint32_t _value )
                {
                    textField->setHorizontAlign( (ETextHorizontAlign)_value );
                } );

                Detail::deserializeNodeProp<uint32_t>( "VerticalAlign", typeNodeTextField, [textField]( uint32_t _value )
                {
                    textField->setVerticalAlign( (ETextVerticalAlign)_value );
                } );

                Detail::deserializeNodeProp<uint32_t>( "MaxCharCount", typeNodeTextField, [textField]( uint32_t _value )
                {
                    textField->setMaxCharCount( _value );
                } );

                Detail::deserializeNodeProp<bool>( "Pixelsnap", typeNodeTextField, [textField]( bool _value )
                {
                    textField->setPixelsnap( _value );
                } );
            }

            pugi::xml_node typeNodeMovie2 = _xmlNode.child( "Type:Movie2" );

            if( typeNodeMovie2.empty() == false )
            {
                UnknownMovie2Interface * unknownMovie2 = _node->getUnknown();

                Detail::deserializeNodeProp<ConstString>( "TextAliasEnvironment", typeNodeMovie2, [unknownMovie2]( const ConstString & _value )
                {
                    unknownMovie2->setTextAliasEnvironment( _value );
                } );

                pugi::xml_node xmlSubCompositions = typeNodeMovie2.child( "SubCompositions" );

                if( xmlSubCompositions.empty() == false )
                {
                    for( pugi::xml_node xmlElement : xmlSubCompositions.children( "Element" ) )
                    {
                        const Char * SubCompositionName = xmlElement.attribute( "SubCompositionName" ).as_string();

                        const Movie2SubCompositionInterfacePtr & subComposition = unknownMovie2->getSubComposition( Helper::stringizeString( SubCompositionName ) );

                        Detail::deserializeNodeProp<bool>( "SubCompositionEnable", xmlElement, [subComposition]( bool _value )
                        {
                            subComposition->setSubCompositionEnable( _value );
                        } );

                        //Detail::serializeNodeProp( _subComposition->getName(), "SubCompositionName", xmlElement );
                        //Detail::serializeNodeProp( _subComposition->getSubCompositionEnable(), "SubCompositionEnable", xmlElement );

                        //AnimationInterface * animation = _subComposition->getAnimation();
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
