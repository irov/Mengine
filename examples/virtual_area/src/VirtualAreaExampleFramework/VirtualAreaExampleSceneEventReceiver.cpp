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

#include "Mosaic/Mosaic.hpp"

#include "Config/StdIO.h"

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

        MosaicRenderPtr mosaicRender = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MosaicRender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mosaicRender, "invalid create MosaicRender" );

        mosaicRender->setName( STRINGIZE_STRING_LOCAL( "VirtualAreaExampleMosaic" ) );
        mosaicRender->setProvider( [this]( Mosaic::Context * _ui )
        {
            this->renderControls_( _ui );
        } );

        m_scene->addChild( mosaicRender );
        m_mosaicRender = mosaicRender;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VirtualAreaExampleSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        this->clearNodes_( &m_frameNodes );
        this->clearNodes_( &m_contentNodes );

        if( m_mosaicRender != nullptr )
        {
            m_mosaicRender->dispose();
            m_mosaicRender = nullptr;
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
        m_virtualArea->clearSnappingPoints();
        m_virtualArea->setSnappingBoundsPoint( mt::vec2f( 0.f, 0.f ) );

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
            m_virtualArea->addSnappingPoint( y );
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
    void VirtualAreaExampleSceneEventReceiver::renderControls_( Mosaic::Context * _ui )
    {
        if( m_virtualArea == nullptr )
        {
            return;
        }

        Mosaic::WindowOptions windowOptions;
        windowOptions.initialBounds = {40.f, 40.f, 390.f, 700.f};
        windowOptions.scrollable = true;

        Mosaic::WindowScope window = Mosaic::window( _ui, "VirtualArea", windowOptions );

        if( window.visible() == false )
        {
            return;
        }


        bool scaleEnable = m_virtualArea->getScaleEnable();
        bool frozen = m_virtualArea->isFrozen();
        bool scrollLocked = m_virtualArea->isScrollLocked();
        bool allowOut = m_virtualArea->getAllowOutOfBounds();
        bool disableInvalid = m_virtualArea->getDisableDragIfInvalid();
        bool defaultHandle = m_virtualArea->getDefaultHandle();

        if( Mosaic::checkbox( _ui, "Scale enable", &scaleEnable ).changed() == true )
        {
            m_virtualArea->setScaleEnable( scaleEnable );
        }

        if( Mosaic::checkbox( _ui, "Frozen", &frozen ).changed() == true )
        {
            m_virtualArea->freeze( frozen );
        }

        if( Mosaic::checkbox( _ui, "Scroll locked", &scrollLocked ).changed() == true )
        {
            m_virtualArea->setScrollLocked( scrollLocked );
        }

        if( Mosaic::checkbox( _ui, "Allow out of bounds", &allowOut ).changed() == true )
        {
            m_virtualArea->setAllowOutOfBounds( allowOut );
        }

        if( Mosaic::checkbox( _ui, "Disable drag if invalid", &disableInvalid ).changed() == true )
        {
            m_virtualArea->setDisableDragIfInvalid( disableInvalid );
        }

        if( Mosaic::checkbox( _ui, "Default handle", &defaultHandle ).changed() == true )
        {
            m_virtualArea->setDefaultHandle( defaultHandle );
        }

        const Mosaic::StringView modes[] = {"none", "free", "horizontal", "vertical"};
        int32_t mode = (int32_t)m_virtualArea->getDraggingMode();

        if( Mosaic::comboBox( _ui, "Dragging mode", &mode, Mosaic::StringViewSpan( modes, 4 ) ).changed() == true )
        {
            m_virtualArea->setDraggingMode( (EVirtualAreaDragMode)mode );
        }

        const Mosaic::StringView snappingModes[] = {"none", "horizontal", "vertical"};
        int32_t snappingMode = (int32_t)m_virtualArea->getSnappingMode();

        if( Mosaic::comboBox( _ui, "Snapping mode", &snappingMode, Mosaic::StringViewSpan( snappingModes, 3 ) ).changed() == true )
        {
            m_virtualArea->setSnappingMode( (EVirtualAreaSnappingMode)snappingMode );
        }

        Mosaic::separator( _ui );

        Mosaic::inputFloatVector( _ui, "Viewport", Mosaic::FloatSpan( m_viewportInput, 4 ) );
        Mosaic::inputFloatVector( _ui, "Content", Mosaic::FloatSpan( m_contentInput, 4 ) );

        const mt::vec2f & anchor = m_virtualArea->getAnchor();
        m_anchorInput[0] = anchor.x;
        m_anchorInput[1] = anchor.y;

        Mosaic::SliderOptions anchorOptions;
        anchorOptions.minimum = -2000.0;
        anchorOptions.maximum = 2000.0;
        anchorOptions.dragSpeed = 1.0;

        if( Mosaic::dragFloatVector( _ui, "Anchor", Mosaic::FloatSpan( m_anchorInput, 2 ), anchorOptions ).changed() == true )
        {
            m_virtualArea->setAnchor( mt::vec2f( m_anchorInput[0], m_anchorInput[1] ) );
        }

        const mt::vec2f & position = m_virtualArea->getPosition();
        m_positionInput[0] = position.x;
        m_positionInput[1] = position.y;

        Mosaic::SliderOptions positionOptions;
        positionOptions.minimum = -4000.0;
        positionOptions.maximum = 4000.0;
        positionOptions.dragSpeed = 1.0;

        if( Mosaic::dragFloatVector( _ui, "Position", Mosaic::FloatSpan( m_positionInput, 2 ), positionOptions ).changed() == true )
        {
            m_virtualArea->setPosition( mt::vec2f( m_positionInput[0], m_positionInput[1] ) );
        }

        mt::vec2f percentage = m_virtualArea->getPercentage();
        m_percentageInput[0] = percentage.x;
        m_percentageInput[1] = percentage.y;

        Mosaic::SliderOptions percentageOptions;
        percentageOptions.minimum = -1.0;
        percentageOptions.maximum = 1.0;

        if( Mosaic::sliderFloatVector( _ui, "Percentage", Mosaic::FloatSpan( m_percentageInput, 2 ), percentageOptions ).changed() == true )
        {
            m_virtualArea->setPercentage( mt::vec2f( m_percentageInput[0], m_percentageInput[1] ) );
        }

        float scale = m_virtualArea->getScaleFactor();
        const float minScale = 1.f / m_virtualArea->getMaxScaleFactor();

        Mosaic::SliderOptions scaleOptions;
        scaleOptions.minimum = (double)minScale;
        scaleOptions.maximum = 1.0;
        scaleOptions.precision = 5;
        scaleOptions.logarithmic = true;
        scaleOptions.roundToFormat = false;

        if( Mosaic::slider( _ui, "Scale", &scale, scaleOptions ).changed() == true )
        {
            m_virtualArea->setScale( scale );
        }

        float maxScale = m_virtualArea->getMaxScaleFactor();

        Mosaic::SliderOptions maxScaleOptions;
        maxScaleOptions.minimum = 1.0;
        maxScaleOptions.maximum = (double)VIRTUAL_AREA_EXAMPLE_MAX_SCALE_FACTOR_LIMIT;
        maxScaleOptions.precision = 1;
        maxScaleOptions.logarithmic = true;
        maxScaleOptions.roundToFormat = false;

        if( Mosaic::slider( _ui, "Max scale", &maxScale, maxScaleOptions ).changed() == true )
        {
            m_virtualArea->setMaxScaleFactor( maxScale );
        }

        float wheelFactor = m_virtualArea->getWheelScaleFactor();

        if( Mosaic::slider( _ui, "Wheel factor", &wheelFactor, 0.f, 1.f ).changed() == true )
        {
            m_virtualArea->setWheelScaleFactor( wheelFactor );
        }

        float friction = m_virtualArea->getFriction();

        if( Mosaic::slider( _ui, "Friction", &friction, 0.f, 1.f ).changed() == true )
        {
            m_virtualArea->setFriction( friction );
        }

        Mosaic::NumericInputOptions frictionBaseOptions;
        frictionBaseOptions.step = 0.0001;
        frictionBaseOptions.fastStep = 0.001;
        frictionBaseOptions.precision = 6;

        float frictionBase = m_virtualArea->getFrictionBase();

        if( Mosaic::inputFloat( _ui, "Friction base", &frictionBase, frictionBaseOptions ).changed() == true )
        {
            m_virtualArea->setFrictionBase( frictionBase );
        }

        Mosaic::NumericInputOptions frictionFactorOptions;
        frictionFactorOptions.step = 0.001;
        frictionFactorOptions.fastStep = 0.01;
        frictionFactorOptions.precision = 6;

        float frictionFactor = m_virtualArea->getFrictionFactor();

        if( Mosaic::inputFloat( _ui, "Friction factor", &frictionFactor, frictionFactorOptions ).changed() == true )
        {
            m_virtualArea->setFrictionFactor( frictionFactor );
        }

        float rigidity = m_virtualArea->getRigidity();

        if( Mosaic::slider( _ui, "Rigidity", &rigidity, 0.f, 1.f ).changed() == true )
        {
            m_virtualArea->setRigidity( rigidity );
        }

        float threshold = m_virtualArea->getDragStartThreshold();

        if( Mosaic::slider( _ui, "Drag threshold", &threshold, 0.f, 200.f ).changed() == true )
        {
            m_virtualArea->setDragStartThreshold( threshold );
        }

        if( Mosaic::button( _ui, "Reset position" ).clicked() == true )
        {
            const Viewport & viewport = m_virtualArea->getViewport();
            m_virtualArea->setPosition( viewport.begin );
        }

        Mosaic::sameLine( _ui );

        if( Mosaic::button( _ui, "Center" ).clicked() == true )
        {
            m_virtualArea->setPercentage( mt::vec2f( 0.5f, 0.5f ) );
        }

        if( Mosaic::button( _ui, "Reset scale" ).clicked() == true )
        {
            m_virtualArea->setScale( 1.f );
        }

        Mosaic::sameLine( _ui );

        if( Mosaic::button( _ui, "Apply viewport" ).clicked() == true )
        {
            m_virtualArea->setViewport( Viewport( m_viewportInput[0], m_viewportInput[1], m_viewportInput[2], m_viewportInput[3] ) );
            this->rebuildFrame_();
        }

        if( Mosaic::button( _ui, "Apply content" ).clicked() == true )
        {
            this->rebuildContent_();
        }

        Mosaic::sameLine( _ui );

        if( Mosaic::button( _ui, "Randomize content" ).clicked() == true )
        {
            this->rebuildContent_();
        }

        Mosaic::separator( _ui );

        const mt::vec2f & readPosition = m_virtualArea->getPosition();
        const mt::vec2f readPercentage = m_virtualArea->getPercentage();
        const mt::vec2f & velocity = m_virtualArea->getVelocity();
        const mt::vec2f viewportSize = m_virtualArea->getViewportSize();
        const mt::vec2f contentSize = m_virtualArea->getContentSizeValue();

        m_readout.clear();

        Char readout[128] = {'\0'};

        MENGINE_SNPRINTF( readout, sizeof( readout ) - 1, "Position: %.1f %.1f", readPosition.x, readPosition.y );
        m_readout.emplace_back( readout );

        MENGINE_SNPRINTF( readout, sizeof( readout ) - 1, "Percentage: %.3f %.3f", readPercentage.x, readPercentage.y );
        m_readout.emplace_back( readout );

        MENGINE_SNPRINTF( readout, sizeof( readout ) - 1, "Scale factor: %.3f", m_virtualArea->getScaleFactor() );
        m_readout.emplace_back( readout );

        MENGINE_SNPRINTF( readout, sizeof( readout ) - 1, "Velocity: %.2f %.2f", velocity.x, velocity.y );
        m_readout.emplace_back( readout );

        MENGINE_SNPRINTF( readout, sizeof( readout ) - 1, "Dragging: %s", m_virtualArea->isDragging() == true ? "true" : "false" );
        m_readout.emplace_back( readout );

        MENGINE_SNPRINTF( readout, sizeof( readout ) - 1, "Touch count: %u", m_virtualArea->getTouchCount() );
        m_readout.emplace_back( readout );

        MENGINE_SNPRINTF( readout, sizeof( readout ) - 1, "Viewport size: %.1f %.1f", viewportSize.x, viewportSize.y );
        m_readout.emplace_back( readout );

        MENGINE_SNPRINTF( readout, sizeof( readout ) - 1, "Content size: %.1f %.1f", contentSize.x, contentSize.y );
        m_readout.emplace_back( readout );

        uint32_t readoutIndex = 0;

        for( const String & line : m_readout )
        {
            Mosaic::Scope lineScope = Mosaic::scope( _ui, Mosaic::Key( readoutIndex++ ) );

            Mosaic::text( _ui, Mosaic::StringView( line.c_str(), line.size() ) );
        }

    }
    //////////////////////////////////////////////////////////////////////////
}
