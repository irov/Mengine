#pragma once

#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderTargetInterface.h"
#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderProgramVariableInterface.h"

#include "Kernel/Node.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/ConstString.h"

#include "math/vec2.h"
#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PostProcessFx3D
        : public Node
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( PostProcessFx3D );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        PostProcessFx3D();
        ~PostProcessFx3D() override;

    public:
        void setSize( const mt::vec2f & _size );
        const mt::vec2f & getSize() const;

    public:
        void setMaterialName( const ConstString & _materialName );
        const ConstString & getMaterialName() const;

    public:
        void setExposure( float _exposure );
        float getExposure() const;

        void setVignette( float _strength, float _softness );
        float getVignetteStrength() const;
        float getVignetteSoftness() const;

        void setChromaticOffset( float _pixels );
        float getChromaticOffset() const;

        void setHitFlash( const mt::vec3f & _color, float _intensity );
        const mt::vec3f & getHitFlashColor() const;
        float getHitFlashIntensity() const;

    public:
        const RenderVertex2D * getVerticesWM() const;
        const RenderMaterialInterfacePtr & getMaterial() const;
        const RenderProgramVariableInterfacePtr & getProgramVariable() const;
        const RenderIndex * getIndices() const;

    protected:
        bool _activate() override;
        void _deactivate() override;
        void _dispose() override;

    protected:
        const RenderInterfacePtr & makeTargetRender( const RenderContext * _context ) const override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void _invalidateColor() const override;
        void _invalidateWorldMatrix() const override;

    protected:
        bool createRenderTarget_();
        void clearRenderTarget_();

        void updateVerticesWM_() const;
        void updateVerticesColor_() const;
        void packPixelUniforms_() const;

    protected:
        ConstString m_materialName;

        mt::vec2f m_size;        

        RenderTargetInterfacePtr m_sceneTarget;
        RenderTextureInterfacePtr m_sceneTexture;

        RenderMaterialInterfacePtr m_material;

        RenderProgramVariableInterfacePtr m_programVariable;

        RenderInterfacePtr m_renderTargetRender;

        mutable RenderVertex2D m_verticesWM[4];
        RenderIndex m_indices[6] = {0, 1, 2, 0, 2, 3};

        float m_exposure;
        float m_vignetteStrength;
        float m_vignetteSoftness;
        float m_chromaticOffset;
        mt::vec3f m_hitFlashColor;
        float m_hitFlashIntensity;

        mutable bool m_invalidateVerticesWM;
        mutable bool m_invalidateVerticesColor;

        mutable bool m_pixelUniformsRegistered;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<PostProcessFx3D> PostProcessFx3DPtr;
    //////////////////////////////////////////////////////////////////////////
}
