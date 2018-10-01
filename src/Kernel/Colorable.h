#pragma once

#include "Kernel/ColourValue.h"
#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    class Colorable
        : public Mixin
    {
    public:
        Colorable();
        ~Colorable() override;

    public:
        void setPersonalColor( const ColourValue& _color );
        inline const ColourValue & getPersonalColor() const;

        void setPersonalAlpha( float _alpha );
        inline float getPersonalAlpha() const;

        void setLocalColor( const ColourValue& _color );
        inline const ColourValue & getLocalColor() const;

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
        inline const ColourValue & getRelationColor() const;

    public:
        void invalidateColor();
        inline bool isInvalidateColor() const;

    protected:
        const ColourValue& updateRelationColor( const ColourValue& _parentColor ) const;

    protected:
        virtual void _setPersonalColor( const ColourValue& _color );
        virtual void _setPersonalAlpha( float _alpha );

        virtual void _invalidateColor();

    protected:
        virtual void _setLocalTransparent( bool _transparent );
        virtual void _setPersonalTransparent( bool _transparent );

    public:
        virtual const ColourValue & getWorldColor() const = 0;
        virtual void calcTotalColor( ColourValue & _color ) const = 0;
        virtual bool isSolidColor() const = 0;

    protected:
        void updateLocalTransparent_();
        void updatePersonalTransparent_();

    protected:
        ColourValue m_personalColor;
        ColourValue m_localColor;
                
        mutable ColourValue m_relationColor;
        mutable bool m_invalidateColor;

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
    const ColourValue & Colorable::getRelationColor() const
    {
        return m_relationColor;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool Colorable::isInvalidateColor() const
    {
        return m_invalidateColor;
    }
    //////////////////////////////////////////////////////////////////////////
    inline const ColourValue & Colorable::getPersonalColor() const
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
    inline const ColourValue& Colorable::getLocalColor() const
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
