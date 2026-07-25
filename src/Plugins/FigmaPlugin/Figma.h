#pragma once

#include "FigmaInterface.h"
#include "FigmaEventReceiverInterface.h"
#include "ResourceFigma.h"

#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/PickerInputHandlerInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BasePicker.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"

#include "figma/figma.hpp"

namespace Mengine
{
    namespace Detail
    {
        class FigmaTextRasterizer;
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class Figma
        : public Node
        , public UnknownFigmaInterface
        , public PickerInputHandlerInterface
        , protected BaseUpdation
        , protected BaseRender
        , protected BaseTransformation
        , protected BaseEventation
        , protected BasePicker
    {
        DECLARE_FACTORABLE( Figma );
        DECLARE_UNKNOWABLE();
        DECLARE_VISITABLE( Node );
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();
        DECLARE_EVENTABLE();
        DECLARE_PICKERABLE();

    public:
        Figma();
        ~Figma() override;

    public:
        void setResourceFigma( const ResourcePtr & _resource ) override;
        const ResourcePtr & getResourceFigma() const override;

    public:
        void setViewportSize( const mt::vec2f & _size ) override;
        const mt::vec2f & getViewportSize() const override;

        void setViewportScale( float _scale ) override;
        float getViewportScale() const override;

    public:
        void setStartFrameId( const String & _startFrameId ) override;
        const String & getStartFrameId() const override;

    public:
        void setFontSearchPath( const String & _fontSearchPath ) override;
        const String & getFontSearchPath() const override;

    public:
        void setPlaybackRate( float _playbackRate ) override;
        float getPlaybackRate() const override;
        bool replay() override;

    public:
        bool inputPointerMove( float _x, float _y ) override;
        bool inputPointerDown( float _x, float _y, uint32_t _button ) override;
        bool inputPointerUp( float _x, float _y, uint32_t _button ) override;
        bool inputPointerCancel( float _x, float _y ) override;
        bool inputKeyDown( uint32_t _keyCode ) override;
        bool inputKeyUp( uint32_t _keyCode ) override;

    public:
        bool setBindingText( const String & _key, const String & _value ) override;
        bool setBindingNumber( const String & _key, double _value ) override;
        bool setBindingVisible( const String & _key, bool _value ) override;
        bool setBindingEnabled( const String & _key, bool _value ) override;
        bool setBindingImage( const String & _key, const String & _assetId ) override;
        bool setBindingState( const String & _key, bool _value ) override;
        bool setBindingValue( const String & _key, const FigmaBindingValue & _value ) override;
        bool clearBindingValue( const String & _key ) override;

    public:
        bool navigateToFrame( const String & _targetFrameId ) override;
        bool openOverlay( const String & _targetFrameId ) override;
        bool closeOverlay() override;
        bool goBack() override;

    protected:
        bool _compile() override;
        void _release() override;
        void _dispose() override;

        bool _activate() override;
        bool _afterActivate() override;
        void _deactivate() override;

    protected:
        void update( const UpdateContext * _context ) override;
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        bool pick( const mt::vec2f & _point, const RenderContext * _context ) const override;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        Scriptable * getPickerScriptable() override;
#endif
        Eventable * getPickerEventable() override;
        PickerInputHandlerInterface * getPickerInputHandler() override;

    public:
        bool handleKeyEvent( const RenderContext * _context, const InputKeyEvent & _event ) override;
        bool handleTextEvent( const RenderContext * _context, const InputTextEvent & _event ) override;
        bool handleMouseButtonEvent( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const RenderContext * _context, const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const RenderContext * _context, const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const RenderContext * _context, const InputMouseWheelEvent & _event ) override;
        bool handleMouseEnter( const RenderContext * _context, const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event ) override;

    protected:
        static figma_result_t FIGMA_CALL routeTriggerCallback_( void * _userData, const figma_trigger_event_t * _event );
        static figma_result_t FIGMA_CALL routeActionCallback_( void * _userData, const figma_action_event_t * _event, figma_action_response_t * _response );
        static void FIGMA_CALL onFrameChangedCallback_( void * _userData, figma_string_view_t _previousFrameId, figma_string_view_t _currentFrameId );
        static void FIGMA_CALL onOverlayOpenedCallback_( void * _userData, figma_string_view_t _frameId );
        static void FIGMA_CALL onOverlayClosedCallback_( void * _userData, figma_string_view_t _frameId );
        static void FIGMA_CALL onStateChangedCallback_( void * _userData, figma_string_view_t _sourceNodeId, figma_string_view_t _previousStateId, figma_string_view_t _currentStateId );

        figma_result_t routeTrigger_( const figma_trigger_event_t & _event );
        figma_result_t routeAction_( const figma_action_event_t & _event, figma_action_response_t * const _response );
        void onFrameChanged_( figma_string_view_t _previousFrameId, figma_string_view_t _currentFrameId );
        void onOverlayOpened_( figma_string_view_t _frameId );
        void onOverlayClosed_( figma_string_view_t _frameId );
        void onStateChanged_( figma_string_view_t _sourceNodeId, figma_string_view_t _previousStateId, figma_string_view_t _currentStateId );

    protected:
        bool inputPointer_( figma_pointer_event_type_t _type, uint32_t _pointerId, float _x, float _y, figma_pointer_button_t _button, figma_input_modifier_flags_t _modifiers, figma_input_dispatch_result_t * const _dispatch );
        bool inputKey_( figma_key_event_type_t _type, uint32_t _keyCode, figma_input_modifier_flags_t _modifiers, figma_input_dispatch_result_t * const _dispatch );
        bool screenToLocal_( const RenderContext * _context, const mt::vec2f & _screenPoint, mt::vec2f * const _localPoint ) const;
        bool applyBindingValue_( const String & _key, const FigmaBindingValue & _value );
        bool updatePlayerViewport_();

    protected:
        struct TextureCacheDesc
        {
            RenderTextureInterfacePtr texture;
            String signature;
        };

        struct RenderLayerTargetDesc
        {
            RenderTargetInterfacePtr target;
            RenderTextureInterfacePtr texture;
            RenderMaterialInterfacePtr material;
            RenderVertex2D vertices[4];
            uint32_t width = 0;
            uint32_t height = 0;
        };
        typedef Map<String, FigmaBindingValue> MapFigmaBindingValue;
        typedef IntrusivePtr<Detail::FigmaTextRasterizer> FigmaTextRasterizerPtr;
        typedef Vector<VectorRenderVertex2D> VectorRenderVertexBuffer;
        typedef Vector<VectorRenderIndex> VectorRenderIndexBuffer;
        typedef Vector<RenderMaterialInterfacePtr> VectorRenderMaterial;
        typedef Vector<RenderScissorInterfacePtr> VectorRenderScissor;
        typedef Map<String, TextureCacheDesc> MapTextureCache;
        typedef Map<uint32_t, RenderLayerTargetDesc> MapRenderLayerTarget;

        RenderTextureInterfacePtr getBatchTexture_( const figma_render_list_t * _renderList, const figma_render_batch_desc_t & _batch, uint32_t _batchIndex ) const;
        RenderTextureInterfacePtr createAssetTexture_( figma_document_t * _document, const figma_render_batch_desc_t & _batch ) const;
        RenderTextureInterfacePtr createGeneratedTexture_( const figma_render_list_t * _renderList, uint32_t _batchIndex, const figma_render_generated_texture_desc_t & _desc ) const;
        RenderTextureInterfacePtr createTextureFromPixels_( uint32_t _width, uint32_t _height, const void * _pixels, size_t _pitch ) const;
        void clearTextureCache_() const;
        RenderLayerTargetDesc * ensureRenderLayerTarget_( uint32_t _layerId, uint32_t _width, uint32_t _height, float _contentWidth, float _contentHeight ) const;
        void clearRenderLayerTargets_() const;
        void renderLayerTarget_( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, RenderLayerTargetDesc * _target, float _opacity ) const;

    protected:
        ResourceFigmaPtr m_resourceFigma;
        figma_player_t * m_player;
        mt::vec2f m_viewportSize;
        float m_viewportScale;
        float m_playbackRate;
        String m_startFrameId;
        String m_fontSearchPath;
        String m_actionTargetFrameId;
        MapFigmaBindingValue m_bindingValues;
        mutable FigmaTextRasterizerPtr m_textRasterizer;

        mutable VectorRenderVertexBuffer m_renderVertices;
        mutable VectorRenderIndexBuffer m_renderIndices;
        mutable VectorRenderMaterial m_renderMaterials;
        mutable VectorRenderScissor m_renderScissors;
        mutable MapTextureCache m_textureCache;
        mutable MapRenderLayerTarget m_renderLayerTargets;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Figma> FigmaPtr;
    //////////////////////////////////////////////////////////////////////////
}
