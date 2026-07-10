#pragma once

#include "FigmaInterface.h"
#include "ResourceFigma.h"

#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderScissorInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"

#include "Figma/Figma.h"

#include <memory>

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
        , protected BaseUpdation
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Figma );
        DECLARE_UNKNOWABLE();
        DECLARE_VISITABLE( Node );
        DECLARE_UPDATABLE();
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

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
        bool inputPointerMove( float _x, float _y ) override;
        bool inputPointerDown( float _x, float _y, uint32_t _button ) override;
        bool inputPointerUp( float _x, float _y, uint32_t _button ) override;
        bool inputPointerCancel( float _x, float _y ) override;
        bool inputKeyDown( uint32_t _keyCode ) override;
        bool inputKeyUp( uint32_t _keyCode ) override;

    protected:
        bool _compile() override;
        void _release() override;
        void _dispose() override;

    protected:
        void update( const UpdateContext * _context ) override;
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        bool inputPointer_( ::Figma::EPointerEventType _type, float _x, float _y, ::Figma::EPointerButton _button );
        bool inputKey_( ::Figma::EKeyEventType _type, uint32_t _keyCode );

    protected:
        struct TextureCacheDesc
        {
            RenderTextureInterfacePtr texture;
            String signature;
        };

        RenderTextureInterfacePtr getBatchTexture_( const ::Figma::RenderListInterface * _renderList, const ::Figma::RenderBatchDesc & _batch, uint32_t _batchIndex ) const;
        RenderTextureInterfacePtr createAssetTexture_( ::Figma::DocumentInterface * _document, const ::Figma::RenderBatchDesc & _batch ) const;
        RenderTextureInterfacePtr createGeneratedTexture_( const ::Figma::RenderListInterface * _renderList, uint32_t _batchIndex, String * const _signature ) const;
        RenderTextureInterfacePtr createTextureFromPixels_( uint32_t _width, uint32_t _height, const void * _pixels, size_t _pitch ) const;
        void clearTextureCache_() const;

    protected:
        ResourceFigmaPtr m_resourceFigma;
        ::Figma::PlayerInterface * m_player;
        mt::vec2f m_viewportSize;
        float m_viewportScale;
        String m_startFrameId;
        String m_fontSearchPath;
        mutable std::unique_ptr<Detail::FigmaTextRasterizer> m_textRasterizer;

        mutable Vector<VectorRenderVertex2D> m_renderVertices;
        mutable Vector<VectorRenderIndex> m_renderIndices;
        mutable Vector<RenderMaterialInterfacePtr> m_renderMaterials;
        mutable Vector<RenderScissorInterfacePtr> m_renderScissors;
        mutable Map<String, TextureCacheDesc> m_textureCache;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Figma> FigmaPtr;
    //////////////////////////////////////////////////////////////////////////
}
