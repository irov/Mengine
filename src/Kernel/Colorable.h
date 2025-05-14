#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Color.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Colorable
        : public Mixin
    {
    public:
        Colorable();
        ~Colorable() override;

    public:
        void setPersonalColor( const Color & _color );
        MENGINE_INLINE const Color & getPersonalColor() const;

        void setPersonalAlpha( float _alpha );
        MENGINE_INLINE float getPersonalAlpha() const;

        void setLocalColor( const Color & _color );
        MENGINE_INLINE const Color & getLocalColor() const;

        void setLocalColorR( float _value );
        MENGINE_INLINE float getLocalColorR() const;

        void setLocalColorG( float _value );
        MENGINE_INLINE float getLocalColorG() const;

        void setLocalColorB( float _value );
        MENGINE_INLINE float getLocalColorB() const;

        void setLocalColorRGB( float _r, float _g, float _b );
        void setLocalColorRGBA( float _r, float _g, float _b, float _a );

        void setLocalAlpha( float _value );
        MENGINE_INLINE float getLocalAlpha() const;

    public:
        MENGINE_INLINE bool isLocalTransparent() const;
        MENGINE_INLINE bool isPersonalTransparent() const;

    protected:
        MENGINE_INLINE const Color & getRelationColor() const;

    public:
        virtual void invalidateColor() const;
        MENGINE_INLINE bool isInvalidateColor() const;

    protected:
        const Color & updateRelationColor( const Color & _parentColor ) const;

    protected:
        virtual void _setPersonalColor( const Color & _color );
        virtual void _setPersonalAlpha( float _alpha );

        virtual void _invalidateColor() const;

    protected:
        virtual void _setLocalTransparent( bool _transparent );
        virtual void _setPersonalTransparent( bool _transparent );

    public:
        virtual const Color & getWorldColor() const = 0;
        virtual void calcTotalColor( Color * const _color ) const = 0;
        virtual bool isSolidColor() const = 0;

    protected:
        void updateLocalTransparent_();
        void updatePersonalTransparent_();

    protected:
        Color m_personalColor;
        Color m_localColor;

        mutable Color m_relationColor;
        mutable bool m_invalidateRelationColor;

        bool m_personalTransparent;
        bool m_localTransparent;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Colorable> ColorablePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Colorable::isLocalTransparent() const
    {
        return m_localTransparent;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Colorable::isPersonalTransparent() const
    {
        return m_personalTransparent;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Color & Colorable::getRelationColor() const
    {
        return m_relationColor;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Colorable::isInvalidateColor() const
    {
        return m_invalidateRelationColor;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Color & Colorable::getPersonalColor() const
    {
        return m_personalColor;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float Colorable::getPersonalAlpha() const
    {
        float alpha = m_personalColor.getA();

        return alpha;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const Color & Colorable::getLocalColor() const
    {
        return m_localColor;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float Colorable::getLocalColorR() const
    {
        return m_localColor.getR();
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float Colorable::getLocalColorG() const
    {
        return m_localColor.getG();
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float Colorable::getLocalColorB() const
    {
        return m_localColor.getB();
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE float Colorable::getLocalAlpha() const
    {
        return m_localColor.getA();
    }
    //////////////////////////////////////////////////////////////////////////
}
