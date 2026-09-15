#include "MosaicModule.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/RenderBatchInterface.h"
#include "Interface/TimeSystemInterface.h"

#include "MosaicKeyCodeTable.h"
#include "MosaicRendererAdapter.h"
#include "MosaicService.h"

#include "Kernel/ColorHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"
#include "Kernel/UnicodeHelper.h"

#include "Config/StdMath.h"

#include "math/utils.h"
#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static uint32_t getMosaicPointerButton_( EMouseButtonCode _button )
        {
            switch( _button )
            {
            case MC_LBUTTON:
                return (uint32_t)Mosaic::PointerButton::Primary;
            case MC_MBUTTON:
                return (uint32_t)Mosaic::PointerButton::Middle;
            case MC_RBUTTON:
                return (uint32_t)Mosaic::PointerButton::Secondary;
            case MC_X1BUTTON:
                return (uint32_t)Mosaic::PointerButton::Auxiliary1;
            case MC_X2BUTTON:
                return (uint32_t)Mosaic::PointerButton::Auxiliary2;
            }

            return (uint32_t)Mosaic::PointerButton::Primary;
        }
        //////////////////////////////////////////////////////////////////////////
        static EMaterial getMosaicMaterialId_( Mosaic::BlendMode _blend )
        {
            switch( _blend )
            {
            case Mosaic::BlendMode::Alpha:
            case Mosaic::BlendMode::PremultipliedAlpha:
                return EM_TEXTURE_BLEND;
            case Mosaic::BlendMode::Additive:
                return EM_TEXTURE_INTENSIVE;
            case Mosaic::BlendMode::Multiply:
                return EM_TEXTURE_MULTIPLY;
            case Mosaic::BlendMode::Opaque:
                return EM_TEXTURE_SOLID;
            }

            return EM_TEXTURE_BLEND;
        }
        //////////////////////////////////////////////////////////////////////////
        static EMaterialBlendMode getMosaicBlendMode_( Mosaic::BlendMode _blend )
        {
            switch( _blend )
            {
            case Mosaic::BlendMode::Alpha:
            case Mosaic::BlendMode::PremultipliedAlpha:
                return EMB_NORMAL;
            case Mosaic::BlendMode::Additive:
                return EMB_ADD;
            case Mosaic::BlendMode::Multiply:
                return EMB_MULTIPLY;
            case Mosaic::BlendMode::Opaque:
                return EMB_NORMAL;
            }

            return EMB_NORMAL;
        }
        //////////////////////////////////////////////////////////////////////////
        static ColorValue_ARGB makeMosaicColor_( uint32_t _color )
        {
            uint8_t a8 = (uint8_t)(_color >> 24);
            uint8_t r8 = (uint8_t)(_color >> 16);
            uint8_t g8 = (uint8_t)(_color >> 8);
            uint8_t b8 = (uint8_t)(_color);

            ColorValue_ARGB color = Helper::makeRGBA8( r8, g8, b8, a8 );

            return color;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicModule::MosaicModule()
        : m_context( nullptr )
        , m_mesh( nullptr )
        , m_timestamp( 0.0 )
        , m_contentResolution( 1280.f, 720.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicModule::~MosaicModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicModule::_initializeModule()
    {
        m_context = MOSAIC_SERVICE()
            ->getContext();

        if( m_context == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicModule::_finalizeModule()
    {
        m_mesh = nullptr;
        m_context = nullptr;

        m_vertices.clear();
        m_indices.clear();
        m_scissors.clear();
        m_materials.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::PointerState & MosaicModule::acquirePointer_( uint32_t _touchId )
    {
        for( Mosaic::PointerState & pointer : m_input.pointers )
        {
            if( pointer.id == _touchId )
            {
                return pointer;
            }
        }

        Mosaic::PointerState pointer;
        pointer.id = _touchId;
        pointer.type = Mosaic::PointerType::Mouse;

        m_input.pointers.push_back( pointer );

        return m_input.pointers.back();
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicModule::updateModifiers_( const InputSpecialData & _special )
    {
        m_input.modifiers.shift = _special.isShift;
        m_input.modifiers.control = _special.isControl;
        m_input.modifiers.alt = _special.isAlt;
        m_input.modifiers.super = _special.isSpecial;
        m_input.modifiers.primary = (_special.isSpecial == true || _special.isControl == true);
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicModule::clearInputEdges_()
    {
        for( Mosaic::PointerState & pointer : m_input.pointers )
        {
            pointer.delta = {};
            pointer.pressed = 0;
            pointer.released = 0;
            pointer.pressPositionValid = 0;
            pointer.clickCounts = {};
            pointer.clickCount = 0;
        }

        m_input.keyboard.clear();
        m_input.text.clear();
        m_input.ime.clear();
        m_input.wheel = {};
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicModule::_handleFocus( bool _focus )
    {
        m_input.windowFocused = _focus;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicModule::_handleKeyEvent( const InputKeyEvent & _event )
    {
        this->updateModifiers_( _event.special );

        Mosaic::KeyCode key = Helper::getMosaicKeyCode( _event.code );

        if( key != Mosaic::KeyCode::Unknown )
        {
            Mosaic::KeyEvent keyEvent;
            keyEvent.key = key;
            keyEvent.pressed = _event.isDown;
            keyEvent.released = (_event.isDown == false);
            keyEvent.repeat = _event.isRepeat;
            keyEvent.modifiers = m_input.modifiers;

            m_input.keyboard.push_back( keyEvent );
        }

        return m_inputCapture.keyboard;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicModule::_handleTextEvent( const InputTextEvent & _event )
    {
        Char utf8[MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE * 4 + 1] = {'\0'};

        size_t utf8Size = 0;

        if( Helper::unicodeToUtf8( _event.text, utf8, sizeof( utf8 ) - 1, &utf8Size ) == true )
        {
            if( utf8Size != 0 )
            {
                m_input.text.push_back( Mosaic::String( utf8, utf8Size ) );
            }
        }

        return m_inputCapture.text;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicModule::_handleMouseButtonEvent( const InputMouseButtonEvent & _event )
    {
        this->updateModifiers_( _event.special );

        Mosaic::PointerState & pointer = this->acquirePointer_( (uint32_t)_event.touchId );

        pointer.position = {_event.position.screen.x * m_contentResolution.x, _event.position.screen.y * m_contentResolution.y};
        pointer.pressure = _event.pressure;

        uint32_t button = Detail::getMosaicPointerButton_( _event.button );
        uint8_t mask = (uint8_t)(1U << button);

        if( _event.isDown == true )
        {
            pointer.down |= mask;
            pointer.pressed |= mask;
            pointer.pressPositionValid |= mask;
            pointer.pressPositions[button] = pointer.position;
            pointer.clickCounts[button] = 1;
            pointer.clickCount = 1;
        }
        else
        {
            pointer.down &= (uint8_t)(~mask);
            pointer.released |= mask;
        }

        return m_inputCapture.pointer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicModule::_handleMouseMove( const InputMouseMoveEvent & _event )
    {
        this->updateModifiers_( _event.special );

        Mosaic::PointerState & pointer = this->acquirePointer_( (uint32_t)_event.touchId );

        Mosaic::Vec2 position = {_event.position.screen.x * m_contentResolution.x, _event.position.screen.y * m_contentResolution.y};

        pointer.delta = {position.x - pointer.position.x, position.y - pointer.position.y};
        pointer.position = position;
        pointer.pressure = _event.pressure;

        return m_inputCapture.pointer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicModule::_handleMouseWheel( const InputMouseWheelEvent & _event )
    {
        this->updateModifiers_( _event.special );

        float scroll = (float)_event.scroll;
        float distance = mt::clamp( 16.f, StdMath::fabsf( scroll ) * 4.f, 48.f );

        m_input.wheel.y += scroll < 0.f ? -distance : distance;

        return m_inputCapture.pointer;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicModule::_update()
    {
        if( m_context == nullptr )
        {
            return;
        }

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        m_contentResolution.x = (float)contentResolution.getWidth();
        m_contentResolution.y = (float)contentResolution.getHeight();

        double timestamp = TIME_SYSTEM()
            ->getElapsedTime() * 0.001;

        double deltaTime = timestamp - m_timestamp;

        if( deltaTime <= 0.0 || deltaTime > 1.0 )
        {
            deltaTime = 1.0 / 60.0;
        }

        m_timestamp = timestamp;

        m_input.deltaTime = (float)deltaTime;
        m_input.timestamp = timestamp;

        Mosaic::Viewport viewport;
        viewport.id = 1;
        viewport.bounds = {0.f, 0.f, m_contentResolution.x, m_contentResolution.y};

        Mosaic::beginFrame( m_context, m_input, viewport );

        m_frameProviders = MOSAIC_SERVICE()
            ->getProviders();

        for( const MosaicProviderDesc & desc : m_frameProviders )
        {
            desc.provider( m_context );
        }

        m_frameProviders.clear();

        const Mosaic::Frame & frame = Mosaic::endFrame( m_context );

        m_inputCapture = frame.inputCapture;

        for( const Mosaic::String & diagnostic : frame.diagnostics )
        {
            LOGGER_ERROR( "mosaic diagnostic: %s"
                , diagnostic.c_str()
            );
        }

        m_mesh = nullptr;

        if( m_bridge.prepare( frame ) == true )
        {
            m_mesh = m_bridge.renderData();
        }
        else
        {
            Mosaic::StringView error = m_bridge.lastError();

            if( error.empty() == false )
            {
                LOGGER_ERROR( "mosaic graphics bridge: %.*s"
                    , (int32_t)error.size()
                    , error.data()
                );
            }
        }

        this->clearInputEdges_();
    }
    //////////////////////////////////////////////////////////////////////////
    const MosaicRenderScissorPtr & MosaicModule::acquireScissor_( const Mosaic::Rect & _clip, const RenderScissorInterface * _parent ) const
    {
        for( const MosaicScissorDesc & desc : m_scissors )
        {
            if( desc.clip.x == _clip.x && desc.clip.y == _clip.y && desc.clip.width == _clip.width && desc.clip.height == _clip.height )
            {
                return desc.scissor;
            }
        }

        MosaicRenderScissorPtr scissor = Helper::makeFactorableUnique<MosaicRenderScissor>( MENGINE_DOCUMENT_FACTORABLE );

        mt::mat4f wm;
        mt::ident_m4( &wm );

        scissor->setScissorViewport( wm, _clip, _parent );

        MosaicScissorDesc desc;
        desc.clip = _clip;
        desc.scissor = scissor;

        m_scissors.emplace_back( desc );

        const MosaicScissorDesc & added = m_scissors.back();

        return added.scissor;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicModule::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        if( m_mesh == nullptr )
        {
            return;
        }

        MosaicRendererAdapter * renderer = static_cast<MosaicService *>(MOSAIC_SERVICE())->getRendererAdapter();

        m_scissors.clear();
        m_materials.clear();

        m_vertices.resize( m_mesh->batches.size() );
        m_indices.resize( m_mesh->batches.size() );

        uint32_t batchIndex = 0;

        for( const Mosaic::RenderBatch & batch : m_mesh->batches )
        {
            VectorMosaicVertices & batchVertices = m_vertices[batchIndex];
            VectorMosaicIndices & batchIndices = m_indices[batchIndex];

            ++batchIndex;

            if( batch.vertexCount == 0 || batch.indexCount == 0 )
            {
                continue;
            }

            if( batch.vertexCount >= MENGINE_RENDER_VERTEX_MAX_BATCH )
            {
                LOGGER_ERROR( "mosaic batch too large '%u' vertices"
                    , batch.vertexCount
                );

                continue;
            }

            const Mosaic::RenderState * state = m_mesh->renderStates.empty() == true || batch.renderKey == 0 || batch.renderKey > m_mesh->renderStates.size()
                ? nullptr
                : &m_mesh->renderStates[(size_t)(batch.renderKey - 1)];

            RenderTextureInterfacePtr texture;

            if( state != nullptr && state->texture != 0 )
            {
                texture = renderer->lookupTexture( state->texture );
            }

            RenderContext context = *_context;

            if( state != nullptr && state->clip.width > 0.f && state->clip.height > 0.f )
            {
                const MosaicRenderScissorPtr & scissor = this->acquireScissor_( state->clip, _context->scissor );

                context.scissor = scissor.get();
            }

            batchVertices.resize( batch.vertexCount );

            for( uint32_t index = 0; index != batch.vertexCount; ++index )
            {
                const Mosaic::RenderVertex & source = m_mesh->vertices[batch.vertexOffset + index];
                RenderVertex2D & vertex = batchVertices[index];

                vertex.position.x = source.position.x;
                vertex.position.y = source.position.y;
                vertex.position.z = 0.f;

                vertex.color = Detail::makeMosaicColor_( source.color );

                if( texture != nullptr )
                {
                    mt::uv4_quad_point( vertex.uv + 0, texture->getUV(), mt::vec2f( source.uv.x, source.uv.y ) );
                }
                else
                {
                    vertex.uv[0].x = source.uv.x;
                    vertex.uv[0].y = source.uv.y;
                }

                vertex.uv[1].x = 0.f;
                vertex.uv[1].y = 0.f;
            }

            batchIndices.resize( batch.indexCount );

            for( uint32_t index = 0; index != batch.indexCount; ++index )
            {
                uint32_t sourceIndex = m_mesh->indices[batch.indexOffset + index];

                MENGINE_ASSERTION_FATAL( sourceIndex < batch.vertexCount, "mosaic batch index '%u' out of vertex range '%u'"
                    , sourceIndex
                    , batch.vertexCount
                );

                batchIndices[index] = (RenderIndex)sourceIndex;
            }

            Mosaic::BlendMode blend = state != nullptr ? state->blend : Mosaic::BlendMode::PremultipliedAlpha;

            RenderMaterialInterfacePtr material;

            if( texture != nullptr )
            {
                bool glyphTexture = state != nullptr && renderer->isGlyphTexture( state->texture ) == true;
                bool glyphPremultiply = glyphTexture == true && renderer->isGlyphTexturePremultiply( state->texture ) == true;

                EMaterial materialId = glyphPremultiply == true
                    ? EM_TEXTURE_BLEND_PREMULTIPLY
                    : Detail::getMosaicMaterialId_( blend );

                RenderTextureInterfacePtr textures[] = {texture};

                material = RENDERMATERIAL_SERVICE()
                    ->getMaterial3( materialId, PT_TRIANGLELIST, textures, 1, MENGINE_DOCUMENT_FACTORABLE );
            }
            else
            {
                EMaterialBlendMode blendMode = Detail::getMosaicBlendMode_( blend );

                material = RENDERMATERIAL_SERVICE()
                    ->getSolidMaterial( blendMode, false );
            }

            if( material == nullptr )
            {
                continue;
            }

            m_materials.emplace_back( material );

            const RenderVertex2D * vertexData = batchVertices.data();
            const RenderIndex * indexData = batchIndices.data();

            _renderPipeline->addRenderObject( &context, material, nullptr, vertexData, batch.vertexCount, indexData, batch.indexCount, nullptr, EROF_NONE, MENGINE_DOCUMENT_FACTORABLE );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
