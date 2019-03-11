#pragma once

#include "Kernel/Color.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    class Colorable
        : public Mixin
    {
    public:
        Colorable();
        ~Colorable() override;

    public:
        void setPersonalColor( const Color& _color );
        inline const Color & getPersonalColor() const;

        void setPersonalAlpha( float _alpha );
        inline float getPersonalAlpha() const;

        void setLocalColor( const Color& _color );
        inline const Color & getLocalColor() const;

        void setLocalColorR( float _value );
        inline float getLocalColorR() const;

        void setLocalColorG( float _value );
        inline float getLocalColorG() const;

        void setLocalColorB( float _value );
        inline float getLocalColorB() const;

        void setLocalColorRGB( float _r, float _g, float _b );
        void setLocalColorRGBA( float _r, float _g, float _b, float _a );

        void setLocalAlpha( float _value );
        inline float getLocalAlpha() const;

    public:
        inline bool isLocalTransparent() const;
        inline bool isPersonalTransparent() const;

    protected:
        inline const Color & getRelationColor() const;

    public:
        virtual void invalidateColor();
        inline bool isInvalidateColor() const;

    protected:
        const Color& updateRelationColor( const Color& _parentColor ) const;

    protected:
        virtual void _setPersonalColor( const Color& _color );
        virtual void _setPersonalAlpha( float _alpha );

        virtual void _invalidateColor();

    protected:
        virtual void _setLocalTransparent( bool _transparent );
        virtual void _setPersonalTransparent( bool _transparent );

    public:
        virtual const Color & getWorldColor() const = 0;
        virtual void calcTotalColor( Color & _color ) const = 0;
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
    inline bool Colorable::isLocalTransparent() const
    {
        return m_localTransparent;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool Colorable::isPersonalTransparent() const
    {
        return m_personalTransparent;
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & Colorable::getRelationColor() const
    {
        return m_relationColor;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool Colorable::isInvalidateColor() const
    {
        return m_invalidateRelationColor;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const Color & Colorable::getPersonalColor() const
    {
        return m_personalColor;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Colorable::getPersonalAlpha() const
    {
        float alpha = m_personalColor.getA();

        return alpha;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const Color& Colorable::getLocalColor() const
    {
        return m_localColor;
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Colorable::getLocalColorR() const
    {
        return m_localColor.getR();
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Colorable::getLocalColorG() const
    {
        return m_localColor.getG();
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Colorable::getLocalColorB() const
    {
        return m_localColor.getB();
    }
    //////////////////////////////////////////////////////////////////////////
    inline float Colorable::getLocalAlpha() const
    {
        return m_localColor.getA();
    }
}
