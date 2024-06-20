#pragma once

#include "ShapeQuad.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ShapeQuadFlex
        : public ShapeQuad
    {
        DECLARE_FACTORABLE( ShapeQuadFlex );

    public:
        ShapeQuadFlex();
        ~ShapeQuadFlex() override;

    public:
        void setCustomSize( const mt::vec2f & _customSize );
        void removeCustomSize();
        bool hasCustomSize() const;
        const mt::vec2f & getCustomSize() const;

        void setCenterAlign( bool _centerAlign );
        bool getCenterAlign() const;

        void setFlipX( bool _flipX );
        bool getFlipX() const;

        void setFlipY( bool _flipY );
        bool getFlipY() const;

        void setUV( const mt::uv4f & _uv );
        const mt::uv4f & getUV() const;

        void setPercentVisibility( const mt::vec4f & _percent );
        const mt::vec4f & getPercentVisibility() const;

        void setTextureUVOffset( const mt::vec2f & _offset );
        const mt::vec2f & getTextureUVOffset() const;

        void setTextureUVScale( const mt::vec2f & _scale );
        const mt::vec2f & getTextureUVScale() const;

    protected:
        void updateVerticesLocal() const override;

    protected:
        mt::vec2f m_customSize;

        mt::uv4f m_uv;

        mt::vec4f m_percentVisibility;

        mt::vec2f m_textureUVOffset;
        mt::vec2f m_textureUVScale;

        bool m_centerAlign;

        bool m_flipX;
        bool m_flipY;

        bool m_solid;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<ShapeQuadFlex> ShapeQuadFlexPtr;
    //////////////////////////////////////////////////////////////////////////
}