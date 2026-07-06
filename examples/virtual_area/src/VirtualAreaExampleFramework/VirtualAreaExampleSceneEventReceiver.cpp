#include "VirtualAreaExampleSceneEventReceiver.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/RandomizerInterface.h"
#include "Interface/TransformationInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Entity.h"
#include "Kernel/NodeCast.h"
#include "Kernel/Shape.h"
#include "Kernel/SurfaceSolidColor.h"

#include "Config/StdMath.h"

#include "imgui.h"

namespace Mengine
{
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        constexpr float VIRTUAL_AREA_EXAMPLE_MAX_SCALE_FACTOR_LIMIT = 64.f;
        //////////////////////////////////////////////////////////////////////////
        static float s_width( const float * _rect )
        {
            return StdMath::fabsf( _rect[2] - _rect[0] );
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_height( const float * _rect )
        {
            return StdMath::fabsf( _rect[3] - _rect[1] );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    VirtualAreaExampleSceneEventReceiver::VirtualAreaExampleSceneEventReceiver()
        : m_scene( nullptr )
    {
        m_viewportInput[0] = 120.f;
        m_viewportInput[1] = 90.f;
        m_viewportInput[2] = 900.f;
        m_viewportInput[3] = 610.f;

        m_contentInput[0] = -900.f;
        m_contentInput[1] = -520.f;
        m_contentInput[2] = 1900.f;
        m_contentInput[3] = 1380.f;

        m_anchorInput[0] = 0.f;
        m_anchorInput[1] = 0.f;

        m_positionInput[0] = 0.f;
        m_positionInput[1] = 0.f;

        m_percentageInput[0] = 0.f;
        m_percentageInput[1] = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    VirtualAreaExampleSceneEventReceiver::~VirtualAreaExampleSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaExampleSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaExampleSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VirtualAreaExampleSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        const float viewportWidth = 760.f;
        const float viewportHeight = 480.f;
        const float left = (resolution.getWidthF() - viewportWidth) * 0.5f;
        const float top = (resolution.getHeightF() - viewportHeight) * 0.5f;

        m_viewportInput[0] = left;
        m_viewportInput[1] = top;
        m_viewportInput[2] = left + viewportWidth;
        m_viewportInput[3] = top + viewportHeight;

        this->setupVirtualArea_();
        this->rebuildContent_();
        this->rebuildFrame_();

        ImGUIRenderPtr imguiRender = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUIRender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imguiRender, "invalid create ImGUIRender" );

        imguiRender->setName( STRINGIZE_STRING_LOCAL( "VirtualAreaExampleImGUI" ) );
        imguiRender->setProvider( [this]( const ImGUIRenderProviderInterfacePtr & _provider )
        {
            this->renderControls_( _provider );
        } );

        m_scene->addChild( imguiRender );
        m_imguiRender = imguiRender;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaExampleSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        this->clearNodes_( &m_frameNodes );
        this->clearNodes_( &m_contentNodes );

        if( m_imguiRender != nullptr )
        {
            m_imguiRender->dispose();
            m_imguiRender = nullptr;
        }

        if( m_virtualArea != nullptr )
        {
            m_virtualArea->dispose();
            m_virtualArea = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr VirtualAreaExampleSceneEventReceiver::makeSolidQuad_( const mt::vec2f & _position, const mt::vec2f & _size, float _r, float _g, float _b, float _a )
    {
        SurfaceSolidColorPtr surface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( surface, "invalid create SurfaceSolidColor" );

        surface->setSolidColor( {_r, _g, _b, _a} );
        surface->setSolidSize( _size );

        ShapePtr shape = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( shape, "invalid create ShapeQuadFixed" );

        shape->setSurface( surface );

        TransformationInterface * transformation = shape->getTransformation();
        transformation->setLocalPosition( mt::vec3f( _position.x, _position.y, 0.f ) );

        return shape;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr VirtualAreaExampleSceneEventReceiver::addContentQuad_( const mt::vec2f & _position, const mt::vec2f & _size, float _r, float _g, float _b, float _a )
    {
        NodePtr node = this->makeSolidQuad_( _position, _size, _r, _g, _b, _a );

        m_virtualArea->addContentNode( node, false );
        m_contentNodes.emplace_back( node );

        return node;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaExampleSceneEventReceiver::setupVirtualArea_()
    {
        VirtualAreaPtr virtualArea = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "VirtualArea" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( virtualArea, "invalid create VirtualArea" );

        virtualArea->setName( STRINGIZE_STRING_LOCAL( "VirtualAreaExample" ) );
        virtualArea->setViewport( Viewport( m_viewportInput[0], m_viewportInput[1], m_viewportInput[2], m_viewportInput[3] ) );
        virtualArea->setContentSize( m_contentInput[0], m_contentInput[1], m_contentInput[2], m_contentInput[3] );

        m_scene->addChild( virtualArea );

        m_virtualArea = virtualArea;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaExampleSceneEventReceiver::rebuildFrame_()
    {
        this->clearNodes_( &m_frameNodes );

        const float left = m_viewportInput[0];
        const float top = m_viewportInput[1];
        const float right = m_viewportInput[2];
        const float bottom = m_viewportInput[3];
        const float thickness = 4.f;

        NodePtr topLine = this->makeSolidQuad_( mt::vec2f( left - thickness, top - thickness ), mt::vec2f( right - left + thickness * 2.f, thickness ), 0.95f, 0.72f, 0.18f, 1.f );
        NodePtr bottomLine = this->makeSolidQuad_( mt::vec2f( left - thickness, bottom ), mt::vec2f( right - left + thickness * 2.f, thickness ), 0.95f, 0.72f, 0.18f, 1.f );
        NodePtr leftLine = this->makeSolidQuad_( mt::vec2f( left - thickness, top ), mt::vec2f( thickness, bottom - top ), 0.95f, 0.72f, 0.18f, 1.f );
        NodePtr rightLine = this->makeSolidQuad_( mt::vec2f( right, top ), mt::vec2f( thickness, bottom - top ), 0.95f, 0.72f, 0.18f, 1.f );

        m_scene->addChild( topLine );
        m_scene->addChild( bottomLine );
        m_scene->addChild( leftLine );
        m_scene->addChild( rightLine );

        m_frameNodes.emplace_back( topLine );
        m_frameNodes.emplace_back( bottomLine );
        m_frameNodes.emplace_back( leftLine );
        m_frameNodes.emplace_back( rightLine );
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaExampleSceneEventReceiver::rebuildContent_()
    {
        if( m_virtualArea == nullptr )
        {
            return;
        }

        this->clearNodes_( &m_contentNodes );

        m_virtualArea->setContentSize( m_contentInput[0], m_contentInput[1], m_contentInput[2], m_contentInput[3] );

        const float left = m_contentInput[0];
        const float top = m_contentInput[1];
        const float right = m_contentInput[2];
        const float bottom = m_contentInput[3];
        const float width = s_width( m_contentInput );
        const float height = s_height( m_contentInput );

        NodePtr background = this->makeSolidQuad_( mt::vec2f( left, top ), mt::vec2f( width, height ), 0.08f, 0.10f, 0.13f, 1.f );
        m_virtualArea->addContentNode( background, false );
        m_contentNodes.emplace_back( background );

        this->addContentQuad_( mt::vec2f( 20.f, 20.f ), mt::vec2f( 720.f, 46.f ), 0.02f, 0.80f, 0.95f, 1.f );
        this->addContentQuad_( mt::vec2f( 20.f, 414.f ), mt::vec2f( 720.f, 46.f ), 0.96f, 0.30f, 0.18f, 1.f );
        this->addContentQuad_( mt::vec2f( 20.f, 20.f ), mt::vec2f( 46.f, 440.f ), 0.98f, 0.82f, 0.10f, 1.f );
        this->addContentQuad_( mt::vec2f( 694.f, 20.f ), mt::vec2f( 46.f, 440.f ), 0.42f, 0.95f, 0.32f, 1.f );
        this->addContentQuad_( mt::vec2f( 300.f, 150.f ), mt::vec2f( 160.f, 160.f ), 0.85f, 0.20f, 0.95f, 1.f );
        this->addContentQuad_( mt::vec2f( 335.f, 185.f ), mt::vec2f( 90.f, 90.f ), 1.f, 1.f, 1.f, 1.f );

        const float boundary = 10.f;
        NodePtr boundaryTop = this->makeSolidQuad_( mt::vec2f( left, top ), mt::vec2f( width, boundary ), 0.85f, 0.18f, 0.16f, 1.f );
        NodePtr boundaryBottom = this->makeSolidQuad_( mt::vec2f( left, bottom - boundary ), mt::vec2f( width, boundary ), 0.85f, 0.18f, 0.16f, 1.f );
        NodePtr boundaryLeft = this->makeSolidQuad_( mt::vec2f( left, top ), mt::vec2f( boundary, height ), 0.85f, 0.18f, 0.16f, 1.f );
        NodePtr boundaryRight = this->makeSolidQuad_( mt::vec2f( right - boundary, top ), mt::vec2f( boundary, height ), 0.85f, 0.18f, 0.16f, 1.f );

        m_virtualArea->addContentNode( boundaryTop, false );
        m_virtualArea->addContentNode( boundaryBottom, false );
        m_virtualArea->addContentNode( boundaryLeft, false );
        m_virtualArea->addContentNode( boundaryRight, false );

        m_contentNodes.emplace_back( boundaryTop );
        m_contentNodes.emplace_back( boundaryBottom );
        m_contentNodes.emplace_back( boundaryLeft );
        m_contentNodes.emplace_back( boundaryRight );

        for( float x = left; x <= right; x += 100.f )
        {
            const bool strong = ((int)(x / 100.f) % 5) == 0;
            NodePtr line = this->makeSolidQuad_( mt::vec2f( x, top ), mt::vec2f( strong ? 3.f : 1.f, height ), 0.22f, 0.31f, 0.39f, strong ? 0.88f : 0.48f );
            m_virtualArea->addContentNode( line, false );
            m_contentNodes.emplace_back( line );
        }

        for( float y = top; y <= bottom; y += 100.f )
        {
            const bool strong = ((int)(y / 100.f) % 5) == 0;
            NodePtr line = this->makeSolidQuad_( mt::vec2f( left, y ), mt::vec2f( width, strong ? 3.f : 1.f ), 0.22f, 0.31f, 0.39f, strong ? 0.88f : 0.48f );
            m_virtualArea->addContentNode( line, false );
            m_contentNodes.emplace_back( line );
        }

        for( float x = left; x <= right; x += 200.f )
        {
            NodePtr marker = this->makeSolidQuad_( mt::vec2f( x - 8.f, top + 18.f ), mt::vec2f( 16.f, 16.f ), 0.12f, 0.62f, 0.96f, 1.f );
            m_virtualArea->addContentNode( marker, false );
            m_contentNodes.emplace_back( marker );
        }

        for( float y = top; y <= bottom; y += 200.f )
        {
            NodePtr marker = this->makeSolidQuad_( mt::vec2f( left + 18.f, y - 8.f ), mt::vec2f( 16.f, 16.f ), 0.20f, 0.78f, 0.52f, 1.f );
            m_virtualArea->addContentNode( marker, false );
            m_contentNodes.emplace_back( marker );
        }

        const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
            ->getRandomizer();

        for( uint32_t index = 0; index != 54; ++index )
        {
            const float w = randomizer->getRandomRangef( 34.f, 150.f );
            const float h = randomizer->getRandomRangef( 28.f, 120.f );
            const float x = randomizer->getRandomRangef( left + 42.f, right - w - 42.f );
            const float y = randomizer->getRandomRangef( top + 42.f, bottom - h - 42.f );

            const float r = randomizer->getRandomRangef( 0.25f, 0.95f );
            const float g = randomizer->getRandomRangef( 0.20f, 0.90f );
            const float b = randomizer->getRandomRangef( 0.22f, 0.96f );

            NodePtr quad = this->makeSolidQuad_( mt::vec2f( x, y ), mt::vec2f( w, h ), r, g, b, 0.86f );
            m_virtualArea->addContentNode( quad, false );
            m_contentNodes.emplace_back( quad );
        }

        struct LandmarkDesc
        {
            float px;
            float py;
            float sx;
            float sy;
            float r;
            float g;
            float b;
        };

        const LandmarkDesc landmarks[] = {
            {0.08f, 0.10f, 220.f, 150.f, 0.94f, 0.38f, 0.30f},
            {0.76f, 0.12f, 260.f, 120.f, 0.36f, 0.70f, 0.96f},
            {0.14f, 0.72f, 180.f, 240.f, 0.48f, 0.86f, 0.52f},
            {0.70f, 0.68f, 320.f, 190.f, 0.96f, 0.78f, 0.24f},
            {0.42f, 0.42f, 280.f, 220.f, 0.72f, 0.44f, 0.90f},
        };

        for( const LandmarkDesc & landmark : landmarks )
        {
            const mt::vec2f position( left + width * landmark.px, top + height * landmark.py );
            const mt::vec2f size( landmark.sx, landmark.sy );

            this->addContentQuad_( position, size, landmark.r, landmark.g, landmark.b, 0.70f );
            this->addContentQuad_( mt::vec2f( position.x + 14.f, position.y + 14.f ), mt::vec2f( size.x - 28.f, 18.f ), 1.f, 1.f, 1.f, 0.64f );
            this->addContentQuad_( mt::vec2f( position.x + 14.f, position.y + size.y - 34.f ), mt::vec2f( size.x - 28.f, 20.f ), 0.03f, 0.04f, 0.05f, 0.50f );
            this->addContentQuad_( mt::vec2f( position.x + size.x - 42.f, position.y + 42.f ), mt::vec2f( 28.f, size.y - 84.f ), 0.03f, 0.04f, 0.05f, 0.38f );
        }

        const float axisThickness = 6.f;

        if( left < 0.f && right > 0.f )
        {
            this->addContentQuad_( mt::vec2f( -axisThickness * 0.5f, top ), mt::vec2f( axisThickness, height ), 1.f, 0.24f, 0.24f, 0.92f );
        }

        if( top < 0.f && bottom > 0.f )
        {
            this->addContentQuad_( mt::vec2f( left, -axisThickness * 0.5f ), mt::vec2f( width, axisThickness ), 0.24f, 0.88f, 0.36f, 0.92f );
        }

        if( left < 0.f && right > 0.f && top < 0.f && bottom > 0.f )
        {
            this->addContentQuad_( mt::vec2f( -28.f, -28.f ), mt::vec2f( 56.f, 56.f ), 1.f, 1.f, 1.f, 0.94f );
            this->addContentQuad_( mt::vec2f( -16.f, -16.f ), mt::vec2f( 32.f, 32.f ), 0.02f, 0.03f, 0.04f, 1.f );
        }

        const uint32_t routeCount = 34;
        const float routeStartX = left + 120.f;
        const float routeStartY = top + 140.f;
        const float routeEndX = right - 240.f;
        const float routeEndY = bottom - 180.f;

        for( uint32_t index = 0; index != routeCount; ++index )
        {
            const float t = (float)index / (float)(routeCount - 1);
            const float x = routeStartX + (routeEndX - routeStartX) * t;
            const float y = routeStartY + (routeEndY - routeStartY) * t;
            const bool even = (index % 2) == 0;

            this->addContentQuad_( mt::vec2f( x - 18.f, y - 18.f ), mt::vec2f( 36.f, 36.f ), even ? 0.98f : 0.12f, even ? 0.62f : 0.72f, even ? 0.18f : 0.95f, 0.96f );
        }

        for( uint32_t index = 0; index != 12; ++index )
        {
            const float x = left + 180.f + (float)index * 190.f;
            const float y = top + 300.f + ((index % 3) * 72.f);

            this->addContentQuad_( mt::vec2f( x, y ), mt::vec2f( 110.f, 18.f ), 0.90f, 0.94f, 0.98f, 0.78f );
            this->addContentQuad_( mt::vec2f( x + 92.f, y - 18.f ), mt::vec2f( 36.f, 54.f ), 0.90f, 0.94f, 0.98f, 0.78f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaExampleSceneEventReceiver::clearNodes_( Vector<NodePtr> * const _nodes )
    {
        for( const NodePtr & node : *_nodes )
        {
            if( node != nullptr )
            {
                node->dispose();
            }
        }

        _nodes->clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaExampleSceneEventReceiver::renderControls_( const ImGUIRenderProviderInterfacePtr & _provider )
    {
        MENGINE_UNUSED( _provider );

        if( m_virtualArea == nullptr )
        {
            return;
        }

        ImGui::SetNextWindowSize( ImVec2( 390.f, 720.f ), ImGuiCond_FirstUseEver );

        if( ImGui::Begin( "VirtualArea" ) == false )
        {
            ImGui::End();
            return;
        }

        bool scaleEnable = m_virtualArea->getScaleEnable();
        bool frozen = m_virtualArea->isFrozen();
        bool allowOut = m_virtualArea->getAllowOutOfBounds();
        bool disableInvalid = m_virtualArea->getDisableDragIfInvalid();
        bool defaultHandle = m_virtualArea->getDefaultHandle();

        if( ImGui::Checkbox( "Scale enable", &scaleEnable ) == true )
        {
            m_virtualArea->setScaleEnable( scaleEnable );
        }

        if( ImGui::Checkbox( "Frozen", &frozen ) == true )
        {
            m_virtualArea->freeze( frozen );
        }

        if( ImGui::Checkbox( "Allow out of bounds", &allowOut ) == true )
        {
            m_virtualArea->setAllowOutOfBounds( allowOut );
        }

        if( ImGui::Checkbox( "Disable drag if invalid", &disableInvalid ) == true )
        {
            m_virtualArea->setDisableDragIfInvalid( disableInvalid );
        }

        if( ImGui::Checkbox( "Default handle", &defaultHandle ) == true )
        {
            m_virtualArea->setDefaultHandle( defaultHandle );
        }

        const char * modes[] = {"none", "free", "horizontal", "vertical"};
        int mode = (int)m_virtualArea->getDraggingMode();

        if( ImGui::Combo( "Dragging mode", &mode, modes, 4 ) == true )
        {
            m_virtualArea->setDraggingMode( (EVirtualAreaDragMode)mode );
        }

        ImGui::Separator();

        ImGui::InputFloat4( "Viewport", m_viewportInput );
        ImGui::InputFloat4( "Content", m_contentInput );

        const mt::vec2f & anchor = m_virtualArea->getAnchor();
        m_anchorInput[0] = anchor.x;
        m_anchorInput[1] = anchor.y;

        if( ImGui::DragFloat2( "Anchor", m_anchorInput, 1.f, -2000.f, 2000.f ) == true )
        {
            m_virtualArea->setAnchor( mt::vec2f( m_anchorInput[0], m_anchorInput[1] ) );
        }

        const mt::vec2f & position = m_virtualArea->getPosition();
        m_positionInput[0] = position.x;
        m_positionInput[1] = position.y;

        if( ImGui::DragFloat2( "Position", m_positionInput, 1.f, -4000.f, 4000.f ) == true )
        {
            m_virtualArea->setPosition( mt::vec2f( m_positionInput[0], m_positionInput[1] ) );
        }

        mt::vec2f percentage = m_virtualArea->getPercentage();
        m_percentageInput[0] = percentage.x;
        m_percentageInput[1] = percentage.y;

        if( ImGui::SliderFloat2( "Percentage", m_percentageInput, -1.f, 1.f ) == true )
        {
            m_virtualArea->setPercentage( mt::vec2f( m_percentageInput[0], m_percentageInput[1] ) );
        }

        float scale = m_virtualArea->getScaleFactor();
        const float minScale = 1.f / m_virtualArea->getMaxScaleFactor();

        if( ImGui::SliderFloat( "Scale", &scale, minScale, 1.f, "%.5f"
            , ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat ) == true )
        {
            m_virtualArea->setScale( scale );
        }

        float maxScale = m_virtualArea->getMaxScaleFactor();
        if( ImGui::SliderFloat( "Max scale", &maxScale, 1.f, VIRTUAL_AREA_EXAMPLE_MAX_SCALE_FACTOR_LIMIT, "%.1f"
            , ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat ) == true )
        {
            m_virtualArea->setMaxScaleFactor( maxScale );
        }

        float wheelFactor = m_virtualArea->getWheelScaleFactor();
        if( ImGui::SliderFloat( "Wheel factor", &wheelFactor, 0.f, 1.f ) == true )
        {
            m_virtualArea->setWheelScaleFactor( wheelFactor );
        }

        float friction = m_virtualArea->getFriction();
        if( ImGui::SliderFloat( "Friction", &friction, 0.f, 1.f ) == true )
        {
            m_virtualArea->setFriction( friction );
        }

        float rigidity = m_virtualArea->getRigidity();
        if( ImGui::SliderFloat( "Rigidity", &rigidity, 0.f, 1.f ) == true )
        {
            m_virtualArea->setRigidity( rigidity );
        }

        float threshold = m_virtualArea->getDragStartThreshold();
        if( ImGui::SliderFloat( "Drag threshold", &threshold, 0.f, 200.f ) == true )
        {
            m_virtualArea->setDragStartThreshold( threshold );
        }

        if( ImGui::Button( "Reset position" ) == true )
        {
            const Viewport & viewport = m_virtualArea->getViewport();
            m_virtualArea->setPosition( viewport.begin );
        }

        ImGui::SameLine();

        if( ImGui::Button( "Center" ) == true )
        {
            m_virtualArea->setPercentage( mt::vec2f( 0.5f, 0.5f ) );
        }

        if( ImGui::Button( "Reset scale" ) == true )
        {
            m_virtualArea->setScale( 1.f );
        }

        ImGui::SameLine();

        if( ImGui::Button( "Apply viewport" ) == true )
        {
            m_virtualArea->setViewport( Viewport( m_viewportInput[0], m_viewportInput[1], m_viewportInput[2], m_viewportInput[3] ) );
            this->rebuildFrame_();
        }

        if( ImGui::Button( "Apply content" ) == true )
        {
            this->rebuildContent_();
        }

        ImGui::SameLine();

        if( ImGui::Button( "Randomize content" ) == true )
        {
            this->rebuildContent_();
        }

        ImGui::Separator();

        const mt::vec2f & readPosition = m_virtualArea->getPosition();
        const mt::vec2f readPercentage = m_virtualArea->getPercentage();
        const mt::vec2f & velocity = m_virtualArea->getVelocity();
        const mt::vec2f viewportSize = m_virtualArea->getViewportSize();
        const mt::vec2f contentSize = m_virtualArea->getContentSizeValue();

        ImGui::Text( "Position: %.1f %.1f", readPosition.x, readPosition.y );
        ImGui::Text( "Percentage: %.3f %.3f", readPercentage.x, readPercentage.y );
        ImGui::Text( "Scale factor: %.3f", m_virtualArea->getScaleFactor() );
        ImGui::Text( "Velocity: %.2f %.2f", velocity.x, velocity.y );
        ImGui::Text( "Dragging: %s", m_virtualArea->isDragging() == true ? "true" : "false" );
        ImGui::Text( "Touch count: %u", m_virtualArea->getTouchCount() );
        ImGui::Text( "Viewport size: %.1f %.1f", viewportSize.x, viewportSize.y );
        ImGui::Text( "Content size: %.1f %.1f", contentSize.x, contentSize.y );

        ImGui::End();
    }
    //////////////////////////////////////////////////////////////////////////
}
